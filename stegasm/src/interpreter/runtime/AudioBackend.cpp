#include "AudioBackend.h"

#include <algorithm>
#include <bit>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstring>
#include <limits>
#include <thread>
#include <unordered_map>

#include "raylib.h"

namespace {
using Format = AudioBackend::Format;
using Settings = AudioBackend::Settings;
using Bytes = AudioBackend::Bytes;
using Input = AudioBackend::Input;

static_assert(std::endian::native == std::endian::little);
constexpr uint32_t unknown = UINT32_MAX;

const char* extension(uint32_t codec)
{
    constexpr const char* formats[] = {nullptr, ".wav", ".ogg", ".mp3", ".qoa", ".flac", ".xm", ".mod"};
    return codec < std::size(formats) ? formats[codec] : nullptr;
}

bool valid_pcm(Input data, Format format)
{
    return format.valid() && !data.empty() && data.size() <= INT_MAX && data.size() % format.stride() == 0;
}

bool valid_settings(Settings settings)
{
    return std::isfinite(settings.gain) && settings.gain >= 0 && settings.gain <= 1 &&
           std::isfinite(settings.pan) && settings.pan >= -1 && settings.pan <= 1 &&
           std::isfinite(settings.pitch) && settings.pitch >= 0.125f && settings.pitch <= 8;
}

struct OwnedWave {
    Wave wave{};
    ~OwnedWave() { if (wave.data) UnloadWave(wave); }
};

AudioBackend::Pcm copy_wave(const Wave& wave)
{
    const Format format{wave.sampleRate, wave.channels, wave.sampleSize};
    const uint64_t size = uint64_t(wave.frameCount) * format.stride();
    if (!wave.data || !format.valid() || !size || size > INT_MAX) return {};
    const auto* first = static_cast<const uint8_t*>(wave.data);
    return {format, Bytes(first, first + size)};
}

struct Source {
    Bytes pcm;
    Bytes encoded;
    uint32_t codec = 0;
    Format format{};
    uint32_t frames = unknown;
    Sound sound{};
    ~Source() { if (sound.stream.buffer) UnloadSound(sound); }
};

struct Stream {
    Format format{};
    Bytes queue;
    Bytes scratch;
    uint32_t block = 0, capacity = 0, head = 0, used = 0, underruns = 0, voice = 0;
    bool ended = false;
};

struct Voice {
    std::shared_ptr<Source> source;
    std::shared_ptr<Stream> stream;
    Sound sound{};
    Music music{};
    AudioStream raw{};
    bool paused = false, stopped = false, primed = false;
    bool auto_release = false;
    unsigned drain = 0;

    ~Voice()
    {
        if (sound.stream.buffer) UnloadSoundAlias(sound);
        if (music.stream.buffer) UnloadMusicStream(music);
        if (raw.buffer) UnloadAudioStream(raw);
        if (stream) stream->voice = 0;
    }

    bool playing() const
    {
        if (paused || stopped) return false;
        if (raw.buffer) return primed && IsAudioStreamPlaying(raw);
        if (music.stream.buffer) return IsMusicStreamPlaying(music);
        return IsSoundPlaying(sound);
    }

    void apply(Settings settings) const
    {
        const float pan = settings.pan;
        if (raw.buffer) {
            SetAudioStreamVolume(raw, settings.gain);
            SetAudioStreamPan(raw, pan);
            SetAudioStreamPitch(raw, settings.pitch);
        } else if (music.stream.buffer) {
            SetMusicVolume(music, settings.gain);
            SetMusicPan(music, pan);
            SetMusicPitch(music, settings.pitch);
        } else {
            SetSoundVolume(sound, settings.gain);
            SetSoundPan(sound, pan);
            SetSoundPitch(sound, settings.pitch);
        }
    }
};
}

struct AudioBackend::State {
    std::unordered_map<uint32_t, std::shared_ptr<Source>> sources;
    std::unordered_map<uint32_t, std::unique_ptr<Voice>> voices;
    std::unordered_map<uint32_t, std::shared_ptr<Stream>> streams;
    bool ready = false;
};

bool AudioBackend::Format::valid() const
{
    return rate >= 1000 && rate <= 384000 && channels >= 1 && channels <= 2 &&
           (bits == 8 || bits == 16 || bits == 32);
}

AudioBackend::AudioBackend() : _state(std::make_unique<State>()) {}
AudioBackend::AudioBackend(AudioBackend&&) noexcept = default;
AudioBackend::~AudioBackend() { if (_state) close(); }

uint32_t AudioBackend::init()
{
    if (!_state->ready) {
        InitAudioDevice();
        _state->ready = IsAudioDeviceReady();
    }
    return _state->ready ? Ok : Unavailable;
}

