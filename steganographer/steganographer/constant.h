//
// Created by Roumite on 18/02/2026.
//

#pragma once

#include <cstdint>
#include <string>

constexpr uint32_t CHECKSUM = 0b01101101011010010111010001100101;
constexpr uint8_t PROTOCOL_VERSION = 1;

enum DataType
{
    STRING,
    FILE_T,
    CODE
};

inline std::string data_type_to_string(DataType type)
{
    switch (type)
    {
        case STRING: return "STRING";
        case FILE_T: return "FILE";
        case CODE: return "CODE";
        default: return "UNKNOWN";
    }
}