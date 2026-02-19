//
// Created by Roumite on 18/02/2026.
//

#pragma once

#include <ostream>
#include <vector>

class ByteBuffer
{
private:
    std::vector<uint8_t> buffer;
    size_t cursor = 0;

    uint8_t write_bit_buffer = 0;
    uint8_t write_bit_count = 0;
public:
    ByteBuffer() = default;
    explicit ByteBuffer(std::initializer_list<uint8_t> init);

    friend std::ostream& operator<<(std::ostream& os, const ByteBuffer& obj)
    {
        for (uint8_t byte : obj.buffer)
        {
            for (int i = 7; i >= 0; --i)
            {
                os << ((byte >> i) & 1);
            }
            os << '\n';
        }

        return os;
    }

    void push_bit(bool bit); // Used to fill buffer bit by bit
    uint8_t get_crc32() const; // calculate crc32 of the bytebuffer

    void write_uint8(uint8_t value);
    void write_uint32(uint32_t value);

    uint8_t read_uint8();
    uint32_t read_uint32();

    void reset_cursor();
    [[nodiscard]] size_t size() const; // Return number of byte in  the buffer
    [[nodiscard]] size_t remaining_uint8() const;
    [[nodiscard]] size_t remaining_uint32() const;

    [[nodiscard]] const std::vector<uint8_t>& data() const;
};

