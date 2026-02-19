//
// Created by Roumite on 18/02/2026.
//

#include "ReadSequence.h"

#include <iostream>

#include "../constant.h"
#include "../header.hpp"
#include "../../utils/Logger.h"

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
        throw std::runtime_error("ReadSequence::check_checksum: invalid checksum (not an encoded image)");
    Logger::log("ReadSequence::check_checksum: checksum OK !", "ReadSequence");
}

ReadSequence::ReadSequence(Image& image, uint32_t seed) : SequenceManager(image, seed)
{
    this->check_checksum();

    uint32_t header_crc32 = read_uint32();
    ByteBuffer header_buffer = read_byte_buffer(HEADER_SIZE_BYTE);

    if (header_buffer.get_crc32() != header_crc32)
        throw std::runtime_error("ReadSequence::ReadSequence: header CRC32 missmatch !");
    Logger::log("ReadSequence::ReadSequence: header read successfully !", "ReadSequence");

    struct header header = convert_byte_buffer_to_header(header_buffer);
    if (header.version != PROTOCOL_VERSION)
        throw std::runtime_error("ReadSequence::ReadSequence: unsupported protocol version !");

    ByteBuffer data_buffer = read_byte_buffer(header.data_size_byte);
    if (data_buffer.get_crc32() != header.data_crc32)
        throw std::runtime_error("ReadSequence::ReadSequence: data CRC32 missmatch !");

    Logger::log("ReadSequence::ReadSequence: data read successfully !", "ReadSequence");
    std::cout << data_buffer << std::endl;
}
