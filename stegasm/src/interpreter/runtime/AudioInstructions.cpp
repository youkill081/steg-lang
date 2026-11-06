#include "interpreter/runtime/InstructionViewImpl.h"
#include "interpreter/exceptions.h"

#include <algorithm>
#include <bit>
#include <cmath>
#include <cstring>

namespace {
using Format = AudioBackend::Format;
using Input = AudioBackend::Input;

class Descriptor
{
public:
    Descriptor(Runtime& runtime, uint32_t address, size_t words)
    {
        auto& block = runtime.memory.get_block(address);
        const size_t offset = address - block.start;
        if (offset > block.data.size() || words * 4 > block.data.size() - offset)
            throw InterpreterError("Audio descriptor outside memory");
        _data = block.data.data() + offset;
    }

    uint32_t operator[](size_t word) const
    {
        const auto* p = _data + word * 4;
        return (uint32_t(p[0]) << 24) | (uint32_t(p[1]) << 16) |
               (uint32_t(p[2]) << 8) | uint32_t(p[3]);
    }

    void write(size_t word, uint32_t value)
    {
        auto* p = _data + word * 4;
        for (unsigned i = 0; i < 4; ++i) p[i] = uint8_t(value >> (24 - 8 * i));
    }

    Format format(size_t start = 0) const
    {
        return {(*this)[start], (*this)[start+1], (*this)[start+2]};
    }

    AudioBackend::Settings settings() const
    {
        return {floating(0), floating(1), floating(2)};
    }

    float floating(size_t word) const { return std::bit_cast<float>((*this)[word]); }

    template<size_t N>
    void write(const std::array<uint32_t, N>& words)
    {
        for (size_t i = 0; i < N; ++i) write(i, words[i]);
    }

private:
    uint8_t* _data;
};

File& file(Runtime& rt, uint32_t id)
{
    if (id >= UINT16_MAX) throw InterpreterError("Invalid audio file descriptor");
    return *rt.files.get_file(descriptor(id));
}

Input bytes(File& f) { return f.get_file_data().get_buffer(); }

void result(Runtime& rt, InstructionView view, uint32_t value)
{
    rt.registries.write(view.r1(), value);
}

float sample(const uint8_t* p, uint32_t bits)
{
    if (bits == 8) return (int(*p) - 128) / 128.0f;
    if (bits == 16) {
        int16_t value;
        std::memcpy(&value, p, sizeof value);
        return value / 32768.0f;
    }
    float value;
    std::memcpy(&value, p, sizeof value);
    return std::isfinite(value) ? value : 0;
}

bool fits(Input data, Format format, uint32_t start, uint32_t count)
{
    return format.valid() && data.size() % format.stride() == 0 &&
           uint64_t(start) + count <= data.size() / format.stride();
}
}

void instr_AUDIO_INIT(Runtime& rt, InstructionView v) { result(rt, v, rt.audio_backend.init()); }
void instr_AUDIO_CLOSE(Runtime& rt, InstructionView) { rt.audio_backend.close(); }
void instr_AUDIO_UPDATE(Runtime& rt, InstructionView) { rt.audio_backend.update(); }
void instr_AUDIO_WAIT(Runtime& rt, InstructionView v) { rt.audio_backend.wait(v.get_r1(rt)); }
void instr_AUDIO_MASTER_SET(Runtime& rt, InstructionView v) { rt.audio_backend.master(std::bit_cast<float>(v.get_r1(rt))); }

void instr_AUDIO_SOURCE_LOAD(Runtime& rt, InstructionView v)
{
    const Descriptor job(rt, v.get_r3(rt), 2);
    auto& input = file(rt, v.get_r2(rt));
    uint32_t handle = 0;
    try {
        if (job[1] <= 1) handle = rt.audio_backend.load(bytes(input), job[0], job[1] != 0);
    } catch (const FileError&) {
        // An optional asset that cannot be read is a failed source load.
    }
    result(rt, v, handle);
}

