#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <span>
#include <vector>

class AudioBackend
{
public:
    using Bytes = std::vector<uint8_t>;
    using Input = std::span<const uint8_t>;

    struct Format {
        uint32_t rate, channels, bits;
        [[nodiscard]] bool valid() const;
        [[nodiscard]] uint32_t stride() const { return channels * (bits / 8); }
    };

    struct Pcm {
        Format format{};
        Bytes data;
        [[nodiscard]] uint32_t frames() const { return format.stride() ? data.size() / format.stride() : 0; }
    };

    struct Settings {
        float gain, pan, pitch;
    };

    enum Status : uint32_t { Ok, InvalidHandle, InvalidValue, Unavailable, Unsupported };

    AudioBackend();
    AudioBackend(AudioBackend&&) noexcept;
    ~AudioBackend();
    uint32_t init();
    void close();
    void update();
    void wait(uint32_t milliseconds);
    void master(float gain);
    uint32_t load(Input bytes, uint32_t codec, bool streaming);
    uint32_t source_pcm(Input bytes, Format format);
    bool source_info(uint32_t source, std::array<uint32_t, 5>& info);
    void free_source(uint32_t source);
    uint32_t play(uint32_t source, Settings settings, bool loop, bool auto_release);
    uint32_t set(uint32_t voice, Settings settings);
    uint32_t control(uint32_t voice, uint32_t command);
    uint32_t seek(uint32_t voice, uint32_t frame);
    bool voice_info(uint32_t voice, std::array<uint32_t, 3>& info);
    uint32_t create_stream(Format format, uint32_t capacity, uint32_t block);
    uint32_t write_stream(uint32_t stream, Input bytes, uint32_t& consumed_bytes);
    bool stream_info(uint32_t stream, std::array<uint32_t, 3>& info);
    uint32_t play_stream(uint32_t stream);
    uint32_t control_stream(uint32_t stream, uint32_t command);

    static Pcm decode(Input bytes, uint32_t codec);
    static Pcm convert(Input bytes, Format from, Format to);
    static Bytes encode_wav(Input bytes, Format format);

private:
    struct State;
    std::unique_ptr<State> _state;
    uint32_t _next = 1;
};
