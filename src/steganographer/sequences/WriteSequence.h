//
// Created by Roumite on 18/02/2026.
//

#pragma once

#include "../SequenceManager.h"

#include <vector>

class ByteBuffer;

class WriteSequence : public SequenceManager
{
private:
    void write_checksum();
public:
    WriteSequence(Image& image, uint32_t seed)
        : SequenceManager(image, seed)
    {
    }

    void write_sequence(const ByteBuffer& data);
};

