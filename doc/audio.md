# Audio

Steg uses raylib for playback. Audio can run independently of a window. Encoded audio and PCM travel through ordinary `file` values, including files embedded in the program and vectors created with `file_create("")`.

The opaque Steg types are `audio_source`, `audio_voice` and `audio_stream`. A source can be played by multiple independent voices. A stream accepts PCM blocks from the program and has one voice at a time. Call `audio_update()` regularly while playing music or feeding a PCM stream.

## Instructions

The Steg built-in names are the lowercase forms of the mnemonics below. `dst` is the destination register in StegASM. In Steg, the same value is the function result. `job` and `out` are `uint32 @` pointers to descriptors in VM memory. The tables give descriptor words in order, starting at index zero. Write float fields through a `float @` view of the same block.

Status values are 0 for success, 1 for an invalid audio handle, 2 for an invalid value, 3 for an unavailable device and 4 for an unsupported operation. Creation functions return an invalid handle on failure (0 in StegASM). In Steg, check a handle using the matching `audio_*_info` function. An invalid VM address or file descriptor raises the usual VM error.

| StegASM | Behavior |
| --- | --- |
| `AUDIO_INIT dst` | Opens playback and returns a status. Repeated calls are safe. |
| `AUDIO_CLOSE` | Releases the device and all audio handles. |
| `AUDIO_UPDATE` | Refills encoded music and PCM device buffers. |
| `AUDIO_WAIT milliseconds` | Waits while continuing to refill audio. Use it in a CLI loop to avoid busy-waiting. |
| `AUDIO_MASTER_SET gain` | Sets the master gain as a float from 0 to 1. |
| `AUDIO_SOURCE_LOAD dst, file, job` | Creates a source. Job words are codec and mode (0 decoded, 1 streamed). |
| `AUDIO_SOURCE_PCM dst, file, job` | Creates a source from raw PCM. Job words are rate, channels and bits. |
| `AUDIO_SOURCE_INFO dst, source, out` | Returns a status. Output words are rate, channels, bits, duration in frames and streaming flag. For encoded streaming sources, the format is unknown and the duration is 4294967295 until a voice opens a decoder. |
| `AUDIO_SOURCE_FREE source` | Releases the source handle. Existing voices retain the data they need. |
| `AUDIO_PLAY dst, source, job` | Starts an independent voice. Job words are gain, pan, pitch and flags. Flag 1 loops an encoded streaming source. Flag 2 releases the voice on an audio update once playback has ended. |
| `AUDIO_VOICE_SET dst, voice, job` | Returns a status. Job words are gain, pan and pitch as floats. Gain is 0 to 1, pan is −1 (left) to 1 (right), and pitch is 0.125 to 8, with 1 at the original speed. |
| `AUDIO_VOICE_CONTROL dst, voice, command` | Returns a status. Commands are 0 pause, 1 resume, 2 stop and 3 release. Release a voice when finished with its handle. |
| `AUDIO_VOICE_SEEK dst, voice, frame` | Returns a status. Seeks an encoded streaming voice to a frame where its decoder supports seeking. Position accuracy depends on the codec. |
| `AUDIO_VOICE_INFO dst, voice, out` | Returns a status. Output words are state (0 stopped, 1 playing, 2 paused, 3 buffering), current frame and duration in frames. An unknown frame or duration is 4294967295. |
| `AUDIO_STREAM_CREATE dst, job` | Creates a PCM queue. Job words are rate, channels, bits, queue capacity in frames and refill block size in frames. |
| `AUDIO_STREAM_WRITE dst, stream, file` | Queues complete PCM frames from the file cursor. Returns the number accepted and advances the cursor by the accepted bytes. A full queue returns zero. |
| `AUDIO_STREAM_INFO dst, stream, out` | Returns a status. Output words are free frames, queued frames and short refills since creation or reset. The queue counters exclude blocks handed to the device. |
| `AUDIO_STREAM_PLAY dst, stream` | Creates a voice that starts when the first block is ready. |
| `AUDIO_STREAM_CONTROL dst, stream, command` | Returns a status. Commands are 0 finish after queued audio, 1 clear and reset, and 2 release. Reset and release invalidate the associated voice. |

Codec numbers are 1 WAV, 2 Ogg Vorbis, 3 MP3, 4 QOA, 5 FLAC, 6 XM and 7 MOD. Availability depends on the codecs built into raylib and the mode used. Streaming keeps an owned copy of the encoded file and decodes small blocks. Short sources share one native sample buffer between simultaneous voices.

PCM formats use unsigned 8-bit, little-endian signed 16-bit or little-endian IEEE float32 samples. Use mono or interleaved stereo at 1000–384000 Hz. One frame contains one sample per channel. The PCM queue accepts capacities up to 1048576 frames and refill blocks from 256 to 32768 frames, with capacity at least as large as the block. Pick a refill size suitable for the update frequency and device. A short refill during playback pads with silence.

## Offline PCM

These instructions work without `audio_init()`. Decode, convert and encode replace the caller-provided destination file on success and reset its cursor. Use a file created with a path if you intend to call `file_save`. Mixing works in place and preserves the output cursor. The decoder reports its actual output format. In the bundled raylib, WAV decoding yields signed 16-bit PCM, including for float WAV input.

| StegASM | Descriptor and result |
| --- | --- |
| `AUDIO_DECODE input, output, job` | Codec (0), output status (1), decoded rate, channels, bits and frame count (2–5). |
| `AUDIO_PCM_CONVERT input, output, job` | Input rate, channels, bits (0–2), output rate, channels, bits (3–5), output status and frame count (6–7). |
| `AUDIO_PCM_MIX output, input, job` | Rate, channels and bits (0–2), input start frame (3), output start frame (4), frame count (5), float gain (6), output status (7). Both buffers use float32 PCM. The destination range must already exist. Sums are clipped to −1 through 1. |
| `AUDIO_PCM_MEASURE dst, input, job` | Rate, channels and bits (0–2), start and frame count (3–4), float peak and RMS (5–6). Returns a status. |
| `AUDIO_PCM_ENCODE input, output, job` | Rate, channels and bits (0–2), codec (3), output status (4). WAV is the available output format. |

## Play a WAV from the command line

```steg
export fn main() -> void {
    if (audio_init() != 0) { exit(1); }
    void @memory = aloc(20);
    uint32 @job = memory;
    float @params = memory;

    file input = file_open("sound.wav");
    job[0] = 1; job[1] = 0;
    audio_source source = audio_source_load(input, job);
    if (audio_source_info(source, job) != 0) { exit(2); }
    file_close(input);

    params[0] = 1.0; params[1] = 0.0; params[2] = 1.0; job[3] = 0;
    audio_voice voice = audio_play(source, job);
    if (audio_voice_info(voice, job) != 0) { exit(3); }
    while (job[0] == 1) {
        audio_wait(10);
        audio_voice_info(voice, job);
    }
    audio_close();
    free(memory);
    exit(0);
}
```