void AudioBackend::close()
{
    _state->voices.clear();
    _state->streams.clear();
    _state->sources.clear();
    if (_state->ready) CloseAudioDevice();
    _state->ready = false;
}

void AudioBackend::master(float gain)
{
    if (_state->ready && std::isfinite(gain)) SetMasterVolume(std::clamp(gain, 0.0f, 1.0f));
}

AudioBackend::Pcm AudioBackend::decode(Input bytes, uint32_t codec)
{
    const auto* ext = extension(codec);
    if (!ext || bytes.empty() || bytes.size() > INT_MAX) return {};
    OwnedWave wave{LoadWaveFromMemory(ext, bytes.data(), int(bytes.size()))};
    return copy_wave(wave.wave);
}

AudioBackend::Pcm AudioBackend::convert(Input bytes, Format from, Format to)
{
    if (!valid_pcm(bytes, from) || !to.valid()) return {};
    const auto frames = uint32_t(bytes.size() / from.stride());
    const auto max_frames = (uint64_t(frames) * to.rate + from.rate - 1) / from.rate;
    if (max_frames * to.stride() > INT_MAX) return {};
    OwnedWave owned{{frames, from.rate, from.bits, from.channels, MemAlloc(unsigned(bytes.size()))}};
    auto& wave = owned.wave;
    if (!wave.data) return {};
    std::memcpy(wave.data, bytes.data(), bytes.size());
    WaveFormat(&wave, to.rate, to.bits, to.channels);
    if (wave.sampleRate != to.rate || wave.channels != to.channels || wave.sampleSize != to.bits) return {};
    return copy_wave(wave);
}

AudioBackend::Bytes AudioBackend::encode_wav(Input bytes, Format format)
{
    if (!valid_pcm(bytes, format)) return {};
    const bool floating = format.bits == 32;
    Bytes result;
    result.reserve(bytes.size() + 57);
    auto tag = [&](const char* name) { result.insert(result.end(), name, name + 4); };
    auto word = [&](uint32_t value, unsigned size) {
        for (unsigned i = 0; i < size; ++i) result.push_back(uint8_t(value >> (i * 8)));
    };
    const auto size = uint32_t(bytes.size());
    tag("RIFF"); word(size + (size & 1) + (floating ? 48 : 36), 4); tag("WAVE");
    tag("fmt "); word(16, 4); word(floating ? 3 : 1, 2); word(format.channels, 2);
    word(format.rate, 4); word(format.rate * format.stride(), 4);
    word(format.stride(), 2); word(format.bits, 2);
    if (floating) { tag("fact"); word(4, 4); word(size / format.stride(), 4); }
    tag("data"); word(size, 4);
    result.insert(result.end(), bytes.begin(), bytes.end());
    if (size & 1) result.push_back(0);
    return result;
}

uint32_t AudioBackend::load(Input bytes, uint32_t codec, bool streaming)
{
    if (!extension(codec) || bytes.empty() || bytes.size() > INT_MAX) return 0;
    auto source = std::make_shared<Source>();
    if (streaming) {
        source->encoded.assign(bytes.begin(), bytes.end());
        source->codec = codec;
    } else {
        auto pcm = decode(bytes, codec);
        if (pcm.data.empty()) return 0;
        source->format = pcm.format;
        source->frames = pcm.frames();
        source->pcm = std::move(pcm.data);
    }
    const auto id = _next++;
    _state->sources.emplace(id, std::move(source));
    return id;
}

uint32_t AudioBackend::source_pcm(Input bytes, Format format)
{
    if (!valid_pcm(bytes, format)) return 0;
    auto source = std::make_shared<Source>();
    source->format = format;
    source->frames = uint32_t(bytes.size() / format.stride());
    source->pcm.assign(bytes.begin(), bytes.end());
    const auto id = _next++;
    _state->sources.emplace(id, std::move(source));
    return id;
}

bool AudioBackend::source_info(uint32_t source, std::array<uint32_t, 5>& info)
{
    const auto it = _state->sources.find(source);
    if (it == _state->sources.end()) return false;
    const auto& item = *it->second;
    const auto f = item.format;
    info = {f.rate, f.channels, f.bits, item.frames, uint32_t(item.codec != 0)};
    return true;
}

void AudioBackend::free_source(uint32_t source)
{
    _state->sources.erase(source);
}