void instr_AUDIO_SOURCE_PCM(Runtime& rt, InstructionView v)
{
    const Descriptor job(rt, v.get_r3(rt), 3);
    result(rt, v, rt.audio_backend.source_pcm(bytes(file(rt, v.get_r2(rt))), job.format()));
}

void instr_AUDIO_SOURCE_INFO(Runtime& rt, InstructionView v)
{
    Descriptor out(rt, v.get_r3(rt), 5);
    std::array<uint32_t, 5> info{};
    const bool ok = rt.audio_backend.source_info(v.get_r2(rt), info);
    if (ok) out.write(info);
    result(rt, v, ok ? AudioBackend::Ok : AudioBackend::InvalidHandle);
}

void instr_AUDIO_SOURCE_FREE(Runtime& rt, InstructionView v) { rt.audio_backend.free_source(v.get_r1(rt)); }

void instr_AUDIO_PLAY(Runtime& rt, InstructionView v)
{
    const Descriptor job(rt, v.get_r3(rt), 4);
    const auto flags = job[3];
    result(rt, v, flags <= 3 ? rt.audio_backend.play(v.get_r2(rt), job.settings(), (flags & 1) != 0, (flags & 2) != 0) : 0);
}

void instr_AUDIO_VOICE_SET(Runtime& rt, InstructionView v)
{
    const Descriptor job(rt, v.get_r3(rt), 3);
    result(rt, v, rt.audio_backend.set(v.get_r2(rt), job.settings()));
}

void instr_AUDIO_VOICE_CONTROL(Runtime& rt, InstructionView v)
{
    result(rt, v, rt.audio_backend.control(v.get_r2(rt), v.get_r3(rt)));
}

void instr_AUDIO_VOICE_SEEK(Runtime& rt, InstructionView v)
{
    result(rt, v, rt.audio_backend.seek(v.get_r2(rt), v.get_r3(rt)));
}

void instr_AUDIO_VOICE_INFO(Runtime& rt, InstructionView v)
{
    Descriptor out(rt, v.get_r3(rt), 3);
    std::array<uint32_t, 3> info{};
    const bool ok = rt.audio_backend.voice_info(v.get_r2(rt), info);
    if (ok) out.write(info);
    result(rt, v, ok ? AudioBackend::Ok : AudioBackend::InvalidHandle);
}

void instr_AUDIO_STREAM_CREATE(Runtime& rt, InstructionView v)
{
    const Descriptor job(rt, v.get_r2(rt), 5);
    result(rt, v, rt.audio_backend.create_stream(job.format(), job[3], job[4]));
}

void instr_AUDIO_STREAM_WRITE(Runtime& rt, InstructionView v)
{
    auto& input = file(rt, v.get_r3(rt));
    const auto data = bytes(input);
    const auto start = input.get_cursor();
    if (start > data.size()) { result(rt, v, 0); return; }
    // The cursor lets a producer retry a partially accepted block.
    uint32_t accepted = 0;
    const auto count = rt.audio_backend.write_stream(v.get_r2(rt), data.subspan(start), accepted);
    if (accepted) input.seek_cursor(start + accepted);
    result(rt, v, count);
}

void instr_AUDIO_STREAM_INFO(Runtime& rt, InstructionView v)
{
    Descriptor out(rt, v.get_r3(rt), 3);
    std::array<uint32_t, 3> info{};
    const bool ok = rt.audio_backend.stream_info(v.get_r2(rt), info);
    if (ok) out.write(info);
    result(rt, v, ok ? AudioBackend::Ok : AudioBackend::InvalidHandle);
}

void instr_AUDIO_STREAM_PLAY(Runtime& rt, InstructionView v)
{
    result(rt, v, rt.audio_backend.play_stream(v.get_r2(rt)));
}

void instr_AUDIO_STREAM_CONTROL(Runtime& rt, InstructionView v)
{
    result(rt, v, rt.audio_backend.control_stream(v.get_r2(rt), v.get_r3(rt)));
}

