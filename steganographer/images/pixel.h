//
// Created by Roumite on 17/02/2026.
//

#pragma once

#include <sstream>

#include "Random32.h"

enum pixel_usage {
    HIDE_SEED,
    HIDE_DATA,
};

struct pixel {
    uint8_t red, blue, green;

    uint8_t channel_read_order[3];
    uint8_t current_read_index = 0;
    pixel_usage usage = HIDE_DATA;

    void shuffle_read_order(Random32 &random)
    {
        channel_read_order[0] = 0;
        channel_read_order[1] = 1;
        channel_read_order[2] = 2;
        current_read_index = 0;

        for (int i = 2; i > 0; --i)
        {
            const uint32_t j = random.get_next_rand() % (i + 1);
            std::swap(channel_read_order[i], channel_read_order[j]);
        }
    }
};