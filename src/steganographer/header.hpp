//
// Created by Roumite on 18/02/2026.
//

#pragma once

#include <cstdint>

#include <ByteBuffer.h>

#include "constant.h"

struct header {
    uint8_t version; // current version of protocole (current 0b1)
    uint32_t data_size_byte; // contains number of bits of data stored in image
    uint8_t data_type;  // contains type of data stored in image
    uint32_t data_crc32; // contains CRC32 of data stored in image
};

constexpr size_t HEADER_SIZE_BYTE = (8 + 32 + 8 + 32) / 8;

inline ByteBuffer gen_header(const ByteBuffer &data, DataType data_type)
{
    ByteBuffer header_buffer;

    header_buffer.write_uint8(PROTOCOL_VERSION);
    header_buffer.write_uint32(data.size());
    header_buffer.write_uint8(data_type);
    header_buffer.write_uint32(data.get_crc32());
    return header_buffer;
}

inline header convert_byte_buffer_to_header(ByteBuffer &buffer)
{
    return {
        .version = buffer.read_uint8(),
        .data_size_byte = buffer.read_uint32(),
        .data_type = buffer.read_uint8(),
        .data_crc32 = buffer.read_uint32()
    };
}