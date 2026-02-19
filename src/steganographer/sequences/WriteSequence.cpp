//
// Created by Roumite on 18/02/2026.
//

#include "WriteSequence.h"

#include "../constant.h"
#include "../../utils/Logger.h"

void WriteSequence::write_uint32(uint32_t value)
{
    for (uint32_t i = 0; i < 32u; ++i)
    {
        pixel &curr = get_next_pixel();
        write_bit_in_pixel(curr, ((value >> (31u - i)) & 0x1u) != 0u);
    }
}

void WriteSequence::write_uint8(uint8_t value)
{
    for (uint32_t i = 0; i < 8u; ++i)
    {
        pixel &curr = get_next_pixel();
        write_bit_in_pixel(curr, ((value >> (7u - i)) & 0x1u) != 0u);
    }
}

void WriteSequence::write_byte_buffer(const ByteBuffer& buffer)
{
    for (const auto &byte : buffer.data())
    {
        write_uint8(byte);
    }

}

void WriteSequence::write_sequence(const ByteBuffer& data, data_type type)
{
    ByteBuffer header = gen_header(data, type);

    write_uint32(CHECKSUM);
    write_uint32(header.get_crc32());
    write_byte_buffer(header);
    write_byte_buffer(data);
}
