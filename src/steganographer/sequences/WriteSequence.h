//
// Created by Roumite on 18/02/2026.
//

#pragma once

#include "../SequenceManager.h"
#include "../header.hpp"

#include <vector>

class ByteBuffer;

class WriteSequence : public SequenceManager
{
private:
    void write_uint32(uint32_t value);
    void write_uint8(uint8_t value);
    void write_byte_buffer(const ByteBuffer& buffer);
public:
    WriteSequence(Image& image, uint32_t seed)
        : SequenceManager(image, seed)
    {
    }

    void write_sequence(const ByteBuffer& data, Data_type type);
};

