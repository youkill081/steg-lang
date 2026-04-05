//
// Created by Roumite on 05/03/2026.
//

#pragma once

#include <array>
#include <cstdint>
#include <stdexcept>

constexpr uint32_t STACK_SIZE = 8192;

class Stack {
private:
    std::array<uint32_t, STACK_SIZE> _data = {};
    uint32_t _sp = 0;
public:
    inline void push(uint32_t value) {
        if (_sp >= STACK_SIZE) {
            throw std::runtime_error("Stack overflow");
        }
        _data[_sp++] = value;
    }

    inline uint32_t pop() {
        if (_sp == 0) {
            throw std::runtime_error("Stack underflow");
        }
        return _data[--_sp];
    }
};