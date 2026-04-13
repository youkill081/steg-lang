//
// Created by Roumite on 17/02/2026.
//

#include "Steganographer.h"

#include <random>

#include "Logger.h"
#include "sequences/ReadSequence.h"
#include "sequences/WriteSequence.h"

Random32 Steganographer::random{};

uint32_t Steganographer::generate_real_random()
{
    std::random_device rd;
    return rd();
}

uint32_t Steganographer::get_seed_size(const steganographer::Image& image)
{
    std::size_t n_pixels = image.get_number_of_pixels();

    return std::min(
        static_cast<uint32_t>(std::ceil(
            std::log2(static_cast<double>(n_pixels)))),
        32u
    );
}

uint32_t Steganographer::zero_unused_seed_bits(uint32_t full_seed, uint32_t seed_size)
{
    if (seed_size >= 32u) return full_seed;
    if (seed_size == 0u)  return 0u;

    const uint32_t mask = 0xFFFFFFFFu << (32u - seed_size);
    return full_seed & mask;
}

uint32_t Steganographer::complete_to_final_seed(const steganographer::Image &image, uint32_t complete_seed)
{
    random.reseed(complete_seed);
    uint32_t final_seed = 0;

    for (int i = 0; i < image.get_number_of_pixels(); ++i) { final_seed = random.get_next_rand(); }

    return final_seed;
}

void Steganographer::write_seed(steganographer::Image& image, uint32_t seed, uint32_t seed_size)
{
    uint32_t padding = 0;
    random.reseed(image.get_number_of_pixels());

    for (int i = 0; i < seed_size; ++i)
    {
        uint32_t n = random.get_next_rand();
        padding += n % image.get_number_of_pixels();

    write_seed_start:

        pixel& pix = image.get_pixel(padding % image.get_number_of_pixels());

        if (pix.usage != HIDE_DATA)
        {
            padding += 1;
            goto write_seed_start;
        }

        pix.red = static_cast<unsigned char>((pix.red & 0xFEu) | ((seed >> (31 - i)) & 0x1u));
        pix.usage = HIDE_SEED;
    }
}

uint32_t Steganographer::read_seed(steganographer::Image& image, uint32_t seed_size)
{
    uint32_t padding = 0;
    uint32_t seed = 0;

    random.reseed(image.get_number_of_pixels());

    for (int i = 0; i < static_cast<int>(seed_size); ++i)
    {
        uint32_t n = random.get_next_rand();
        padding += n % image.get_number_of_pixels();

    read_seed_start:

        pixel& pix = image.get_pixel(padding % image.get_number_of_pixels());

        if (pix.usage != HIDE_DATA)
        {
            padding += 1;
            goto read_seed_start;
        }

        const uint32_t bit = static_cast<uint32_t>(pix.red) & 0x1u;
        seed |= (bit << (31 - i));

        pix.usage = HIDE_SEED;
    }

    return seed;
}

void Steganographer::encode(steganographer::Image& image, const ByteBuffer& data, DataType type)
{
    uint32_t seed = generate_real_random();
    uint32_t seed_size = get_seed_size(image);
    seed = zero_unused_seed_bits(seed, seed_size); // set seed_size last bit to zero
    write_seed(image, seed, seed_size);

    uint32_t real_seed = complete_to_final_seed(image, seed);
    Logger::log("Write with obfuscated seed : " + std::to_string(seed), "Steganographer");

    WriteSequence write_sequencer{image, real_seed};
    write_sequencer.write_sequence(data, type);
}

Steganographer::DecodeResult Steganographer::decode(steganographer::Image& image)
{
    uint32_t seed_size = get_seed_size(image);
    uint32_t seed = read_seed(image, seed_size);
    uint32_t real_seed = complete_to_final_seed(image, seed);

    Logger::log("Decode with obfuscated seed " + std::to_string(seed), "Steganographer");

    ReadSequence read_sequencer{image, real_seed};
    return Steganographer::DecodeResult{
        .header = read_sequencer.get_header(),
        .data = read_sequencer.get_data_buffer()
    };

}
