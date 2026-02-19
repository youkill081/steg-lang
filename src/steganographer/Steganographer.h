//
// Created by Roumite on 17/02/2026.
//

#pragma once

#include "ByteBuffer.h"
#include "constant.h"
#include "../images/Image.h"
#include "Random32.h"

class Steganographer
{
private:
    static Random32 random;

    static unsigned int generate_real_random();
    static unsigned int get_seed_size(const Image &image);

    static uint32_t zero_unused_seed_bits(uint32_t full_seed, uint32_t seed_size);
    static uint32_t complete_to_final_seed(const Image &image, uint32_t complete_seed);

    static void write_seed(Image &image, uint32_t seed, uint32_t seed_size);
    static uint32_t read_seed(Image &image, uint32_t seed_size);
public:
    static void encode(Image &image, const ByteBuffer &data, Data_type type);
    static void decode(Image &image);
};
