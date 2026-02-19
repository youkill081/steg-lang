//
// Created by Roumite on 18/02/2026.
//

#pragma once

#include <cstdint>
#include <string>

constexpr uint32_t CHECKSUM = 0b01101101011010010111010001100101;
constexpr uint8_t PROTOCOL_VERSION = 1;

enum Data_type
{
    RAW,
    STRING
};

inline std::string data_type_to_string(Data_type type)
{
    switch (type)
    {
        case RAW: return "RAW";
        case STRING: return "STRING";
        default: return "UNKNOWN";
    }
}