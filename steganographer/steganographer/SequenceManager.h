//
// Created by Roumite on 18/02/2026.
//

#pragma once

#include <cstdint>
#include <numeric>
#include <vector>

#include "Random32.h"
#include "../images/pixel.h"
#include "../images/Image.h"


class Image;

class SequenceManager
{
protected:
    Random32 random{};
    uint32_t padding = 0;

    steganographer::Image &_image;
    std::vector<uint64_t> _index_list{};
    uint32_t _seed;

    uint64_t _current_index = 0;
    uint8_t _nbr_of_shuffle = 0;

    pixel &get_next_pixel();
    static uint8_t &get_pixel_data_channel(pixel &pixel);

    void write_bit_in_pixel(pixel &pixel, bool bit) const;
    bool read_bit_in_pixel(pixel &pixel) const;

    void shuffle_index_list();
public:
    SequenceManager(steganographer::Image &image, uint32_t seed) : _image(image), _seed(seed)
    {
        random.reseed(_seed);

        _index_list.resize(_image.get_number_of_pixels());
        std::iota(_index_list.begin(), _index_list.end(), 0);
        this->shuffle_index_list();
    }
};
