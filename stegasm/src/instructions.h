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
void instr_WINDOW_SET_TEXT_SIZE(Runtime &runtime, InstructionView view);
void instr_WINDOW_SET_TEXT_COLOR(Runtime &runtime, InstructionView view);
void instr_WINDOW_DRAW_TEXT(Runtime &runtime, InstructionView view);

struct RawInstruction
{
    const std::string_view name;
    RegCount regCount;
    DataCount dataCount;
    InstructionFct fn;
};

struct InstructionDesc
{
    std::string_view name;
    uint8_t opcode;
    RegCount regCount;
    DataCount dataCount;
    InstructionFct fn;
};

constexpr std::array rawInstructionSet =
{
    RawInstruction{"LOADA", ONE_REG, ONE_DATA, &instr_LOADA},
    RawInstruction{"LOADR", TWO_REG, NO_DATA, &instr_LOADR},
    RawInstruction{"STOREA", ONE_REG, ONE_DATA, &instr_STOREA},
    RawInstruction{"STORER", TWO_REG, NO_DATA, &instr_STORER},
    RawInstruction{"MOV", TWO_REG, NO_DATA, &instr_MOV},
    RawInstruction{"ADD", TWO_REG, NO_DATA, &instr_ADD},
    RawInstruction{"SUB", TWO_REG, NO_DATA, &instr_SUB},
    RawInstruction{"JMP", NO_REG, ONE_DATA, &instr_JMP},
    RawInstruction{"CMPR", TWO_REG, NO_DATA, &instr_CMPR},
    RawInstruction{"CMPA", ONE_REG, ONE_DATA, &instr_CMPA},
    RawInstruction{"JE", NO_REG, ONE_DATA, &instr_JE},
    RawInstruction{"JNE", NO_REG, ONE_DATA, &instr_JNE},
    RawInstruction{"JA", NO_REG, ONE_DATA, &instr_JA},
    RawInstruction{"JB", NO_REG, ONE_DATA, &instr_JB},
    RawInstruction{"DISPLAY_N", ONE_REG, NO_DATA, &instr_DISPLAY_N},
    RawInstruction{"DISPLAY_AN", ONE_REG, NO_DATA, &instr_DISPLAY_AN},
    RawInstruction{"DISPLAY_C", ONE_REG, NO_DATA, &instr_DISPLAY_C},
    RawInstruction{"DISPLAY_AC", ONE_REG, NO_DATA, &instr_DISPLAY_AC},
    RawInstruction{"DISPLAY_B", ONE_REG, NO_DATA, &instr_DISPLAY_B},
    RawInstruction{"DISPLAY_AB", ONE_REG, NO_DATA, &instr_DISPLAY_AB},
    RawInstruction{"HALT", NO_REG, NO_DATA, &instr_HALT},
    RawInstruction{"ALOCA", ONE_REG, ONE_DATA, &instr_ALOCA},
    RawInstruction{"ALOCR", TWO_REG, NO_DATA, &instr_ALOCR},
    RawInstruction{"FREE", ONE_REG, NO_DATA, &instr_FREE},
    RawInstruction{"DEBUG_R", NO_REG, NO_DATA, &instr_DEBUG_R},
    RawInstruction{"DEBUG_M", NO_REG, NO_DATA, &instr_DEBUG_M},
    RawInstruction{"ADDA", ONE_REG, ONE_DATA, &instr_ADDA},
    RawInstruction{"CALL", NO_REG, ONE_DATA, &instr_CALL},
    RawInstruction{"RET", NO_REG, NO_DATA, &instr_RET},
    RawInstruction{"WINDOW_CREATE", TWO_REG, ONE_DATA, &instr_WINDOW_CREATE},
    RawInstruction{"WINDOW_CLOSE", NO_REG, NO_DATA, &instr_WINDOW_CLOSE},
    RawInstruction{"WINDOW_POOL", NO_REG, NO_DATA, &instr_WINDOW_POOL},
    RawInstruction{"WINDOW_SHOULD_CLOSE", ONE_REG, NO_DATA, &instr_WINDOW_SHOULD_CLOSE},
    RawInstruction{"WINDOW_CLEAR", THREE_REG, NO_DATA, &instr_WINDOW_CLEAR},
    RawInstruction{"WINDOW_PRESENT", NO_REG, NO_DATA, &instr_WINDOW_PRESENT},
    RawInstruction{"WINDOW_KEY_PRESSED", ONE_REG, ONE_DATA, &instr_WINDOW_KEY_PRESSED},
    RawInstruction{"WINDOW_KEY_DOWN", ONE_REG, ONE_DATA, &instr_WINDOW_KEY_DOWN},
    RawInstruction{"WINDOW_SET_TARGET_FPS", NO_REG, ONE_DATA, &instr_WINDOW_SET_TARGET_FPS},
    RawInstruction{"WINDOW_SET_TEXT_SIZE", NO_REG, ONE_DATA, &instr_WINDOW_SET_TEXT_SIZE},
    RawInstruction{"WINDOW_SET_TEXT_COLOR", THREE_REG, NO_DATA, &instr_WINDOW_SET_TEXT_COLOR},
    RawInstruction{"WINDOW_DRAW_TEXT", TWO_REG, ONE_DATA, &instr_WINDOW_DRAW_TEXT},
};

constexpr std::array<InstructionDesc, rawInstructionSet.size()> compute_instructions()
{
    std::array<InstructionDesc, rawInstructionSet.size()> result{};
    uint8_t current_op_code = 0;
    for (size_t i = 0; i < rawInstructionSet.size(); ++i)
    {
        result.data()[i] = InstructionDesc{
            rawInstructionSet[i].name,
            current_op_code++,
            rawInstructionSet[i].regCount,
            rawInstructionSet[i].dataCount,
            rawInstructionSet[i].fn
        };
    }
    return result;
}

constexpr std::array instructionSet = compute_instructions();

#include "check_instructions.hpp"
#include "instruction_utils.hpp"
