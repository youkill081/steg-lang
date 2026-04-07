//
// Created by Roumite on 23/02/2026.
//

#include "Utils.h"
#include "Runtime.h"

#include <algorithm>

#include "interpreter/exceptions.h"

std::string Utils::get_string_from_address(const uint32_t address) const
{
    std::string end;
    for (uint32_t i = address; const uint8_t curr = _runtime.memory.read_uint8(i); i++)
        end += curr;
    return end;
}

uint32_t Utils::load_image_and_map(const std::string& path)
{
    Image img = LoadImage(path.c_str());
    if (img.data == nullptr)
    {
        throw UtilsError("Can't load the image");
    }

    ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

    _last_image_width = img.width;
    _last_image_height = img.height;

    const uint32_t total_size = _last_image_width * _last_image_height * 4;
    const uint32_t address = _runtime.memory.allocate(total_size);
    auto &block = _runtime.memory.get_block(address);

    const uint8_t *pixels = static_cast<uint8_t *>(img.data);
    std::copy_n(pixels, total_size, block.data.begin());

    return address;
}
