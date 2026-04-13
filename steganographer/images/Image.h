//
// Created by Roumite on 17/02/2026.
//

#pragma once

#include <string>
#include <memory>

#include "Pixels.h"

namespace steganographer
{
    class Image
    {
    private:
        int _width{}, _height{};
    public:
        std::unique_ptr<Pixels> pixels;
        explicit Image(const std::string &path);
        void save_png(const std::string &output_path) const;

        [[nodiscard]] int get_width() const {return _width;};
        [[nodiscard]] int get_height() const {return _height;};

        [[nodiscard]] pixel &get_pixel(uint32_t number) const;
        [[nodiscard]] Pixels &get_pixels() const;

        [[nodiscard]] std::size_t get_number_of_pixels() const {return pixels->size();};
    };
}
