//
// Created by Roumite on 18/02/2026.
//

#pragma once

#include "../ByteBuffer.h"
#include "../SequenceManager.h"

class ReadSequence : public SequenceManager
{
private:
    uint32_t read_uint32();
    ByteBuffer read_byte_buffer(uint32_t number_of_byte);

    void check_checksum();
public:
    ReadSequence(Image& image, uint32_t seed);
};