void instr_AUDIO_DECODE(Runtime& rt, InstructionView v)
{
    Descriptor job(rt, v.get_r3(rt), 6);
    auto pcm = AudioBackend::decode(bytes(file(rt, v.get_r1(rt))), job[0]);
    job.write(1, pcm.data.empty() ? AudioBackend::InvalidValue : AudioBackend::Ok);
    if (pcm.data.empty()) return;
    job.write(2, pcm.format.rate);
    job.write(3, pcm.format.channels);
    job.write(4, pcm.format.bits);
    job.write(5, pcm.frames());
    file(rt, v.get_r2(rt)).replace_data(std::move(pcm.data));
}

void instr_AUDIO_PCM_CONVERT(Runtime& rt, InstructionView v)
{
    Descriptor job(rt, v.get_r3(rt), 8);
    auto pcm = AudioBackend::convert(bytes(file(rt, v.get_r1(rt))), job.format(), job.format(3));
    job.write(6, pcm.data.empty() ? AudioBackend::InvalidValue : AudioBackend::Ok);
    if (pcm.data.empty()) return;
    job.write(7, pcm.frames());
    file(rt, v.get_r2(rt)).replace_data(std::move(pcm.data));
}

void instr_AUDIO_PCM_ENCODE(Runtime& rt, InstructionView v)
{
    Descriptor job(rt, v.get_r3(rt), 5);
    const auto codec = job[3];
    if (codec != 1) { job.write(4, AudioBackend::Unsupported); return; }
    auto encoded = AudioBackend::encode_wav(bytes(file(rt, v.get_r1(rt))), job.format());
    job.write(4, encoded.empty() ? AudioBackend::InvalidValue : AudioBackend::Ok);
    if (!encoded.empty()) file(rt, v.get_r2(rt)).replace_data(std::move(encoded));
}

void instr_AUDIO_PCM_MIX(Runtime& rt, InstructionView v)
{
    Descriptor job(rt, v.get_r3(rt), 8);
    const auto f = job.format();
    auto& output = file(rt, v.get_r1(rt));
    auto& input = file(rt, v.get_r2(rt));
    const auto data = bytes(input);
    const auto first = job[3], dest = job[4], count = job[5];
    const float gain = job.floating(6);
    if (f.bits != 32 || !std::isfinite(gain) ||
        !fits(data, f, first, count) || !fits(bytes(output), f, dest, count)) {
        job.write(7, AudioBackend::InvalidValue);
        return;
    }
    if (!count) { job.write(7, AudioBackend::Ok); return; }
    auto out = output.mutable_bytes();
    const auto* src = data.data() + size_t(first) * f.stride();
    auto* target = out.data() + size_t(dest) * f.stride();
    const size_t samples = size_t(count) * f.channels;
    const bool reverse = &output == &input && dest > first;
    for (size_t i = 0; i < samples; ++i) {
        const size_t at = (reverse ? samples - 1 - i : i) * sizeof(float);
        const float value = float(std::clamp(double(sample(target + at, 32)) +
            double(sample(src + at, 32)) * gain, -1.0, 1.0));
        std::memcpy(target + at, &value, sizeof value);
    }
    job.write(7, AudioBackend::Ok);
}

void instr_AUDIO_PCM_MEASURE(Runtime& rt, InstructionView v)
{
    Descriptor job(rt, v.get_r3(rt), 7);
    const auto data = bytes(file(rt, v.get_r2(rt)));
    const auto format = job.format();
    const auto first = job[3], count = job[4];
    if (!fits(data, format, first, count) || !count) {
        result(rt, v, AudioBackend::InvalidValue);
        return;
    }
    const auto* start = data.data() + size_t(first) * format.stride();
    const size_t samples = size_t(count) * format.channels;
    double energy = 0;
    float peak = 0;
    for (size_t i = 0; i < samples; ++i) {
        const float value = sample(start + i * (format.bits / 8), format.bits);
        peak = std::max(peak, std::abs(value));
        energy += double(value) * value;
    }
    job.write(5, std::bit_cast<uint32_t>(peak));
    job.write(6, std::bit_cast<uint32_t>(float(std::sqrt(energy / samples))));
    result(rt, v, AudioBackend::Ok);
}
