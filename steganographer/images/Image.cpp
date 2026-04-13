//
// Created by Roumite on 17/02/2026.
//

#include "Image.h"

#include <stdexcept>
#include <vector>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_STATIC
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <filesystem>

#include "stb_image_write.h"
#include "Logger.h"

using namespace steganographer;

Image::Image(const std::string& path)
{
    int channels;
    unsigned char* data = stbi_load(path.c_str(), &_width, &_height, &channels, 3);
    if (!data) {
        throw std::runtime_error("Can't load image at path : " + path);
    }

    pixels = std::make_unique<::Pixels>();
    pixels->reserve(_width * _height);

    for (int y = 0; y < _height; ++y) {
        for (int x = 0; x < _width; ++x) {
            unsigned char* p = data + (y * _width + x) * 3;
            pixel px{};

            px.red = p[0];
            px.green = p[1];
            px.blue = p[2];
            pixels->push_back(px);
        }
    }

    stbi_image_free(data);
}

void Image::save_png(const std::string& output_path) const
{
    if (!pixels) {
        throw std::runtime_error("Image::save_png: pixels is null");
    }
    if (_width <= 0 || _height <= 0) {
        throw std::runtime_error("Image::save_png: invalid image dimensions");
    }
    if (pixels->size() != static_cast<std::size_t>(_width) * static_cast<std::size_t>(_height)) {
        throw std::runtime_error("Image::save_png: pixels size doesn't match width*height");
    }

    std::string absolute_path;
    try {
        absolute_path = std::filesystem::absolute(output_path).string();
    } catch (...) {
        absolute_path = output_path;
    }

    Logger::log("Saving image to " + absolute_path, "Image");

    std::vector<unsigned char> data;
    data.resize(static_cast<std::size_t>(_width) * static_cast<std::size_t>(_height) * 3u);

    for (int y = 0; y < _height; ++y) {
        for (int x = 0; x < _width; ++x) {
            const std::size_t idx_pix = static_cast<std::size_t>(y) * static_cast<std::size_t>(_width) + static_cast<std::size_t>(x);
            const std::size_t idx = idx_pix * 3u;

            const pixel& px = pixels->at(idx_pix);
            data[idx + 0] = px.red;
            data[idx + 1] = px.green;
            data[idx + 2] = px.blue;
        }
    }

    const int stride_in_bytes = _width * 3;
    const int ok = stbi_write_png(output_path.c_str(), _width, _height, 3, data.data(), stride_in_bytes);
    if (ok == 0) {
        throw std::runtime_error("Image::save_png: failed to write png: " + output_path);
    }
}

pixel &Image::get_pixel(uint32_t number) const
{
    return pixels->at(number%pixels->size());
}

Pixels &Image::get_pixels() const
{
    return *pixels;
}
