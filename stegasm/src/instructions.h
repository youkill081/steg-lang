//
// Created by Roumite on 20/02/2026.
//

#pragma once

#include <array>
#include <cstdint>
#include <string_view>

class Runtime;

enum RegNames
{
    R0 = 0b000,
    R1 = 0b001,
    R2 = 0b010,
    R3 = 0b011,
    R4 = 0b100,
    R5 = 0b101,
    R6 = 0b110,
    R7 = 0b111
};

constexpr uint16_t number_of_registries = 8;

enum RegCount
{
    NO_REG = 0,
    ONE_REG = 1,
    TWO_REG = 2,
    THREE_REG = 3,
    FOUR_REG = 4,
    FIVE_REG = 5,
    SIX_REG = 6
};

enum DataCount
{
    NO_DATA = 0,
    ONE_DATA = 1,
    TWO_DATA = 2
};

struct InstructionView
{
    uint64_t raw_data;

    // opcode
    [[nodiscard]] uint8_t opcode() const { return static_cast<uint8_t>((raw_data >> 56) & 0xFF); }

    // Registries
    [[nodiscard]] RegNames r1() const { return static_cast<RegNames>((raw_data >> 53) & 0x7); }
    [[nodiscard]] RegNames r2() const { return static_cast<RegNames>((raw_data >> 48) & 0x7); }
    [[nodiscard]] RegNames r3() const { return static_cast<RegNames>((raw_data >> 29) & 0x7); }
    [[nodiscard]] RegNames r4() const { return static_cast<RegNames>((raw_data >> 26) & 0x7); }
    [[nodiscard]] RegNames r5() const { return static_cast<RegNames>((raw_data >> 23) & 0x7); }
    [[nodiscard]] RegNames r6() const { return static_cast<RegNames>((raw_data >> 20) & 0x7); }
    [[nodiscard]] RegNames r7() const { return static_cast<RegNames>((raw_data >> 17) & 0x7); }

    // Datas
    [[nodiscard]] bool is_d1_addr() const { return (raw_data >> 52) & 0x1; }
    [[nodiscard]] bool is_d2_addr() const { return (raw_data >> 51) & 0x1; }
    [[nodiscard]] uint16_t get_d1(const Runtime&, bool force_as_address=false) const;
    [[nodiscard]] uint16_t get_d2(const Runtime&, bool force_as_address=false) const;

};

using InstructionFct = void(*)(Runtime &, InstructionView raw_data);

struct InstructionDesc
{
    const std::string_view name;
    uint8_t opcode;
    RegCount regCount;
    DataCount dataCount;
    InstructionFct fn;
};

void instr_LOADA(Runtime &runtime, InstructionView view);
void instr_LOADR(Runtime &runtime, InstructionView view);
void instr_STOREA(Runtime &runtime, InstructionView view);
void instr_STORER(Runtime &runtime, InstructionView view);
void instr_MOV(Runtime &runtime, InstructionView view);
void instr_ADD(Runtime &runtime, InstructionView view);
void instr_SUB(Runtime &runtime, InstructionView view);
void instr_JMP(Runtime &runtime, InstructionView view);
void instr_CMPR(Runtime &runtime, InstructionView view);
void instr_CMPA(Runtime &runtime, InstructionView view);
void instr_JE(Runtime &runtime, InstructionView view);
void instr_JNE(Runtime &runtime, InstructionView view);
void instr_JA(Runtime &runtime, InstructionView view);
void instr_JB(Runtime &runtime, InstructionView view);
void instr_DISPLAY_N(Runtime &runtime, InstructionView view);
void instr_DISPLAY_AN(Runtime &runtime, InstructionView view);
void instr_DISPLAY_C(Runtime &runtime, InstructionView view);
void instr_DISPLAY_AC(Runtime &runtime, InstructionView view);
void instr_DISPLAY_B(Runtime &runtime, InstructionView view);
void instr_DISPLAY_AB(Runtime &runtime, InstructionView view);
void instr_HALT(Runtime &runtime, InstructionView view);
void instr_ALOCA(Runtime &runtime, InstructionView view);
void instr_ALOCR(Runtime &runtime, InstructionView view);
void instr_FREE(Runtime &runtime, InstructionView view);
void instr_DEBUG_R(Runtime &runtime, InstructionView view);
void instr_DEBUG_M(Runtime &runtime, InstructionView view);
void instr_ADDA(Runtime &runtime, InstructionView view);
void instr_CALL(Runtime &runtime, InstructionView view);
void instr_RET(Runtime &runtime, InstructionView view);
void instr_WINDOW_CREATE(Runtime &runtime, InstructionView view);
void instr_WINDOW_CLOSE(Runtime &runtime, InstructionView view);
void instr_WINDOW_POOL(Runtime &runtime, InstructionView view);
void instr_WINDOW_SHOULD_CLOSE(Runtime &runtime, InstructionView view);
void instr_WINDOW_CLEAR(Runtime &runtime, InstructionView view);
void instr_WINDOW_PRESENT(Runtime &runtime, InstructionView view);
void instr_WINDOW_KEY_PRESSED(Runtime &runtime, InstructionView view);
void instr_WINDOW_KEY_DOWN(Runtime &runtime, InstructionView view);
void instr_WINDOW_SET_TARGET_FPS(Runtime &runtime, InstructionView view);

