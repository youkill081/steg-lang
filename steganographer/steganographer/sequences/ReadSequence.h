//
// Created by Roumite on 18/02/2026.
//

#pragma once

#include <ByteBuffer.h>
#include "../Header.hpp"
#include "../SequenceManager.h"

class ReadSequenceError final : public std::runtime_error
{
public:
    using runtime_error::runtime_error;
};

class ReadSequence : public SequenceManager
{
private:
    uint32_t read_uint32();
    ByteBuffer read_byte_buffer(uint32_t number_of_byte);

    ByteBuffer _header_buffer;
    ByteBuffer _data_buffer;
    Header _header;

    void check_checksum();
public:
    ReadSequence(Image& image, uint32_t seed);

    ByteBuffer &get_data_buffer() { return _data_buffer; }
    ByteBuffer &get_header_buffer() { return _header_buffer; }
    Header &get_header() { return _header; }
};
