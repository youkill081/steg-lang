//
// Created by Roumite on 18/02/2026.
//

#include "ByteBuffer.h"

ByteBuffer::ByteBuffer(std::initializer_list<uint8_t> data)
{
    for (auto &byte : data)
        this->write_uint8(byte);
}

void ByteBuffer::push_bit(bool bit)
{
    write_bit_buffer <<= 1; // shift all bit on left

    if(bit)
        write_bit_buffer |= 1; // put LSB to 1
    write_bit_count++;

    if (write_bit_count == 8){
        this->write_uint8(write_bit_buffer);
        write_bit_buffer = 0;
        write_bit_count = 0;
    }
}

/*
 * AI implémentation of crc32 algorith
 */
uint32_t ByteBuffer::get_crc32() const
{
    static uint32_t table[256];
    static bool table_computed = false;

    if(!table_computed)
    {
        for(uint32_t i = 0; i < 256; i++)
        {
            uint32_t c = i;
            for(size_t j = 0; j < 8; j++)
            {
                if(c & 1)
                    c = 0xEDB88320 ^ (c >> 1);
                else
                    c >>= 1;
            }
            table[i] = c;
        }
        table_computed = true;
    }

    uint32_t crc = 0xFFFFFFFF;

    for(uint8_t byte : buffer)
    {
        crc = table[(crc ^ byte) & 0xFF] ^ (crc >> 8);
    }

    return crc ^ 0xFFFFFFFF;
}

void ByteBuffer::write_uint8(uint8_t value)
{
    this->buffer.push_back(value);
}

void ByteBuffer::write_uint16(uint16_t value)
{
    buffer.push_back((value >> 8) & 0xFF);
    buffer.push_back(value & 0xFF);
}

void ByteBuffer::write_uint32(uint32_t value)
{
    buffer.push_back((value >> 24) & 0xFF);
    buffer.push_back((value >> 16) & 0xFF);
    buffer.push_back((value >> 8) & 0xFF);
    buffer.push_back(value & 0xFF);
}

uint8_t ByteBuffer::read_uint8()
{
    return buffer[cursor++];
}

uint16_t ByteBuffer::read_uint16()
{
    uint16_t v = (buffer[cursor] << 8) | buffer[cursor + 1];
    cursor += 2;
    return v;
}

uint32_t ByteBuffer::read_uint32()
{
    uint32_t v =
        (buffer[cursor] << 24) |
        (buffer[cursor + 1] << 16) |
        (buffer[cursor + 2] << 8) |
        buffer[cursor + 3];

    cursor += 4;
    return v;
}

void ByteBuffer::reset_cursor()
{
    cursor = 0;
}

size_t ByteBuffer::size() const
{
    return buffer.size();
}

size_t ByteBuffer::remaining_uint8() const
{
    return buffer.size() - cursor;
}

size_t ByteBuffer::remaining_uint16() const
{
    return (buffer.size() - cursor) / 2;
}

size_t ByteBuffer::remaining_uint32() const
{
    return (buffer.size() - cursor) / 4;
}

const std::vector<uint8_t>& ByteBuffer::data() const
{
    return buffer;
}
