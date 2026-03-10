//
// Created by Roumite on 20/02/2026.
//

#pragma once

#include <unordered_set>
constexpr bool is_lower(char c) {
    return c >= 'a' && c <= 'z';
}

constexpr bool instruction_name_contains_lower()
{
    for (const auto &instruction : instructionSet)
    {
        // Name Check
        for (const auto &c : instruction.name)
            if (is_lower(c))
                return false;
    }
    return true;
}

constexpr bool instruction_name_unique()
{
    for (size_t i = 0; i < instructionSet.size(); ++i)
        for (size_t j = i + 1; j < instructionSet.size(); ++j)
            if (instructionSet[i].name == instructionSet[j].name)
                return false;
    return true;
}

constexpr bool opcode_unique()
{
    for (size_t i = 0; i < instructionSet.size(); ++i)
        for (size_t j = i + 1; j < instructionSet.size(); ++j)
            if (instructionSet[i].opcode == instructionSet[j].opcode)
                return false;
    return true;
}

static_assert(instruction_name_contains_lower(), "InstructionSet contains lowercase letters!");
static_assert(instruction_name_unique(), "InstructionSet names must be unique !");
static_assert(opcode_unique(), "InstructionSet opcode must be unique !");