//
// Created by Roumite on 18/02/2026.
//

#include "ReadSequence.h"

#include <iostream>

#include "../constant.h"
#include "Logger.h"

uint32_t ReadSequence::read_uint32()
{
    uint32_t result = 0u;
    for (uint32_t i = 0; i < 32u; ++i)
    {
        pixel& curr = get_next_pixel();
        result = (result << 1u) | read_bit_in_pixel(curr);
    }
    return result;
}

ByteBuffer ReadSequence::read_byte_buffer(uint32_t number_of_byte)
{
    ByteBuffer buffer;

    for (int i = 0; i < number_of_byte * 8; i++)
    {
        buffer.push_bit(
            read_bit_in_pixel(
                get_next_pixel()
            )
        );
    }
    return buffer;
}

void ReadSequence::check_checksum()
{
    uint32_t checksum = read_uint32();

    if (checksum != CHECKSUM)
        throw ReadSequenceError("ReadSequence::check_checksum: invalid checksum (not an encoded image)");
    Logger::log("ReadSequence::check_checksum: checksum OK !", "ReadSequence");
}

ReadSequence::ReadSequence(Image& image, uint32_t seed) : SequenceManager(image, seed)
{
    this->check_checksum();

    uint32_t header_crc32 = read_uint32();
    _header_buffer = read_byte_buffer(HEADER_SIZE_BYTE);

    if (_header_buffer.get_crc32() != header_crc32)
        throw ReadSequenceError("ReadSequence::ReadSequence: header CRC32 missmatch !");
    Logger::log("ReadSequence::ReadSequence: header read successfully !", "ReadSequence");

    _header = convert_byte_buffer_to_header(_header_buffer);
    if (_header.version != PROTOCOL_VERSION)
        throw ReadSequenceError("ReadSequence::ReadSequence: unsupported protocol version !");

    _data_buffer = read_byte_buffer(_header.data_size_byte);
    if (_data_buffer.get_crc32() != _header.data_crc32)
        throw ReadSequenceError("ReadSequence::ReadSequence: data CRC32 missmatch !");

    Logger::log("ReadSequence::ReadSequence: data read successfully !", "ReadSequence");
}
