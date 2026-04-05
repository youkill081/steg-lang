//
// Created by Roumite on 23/02/2026.
//

#include "Utils.h"

#include "Runtime.h"

std::string Utils::get_string_from_address(const uint32_t address) const
{
    std::string end;
    for (uint32_t i = address; const uint8_t curr = _runtime.memory.read_uint8(i); i++)
        end += curr;
    return end;
}