uint32_t AudioBackend::play(uint32_t id, Settings settings, bool loop, bool auto_release)
{
    const auto it = _state->sources.find(id);
    if (!_state->ready || it == _state->sources.end() || !valid_settings(settings)) return 0;
    auto voice = std::make_unique<Voice>();
    voice->source = it->second;
    voice->auto_release = auto_release;
    auto& source = *voice->source;
    if (source.codec) {
        voice->music = LoadMusicStreamFromMemory(extension(source.codec), source.encoded.data(), int(source.encoded.size()));
        if (!IsMusicValid(voice->music)) return 0;
        voice->music.looping = loop;
        const auto& stream = voice->music.stream;
        source.format = {stream.sampleRate, stream.channels, stream.sampleSize};
        source.frames = voice->music.frameCount;
    } else {
        if (loop) return 0;
        if (!source.sound.stream.buffer) {
            const auto f = source.format;
            source.sound = LoadSoundFromWave({source.frames, f.rate, f.bits, f.channels, source.pcm.data()});
            if (!IsSoundValid(source.sound)) return 0;
            Bytes{}.swap(source.pcm);
        }
        voice->sound = LoadSoundAlias(source.sound);
        if (!IsSoundValid(voice->sound)) return 0;
    }
    voice->apply(settings);
    if (source.codec) PlayMusicStream(voice->music);
    else PlaySound(voice->sound);
    const auto handle = _next++;
    _state->voices.emplace(handle, std::move(voice));
    return handle;
}

uint32_t AudioBackend::set(uint32_t id, Settings settings)
{
    const auto it = _state->voices.find(id);
    if (it == _state->voices.end()) return InvalidHandle;
    if (!valid_settings(settings)) return InvalidValue;
    it->second->apply(settings);
    return Ok;
}

uint32_t AudioBackend::control(uint32_t id, uint32_t command)
{
    const auto it = _state->voices.find(id);
    if (it == _state->voices.end()) return InvalidHandle;
    auto& voice = *it->second;
    if (command == 3) { _state->voices.erase(it); return Ok; }
    if (command > 3) return InvalidValue;
    if (voice.stopped || (command == 1 && !voice.paused)) return Ok;
    if (voice.raw.buffer) {
        if (command == 0) PauseAudioStream(voice.raw);
        if (command == 1 && voice.primed) ResumeAudioStream(voice.raw);
        if (command == 2) StopAudioStream(voice.raw);
    } else if (voice.music.stream.buffer) {
        if (command == 0) PauseMusicStream(voice.music);
        if (command == 1) ResumeMusicStream(voice.music);
        if (command == 2) StopMusicStream(voice.music);
    } else {
        if (command == 0) PauseSound(voice.sound);
        if (command == 1) ResumeSound(voice.sound);
        if (command == 2) StopSound(voice.sound);
    }
    voice.paused = command == 0;
    voice.stopped = command == 2;
    return Ok;
}

uint32_t AudioBackend::seek(uint32_t id, uint32_t frame)
{
    const auto it = _state->voices.find(id);
    if (it == _state->voices.end()) return InvalidHandle;
    auto& voice = *it->second;
    if (!voice.music.stream.buffer || voice.source->codec >= 6) return Unsupported;
    if (frame >= voice.music.frameCount || voice.stopped) return InvalidValue;
    SeekMusicStream(voice.music, float(frame) / voice.music.stream.sampleRate);
    return Ok;
}

bool AudioBackend::voice_info(uint32_t id, std::array<uint32_t, 3>& info)
{
    const auto it = _state->voices.find(id);
    if (it == _state->voices.end()) return false;
    auto& voice = *it->second;
    info = {voice.paused ? 2u : (voice.playing() ? 1u : 0u), unknown, unknown};
    if (voice.music.stream.buffer) {
        const double frame = double(GetMusicTimePlayed(voice.music)) * voice.music.stream.sampleRate;
        info[1] = uint32_t(std::clamp(frame, 0.0, double(voice.music.frameCount)));
        info[2] = voice.music.frameCount;
    } else if (voice.source) info[2] = voice.source->frames;
    else if (!voice.stopped && !voice.primed && !voice.paused) info[0] = 3;
    return true;
}

uint32_t AudioBackend::create_stream(Format format, uint32_t capacity, uint32_t block)
{
    if (!format.valid() || block < 256 || block > 32768 || capacity < block || capacity > 1048576) return 0;
    auto stream = std::make_shared<Stream>();
    stream->format = format;
    stream->capacity = capacity;
    stream->block = block;
    stream->queue.resize(size_t(capacity) * format.stride());
    stream->scratch.resize(size_t(block) * format.stride());
    const auto id = _next++;
    _state->streams.emplace(id, std::move(stream));
    return id;
}