constexpr std::array instructionSet =
{
    InstructionDesc{ "LOADA", 0x1, ONE_REG, ONE_DATA, &instr_LOADA },
    InstructionDesc{ "LOADR", 0x2, TWO_REG, NO_DATA, &instr_LOADR },
    InstructionDesc{ "STOREA", 0x3, ONE_REG, ONE_DATA, &instr_STOREA },
    InstructionDesc{ "STORER", 0x4, TWO_REG, NO_DATA, &instr_STORER },
    InstructionDesc{ "MOV", 0x5, TWO_REG, NO_DATA, &instr_MOV },
    InstructionDesc{ "ADD", 0x6, TWO_REG, NO_DATA, &instr_ADD },
    InstructionDesc{ "SUB", 0x7, TWO_REG, NO_DATA, &instr_SUB },
    InstructionDesc{ "JMP", 0x8, NO_REG, ONE_DATA, &instr_JMP },
    InstructionDesc{ "CMPR", 0x9, TWO_REG, NO_DATA, &instr_CMPR },
    InstructionDesc{ "CMPA", 0xA, ONE_REG, ONE_DATA, &instr_CMPA },
    InstructionDesc{ "JE", 0xB, NO_REG, ONE_DATA, &instr_JE },
    InstructionDesc{ "JNE", 0xC, NO_REG, ONE_DATA, &instr_JNE },
    InstructionDesc{ "JA", 0xD, NO_REG, ONE_DATA, &instr_JA },
    InstructionDesc{ "JB", 0xE, NO_REG, ONE_DATA, &instr_JB },
    InstructionDesc{ "DISPLAY_N", 0xF, ONE_REG, NO_DATA, &instr_DISPLAY_N },
    InstructionDesc{ "DISPLAY_AN", 0x10, ONE_REG, NO_DATA, &instr_DISPLAY_AN },
    InstructionDesc{ "DISPLAY_C", 0x11, ONE_REG, NO_DATA, &instr_DISPLAY_C },
    InstructionDesc{ "DISPLAY_AC", 0x12, ONE_REG, NO_DATA, &instr_DISPLAY_AC },
    InstructionDesc{ "DISPLAY_B", 0x13, ONE_REG, NO_DATA, &instr_DISPLAY_B },
    InstructionDesc{ "DISPLAY_AB", 0x14, ONE_REG, NO_DATA, &instr_DISPLAY_AB },
    InstructionDesc{ "HALT", 0x15, NO_REG, NO_DATA, &instr_HALT },
    InstructionDesc{ "ALOCA", 0x16, ONE_REG, ONE_DATA, &instr_ALOCA },
    InstructionDesc{ "ALOCR", 0x17, TWO_REG, NO_DATA, &instr_ALOCR },
    InstructionDesc{ "FREE", 0x18, ONE_REG, NO_DATA, &instr_FREE },
    InstructionDesc{ "DEBUG_R", 0x19, NO_REG, NO_DATA, &instr_DEBUG_R },
    InstructionDesc{ "DEBUG_M", 0x1A, NO_REG, NO_DATA, &instr_DEBUG_M },
    InstructionDesc{ "ADDA", 0x1B, ONE_REG, ONE_DATA, &instr_ADDA },
    InstructionDesc{ "CALL", 0x1C, NO_REG, ONE_DATA, &instr_CALL },
    InstructionDesc{ "RET", 0x1D, NO_REG, NO_DATA, &instr_RET },
    InstructionDesc{ "WINDOW_CREATE", 0x1E, NO_REG, NO_DATA, &instr_WINDOW_CREATE },
    InstructionDesc{ "WINDOW_CLOSE", 0x1F, NO_REG, NO_DATA, &instr_WINDOW_CLOSE },
    InstructionDesc{ "WINDOW_POOL", 0x20, NO_REG, NO_DATA, &instr_WINDOW_POOL },
    InstructionDesc{ "WINDOW_SHOULD_CLOSE", 0x21, ONE_REG, NO_DATA, &instr_WINDOW_SHOULD_CLOSE },
    InstructionDesc{ "WINDOW_CLEAR", 0x22, THREE_REG, NO_DATA, &instr_WINDOW_CLEAR },
    InstructionDesc{ "WINDOW_PRESENT", 0x23, NO_REG, NO_DATA, &instr_WINDOW_PRESENT },
    InstructionDesc{ "WINDOW_KEY_PRESSED", 0x24, ONE_REG, ONE_DATA, &instr_WINDOW_KEY_PRESSED },
    InstructionDesc{ "WINDOW_KEY_DOWN", 0x25, ONE_REG, ONE_DATA, &instr_WINDOW_KEY_DOWN },
    InstructionDesc{ "WINDOW_SET_TARGET_FPS", 0x26, NO_REG, ONE_DATA, &instr_WINDOW_SET_TARGET_FPS },
};

#include "check_instructions.hpp"
#include "instruction_utils.hpp"
