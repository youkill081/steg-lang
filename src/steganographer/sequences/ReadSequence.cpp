//
// Created by Roumite on 18/02/2026.
//

#include "ReadSequence.h"

#include "../constant.h"
#include "../../utils/Logger.h"

void ReadSequence::check_checksum()
{
    uint32_t read = 0u;

    for (uint32_t i = 0; i < 32u; ++i)
    {
        pixel& curr = get_next_pixel();

        const bool bit = read_bit_in_pixel(curr);
        read = (read << 1u) | (bit ? 1u : 0u);
    }

    Logger::log("Read checksum : " + std::to_string(read), "ReadSequence");
    Logger::log_uint32_as_bit(read);
    if (read != CHECKSUM)
        throw std::runtime_error("ReadSequence::check_checksum: invalid checksum (not an encoded image)");
}

ReadSequence::ReadSequence(Image& image, uint32_t seed) : SequenceManager(image, seed)
{
    this->check_checksum();
}