uint32_t AudioBackend::write_stream(uint32_t id, Input bytes, uint32_t& consumed_bytes)
{
    consumed_bytes = 0;
    const auto it = _state->streams.find(id);
    if (it == _state->streams.end() || it->second->ended) return 0;
    auto& stream = *it->second;
    const auto stride = stream.format.stride();
    if (bytes.size() % stride) return 0;
    const auto count = uint32_t(std::min<size_t>(bytes.size() / stride, stream.capacity - stream.used));
    if (!count) return 0;
    const auto tail = (stream.head + stream.used) % stream.capacity;
    const auto part = std::min(count, stream.capacity - tail);
    const auto* data = bytes.data();
    std::memcpy(stream.queue.data() + tail * stride, data, part * stride);
    if (part < count) std::memcpy(stream.queue.data(), data + part * stride, (count - part) * stride);
    stream.used += count;
    consumed_bytes = count * stride;
    return count;
}

bool AudioBackend::stream_info(uint32_t id, std::array<uint32_t, 3>& info)
{
    const auto it = _state->streams.find(id);
    if (it == _state->streams.end()) return false;
    const auto& s = *it->second;
    info = {s.capacity - s.used, s.used, s.underruns};
    return true;
}

uint32_t AudioBackend::play_stream(uint32_t id)
{
    const auto it = _state->streams.find(id);
    if (!_state->ready || it == _state->streams.end() || it->second->voice) return 0;
    auto voice = std::make_unique<Voice>();
    const auto& s = *it->second;
    SetAudioStreamBufferSizeDefault(int(s.block));
    voice->raw = LoadAudioStream(s.format.rate, s.format.bits, s.format.channels);
    SetAudioStreamBufferSizeDefault(0);
    if (!IsAudioStreamValid(voice->raw)) return 0;
    voice->stream = it->second;
    const auto handle = _next++;
    voice->stream->voice = handle;
    _state->voices.emplace(handle, std::move(voice));
    return handle;
}

uint32_t AudioBackend::control_stream(uint32_t id, uint32_t command)
{
    const auto it = _state->streams.find(id);
    if (it == _state->streams.end()) return InvalidHandle;
    auto& stream = *it->second;
    switch (command) {
    case 0: stream.ended = true; break;
    case 1:
        if (stream.voice) control(stream.voice, 3);
        stream.head = stream.used = stream.underruns = 0;
        stream.ended = false;
        break;
    case 2:
        if (stream.voice) control(stream.voice, 3);
        _state->streams.erase(it);
        break;
    default: return InvalidValue;
    }
    return Ok;
}

void AudioBackend::update()
{
    for (auto it = _state->voices.begin(); it != _state->voices.end();) {
        const auto current = it++;
        const auto id = current->first;
        auto& voice = *current->second;
        if (voice.auto_release && !voice.paused && !voice.playing()) {
            _state->voices.erase(current);
            continue;
        }
        if (voice.paused || voice.stopped) continue;
        if (voice.music.stream.buffer) {
            if (voice.playing()) UpdateMusicStream(voice.music);
            continue;
        }
        if (!voice.stream) continue;
        auto& s = *voice.stream;
        const auto stride = s.format.stride();
        for (unsigned i = 0; i < 2 && IsAudioStreamProcessed(voice.raw); ++i) {
            if (!s.ended && s.used < s.block && !voice.primed) break;
            const auto count = std::min(s.used, s.block);
            if (!count && s.ended && (!voice.primed || voice.drain == 2)) {
                control(id, 2);
                break;
            }
            if (!count && s.ended) ++voice.drain;
            if (!voice.primed) {
                PlayAudioStream(voice.raw);
                voice.primed = true;
            }
            const auto part = std::min(count, s.capacity - s.head);
            const auto* block = s.queue.data() + s.head * stride;
            if (part != s.block) {
                std::memcpy(s.scratch.data(), block, part * stride);
                if (part < count)
                    std::memcpy(s.scratch.data() + part * stride, s.queue.data(), (count - part) * stride);
                std::fill(s.scratch.begin() + count * stride, s.scratch.end(), s.format.bits == 8 ? 128 : 0);
                block = s.scratch.data();
            }
            UpdateAudioStream(voice.raw, block, int(s.block));
            s.head = (s.head + count) % s.capacity;
            s.used -= count;
            if (count < s.block && !s.ended) ++s.underruns;
        }
    }
}

void AudioBackend::wait(uint32_t milliseconds)
{
    using namespace std::chrono;
    const auto end = steady_clock::now() + std::chrono::milliseconds(milliseconds);
    do {
        update();
        const auto now = steady_clock::now();
        if (now >= end) break;
        std::this_thread::sleep_until(std::min(end, now + 5ms));
    } while (true);
}
