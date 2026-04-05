//
// Created by Roumite on 05/03/2026.
//

#pragma once

class Runtime;

enum RegNames
{
    R0 = 0b00000,
    R1 = 0b00001,
    R2 = 0b00010,
    R3 = 0b00011,
    R4 = 0b00100,
    R5 = 0b00101,
    R6 = 0b00110,
    R7 = 0b00111,
    R8 = 0b01000,
    R9 = 0b01001,
    R10 = 0b01010,
    R11 = 0b01011,
    R12 = 0b01100,
    R13 = 0b01101,
    R14 = 0b01110,
    R15 = 0b01111,
    R16 = 0b10000,
    R17 = 0b10001,
    R18 = 0b10010,
    R19 = 0b10011,
    R20 = 0b10100,
    R21 = 0b10101,
    R22 = 0b10110,
    R23 = 0b10111,
    R24 = 0b11000,
    R25 = 0b11001,
    R26 = 0b11010,
    R27 = 0b11011,
    R28 = 0b11100,
    R29 = 0b11101,
    R30 = 0b11110,
    R31 = 0b11111
};

constexpr uint16_t number_of_registries = 32;

enum RegCount
{
    NO_REG = 0,
    ONE_REG = 1,
    TWO_REG = 2,
    THREE_REG = 3
};

enum RegTypes
{
    REG_NO,
    REG,
    REG_ADDRESS,
    REG_BOTH
};

enum DataCount
{
    NO_DATA = 0,
    ONE_DATA = 1
};

struct InstructionView
{
    uint64_t raw_data;

    [[nodiscard]] uint8_t opcode() const { return static_cast<uint8_t>((raw_data >> 56) & 0xFF); }

    [[nodiscard]] uint32_t header() const { return static_cast<uint32_t>((raw_data >> 32) & 0xFFFFFFFF); }
    [[nodiscard]] uint8_t handler_number() const { return static_cast<uint8_t>((header() >> 22) & 0x03); }

    [[nodiscard]] bool is_r1_addr() const { return (header() >> 21) & 0x1; }
    [[nodiscard]] RegNames r1() const { return static_cast<RegNames>((header() >> 16) & 0x1F); }
    [[nodiscard]] uint32_t get_r1(const Runtime& rt, uint8_t size = 4) const;

    [[nodiscard]] bool is_r2_addr() const { return (header() >> 15) & 0x1; }
    [[nodiscard]] RegNames r2() const { return static_cast<RegNames>((header() >> 10) & 0x1F); }
    [[nodiscard]] uint32_t get_r2(const Runtime& rt, uint8_t size = 4) const;

    [[nodiscard]] bool is_r3_addr() const { return (header() >> 9) & 0x1; }
    [[nodiscard]] RegNames r3() const { return static_cast<RegNames>((header() >> 4) & 0x1F); }
    [[nodiscard]] inline uint32_t get_r3(const Runtime& rt, uint8_t size = 4) const;

    [[nodiscard]] uint8_t data_type() const { return static_cast<uint8_t>(header() & 0x3); }
    [[nodiscard]] uint32_t get_raw_data() const { return static_cast<uint32_t>(raw_data & 0xFFFFFFFF); }
    [[nodiscard]] uint32_t get_data(const Runtime& rt, uint8_t size = 4) const;

private:
    static uint32_t read_mem_sized(const Runtime& rt, uint32_t addr, uint8_t size);
};