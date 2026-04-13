//
// Created by Roumite on 17/02/2026.
//

#pragma once

#include <iostream>
#include <vector>

#include "pixel.h"

class Pixels : public std::vector<pixel>
{
public:
    void shuffle_pixels_channels(Random32 &random)
    {
        for (auto &pix: *this)
        {
            pix.shuffle_read_order(random);
        }
    }
};
