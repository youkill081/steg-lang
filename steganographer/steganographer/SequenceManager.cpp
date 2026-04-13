//
// Created by Roumite on 18/02/2026.
//

#include "SequenceManager.h"

#include "../images/Image.h"
#include "Logger.h"

pixel &SequenceManager::get_next_pixel()
{
    while (true)
    {
        if (_current_index >= _index_list.size())
        {
            shuffle_index_list();
            _current_index = 0;
        }

        pixel &pixel = _image.get_pixel(_index_list[_current_index++]);
        if (pixel.usage == HIDE_DATA)
            return pixel;
    }
}

uint8_t& SequenceManager::get_pixel_data_channel(pixel& pixel)
{
    if (pixel.current_read_index >= 3)
        throw std::runtime_error(
            "SequenceManager::get_pixel_data_uint32: try to get data on full pixel!"
        );

    switch (pixel.channel_read_order[pixel.current_read_index++])
    {
    case 0: return pixel.red;
    case 1: return pixel.green;
    case 2: return pixel.blue;
    default: throw std::runtime_error("invalid channel");
    }

}

void SequenceManager::write_bit_in_pixel(pixel& pixel, bool bit) const
{
    uint8_t &data = get_pixel_data_channel(pixel);

    const uint8_t mask = static_cast<uint8_t>(1u << (_nbr_of_shuffle - 1));
    const auto b = static_cast<uint8_t>(bit ? 1u : 0u);

    data = static_cast<uint8_t>((data & ~mask) | b);
}

bool SequenceManager::read_bit_in_pixel(pixel& pixel) const
{
    const uint8_t &data = get_pixel_data_channel(pixel);
    return (data & (1u << (_nbr_of_shuffle - 1))) != 0u;
}

void SequenceManager::shuffle_index_list()
{
    std::shuffle(_index_list.begin(), _index_list.end(), random.get_engine());
    _image.pixels->shuffle_pixels_channels(random);
    _nbr_of_shuffle++;

    if (_nbr_of_shuffle > 8)
        throw std::runtime_error("SequenceManager::shuffle_index_list: no more colors channels free, try with bigger image !");
}
