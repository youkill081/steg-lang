//
// Created by Roumite on 18/02/2026.
//

#include "WriteSequence.h"
#include "../constant.h"
#include "../../utils/Logger.h"

void WriteSequence::write_checksum()
{
    Logger::log("Writing Checksum", "WriteSequence");

    for (uint32_t i = 0; i < 32u; ++i)
    {
        pixel &curr = get_next_pixel();

        const bool bit = ((CHECKSUM >> (31u - i)) & 0x1u) != 0u;
        write_bit_in_pixel(curr, bit);
    }
}

void WriteSequence::write_sequence(const ByteBuffer& data)
{
    this->write_checksum();
}
