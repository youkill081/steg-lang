//
// Created by Roumite on 05/03/2026.
//

#pragma once

#include "Runtime.h"
#include "InstructionView.h"

inline uint32_t InstructionView::get_r1(const Runtime &rt, uint8_t size) const
{
    const uint16_t reg_idx = r1();
    const uint32_t value = rt.registries.read(reg_idx);

    if (__builtin_expect(is_r1_addr(), 0)) {
        return read_mem_sized(rt, value, size);
    }
    return value;
}

inline uint32_t InstructionView::get_r2(const Runtime &rt, uint8_t size) const
{
    const uint16_t reg_idx = r2();
    const uint32_t value = rt.registries.read(reg_idx);

    if (__builtin_expect(is_r2_addr(), 0)) {
        return read_mem_sized(rt, value, size);
    }
    return value;
}

inline uint32_t InstructionView::get_r3(const Runtime &rt, uint8_t size) const
{
    const uint32_t h = header();
    const uint16_t reg_idx = r3();
    uint32_t value = rt.registries.read(reg_idx);

    if (__builtin_expect(is_r3_addr(), 0)) {
        return read_mem_sized(rt, value, size);
    }
    return value;
}

inline uint32_t InstructionView::get_data(const Runtime &rt, uint8_t size) const
{
    uint32_t val = get_raw_data();
    if (data_type() == 0b10) {
        switch (size) {
            case 1: return rt.memory.read_uint8(val);
            case 2: return rt.memory.read_uint16(val);
            default: return rt.memory.read_uint32(val);
        }
    }
    return val;
}


inline uint32_t InstructionView::read_mem_sized(const Runtime &rt, uint32_t addr, uint8_t size)
{
    switch (size) {
        case 1: return rt.memory.read_uint8(addr);
        case 2: return rt.memory.read_uint16(addr);
        default: return rt.memory.read_uint32(addr);
    }
}
