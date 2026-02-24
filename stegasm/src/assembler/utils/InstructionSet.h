//
// Created by Roumite on 23/02/2026.
//

#pragma once

#include <array>

#include "instructions.h"

namespace assembler
{
    using UsedRegistries = std::array<RegNames, 8>;

    struct DataValueParsingResult
    {
        bool is_address;
        uint16_t value;
    };

    using DataValues = std::array<DataValueParsingResult, 2>;

    struct Instruction
    {
        const InstructionDesc &desc;
        UsedRegistries registries;
        DataValues datas;
    };

    class InstructionSet : public std::vector<Instruction>
    {
    public:
        void display() const;
    };
}
