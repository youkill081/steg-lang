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
    THREE_REG = 3
};

enum DataCount
{
    NO_DATA = 0,
    ONE_DATA = 1
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

void instr_EOF(Runtime &runtime, InstructionView view);
void instr_LOADA(Runtime &runtime, InstructionView view);
void instr_LOADR(Runtime &runtime, InstructionView view);
void instr_STOREA(Runtime &runtime, InstructionView view);
void instr_STORER(Runtime &runtime, InstructionView view);
void instr_MOV(Runtime &runtime, InstructionView view);
void instr_ADD(Runtime &runtime, InstructionView view);
void instr_ADDA(Runtime &runtime, InstructionView view);
void instr_SUB(Runtime &runtime, InstructionView view);
void instr_SUBA(Runtime &runtime, InstructionView view);
void instr_MUL(Runtime &runtime, InstructionView view);
void instr_MULA(Runtime &runtime, InstructionView view);
void instr_DIV(Runtime &runtime, InstructionView view);
void instr_DIVA(Runtime &runtime, InstructionView view);
void instr_MIN(Runtime &runtime, InstructionView view);
void instr_MINA(Runtime &runtime, InstructionView view);
void instr_MAX(Runtime &runtime, InstructionView view);
void instr_MAXA(Runtime &runtime, InstructionView view);
void instr_MOD(Runtime &runtime, InstructionView view);
void instr_MODA(Runtime &runtime, InstructionView view);
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
void instr_CALL(Runtime &runtime, InstructionView view);
void instr_RET(Runtime &runtime, InstructionView view);
void instr_PUSH(Runtime &runtime, InstructionView view);
void instr_POP(Runtime &runtime, InstructionView view);
void instr_RAND(Runtime &runtime, InstructionView view);
void instr_WINDOW_CREATE(Runtime &runtime, InstructionView view);
void instr_WINDOW_CLOSE(Runtime &runtime, InstructionView view);
void instr_WINDOW_SET_VIEWPORT_SIZE(Runtime &runtime, InstructionView view);
void instr_WINDOW_DISABLE_VIEWPORT(Runtime &runtime, InstructionView view);
void instr_WINDOW_POOL(Runtime &runtime, InstructionView view);
void instr_WINDOW_SHOULD_CLOSE(Runtime &runtime, InstructionView view);
void instr_WINDOW_CLEAR(Runtime &runtime, InstructionView view);
void instr_WINDOW_PRESENT(Runtime &runtime, InstructionView view);
void instr_WINDOW_KEY_PRESSED(Runtime &runtime, InstructionView view);
void instr_WINDOW_KEY_DOWN(Runtime &runtime, InstructionView view);
void instr_WINDOW_SET_TARGET_FPS(Runtime &runtime, InstructionView view);
void instr_WINDOW_SET_TEXT_SIZE(Runtime &runtime, InstructionView view);
void instr_WINDOW_SET_TEXT_COLOR(Runtime &runtime, InstructionView view);
void instr_WINDOW_SET_FONT(Runtime &runtime, InstructionView view);
void instr_WINDOW_DRAW_TEXT(Runtime &runtime, InstructionView view);
void instr_WINDOW_DRAW_TEXTURE(Runtime &runtime, InstructionView view);
void instr_WINDOW_SET_TEXTURE_COLOR_MASK(Runtime &runtime, InstructionView view);
void instr_WINDOW_RESET_TEXTURE_COLOR_MASK(Runtime &runtime, InstructionView view);
void instr_WINDOW_SET_ICON(Runtime &runtime, InstructionView view);
void instr_FILE_OPEN(Runtime &runtime, InstructionView view);
void instr_FILE_CREATE(Runtime &runtime, InstructionView view);
void instr_FILE_SAVE(Runtime &runtime, InstructionView view);
void instr_FILE_DELETE(Runtime &runtime, InstructionView view);
void instr_FILE_CLOSE(Runtime &runtime, InstructionView view);
void instr_FILE_RESET_CURSOR(Runtime &runtime, InstructionView view);
void instr_FILE_CLEAR_DATA(Runtime &runtime, InstructionView view);
void instr_FILE_READ_BYTE(Runtime &runtime, InstructionView view);
void instr_FILE_READ_WORD(Runtime &runtime, InstructionView view);
void instr_FILE_APPEND_BYTE(Runtime &runtime, InstructionView view);
void instr_FILE_APPEND_WORD(Runtime &runtime, InstructionView view);
void instr_FILE_IS_BYTE_REMAINING(Runtime &runtime, InstructionView view);
void instr_FILE_IS_WORD_REMAINING(Runtime &runtime, InstructionView view);
void instr_CLOCK_CREATE(Runtime &runtime, InstructionView view);
void instr_CLOCK_DELETE(Runtime &runtime, InstructionView view);
void instr_CLOCK_GET_ELAPSED_MS(Runtime &runtime, InstructionView view);
void instr_CLOCK_GET_ELAPSED_S(Runtime &runtime, InstructionView view);
void instr_CLOCK_RESET(Runtime &runtime, InstructionView view);

struct RawInstruction
{
    const std::string_view name;
    RegCount regCount;
    DataCount dataCount;
    InstructionFct fn;
    bool user_usable = true;
};

struct InstructionDesc
{
    std::string_view name;
    uint8_t opcode;
    RegCount regCount;
    DataCount dataCount;
    InstructionFct fn;
    bool user_usable;
};

constexpr std::array rawInstructionSet =
{
    RawInstruction{"EOF", NO_REG, NO_DATA, &instr_EOF, false}, // Added by Assembler when reach and of .text sections
    RawInstruction{"LOADA", ONE_REG, ONE_DATA, &instr_LOADA},
    RawInstruction{"LOADR", TWO_REG, NO_DATA, &instr_LOADR},
    RawInstruction{"STOREA", ONE_REG, ONE_DATA, &instr_STOREA},
    RawInstruction{"STORER", TWO_REG, NO_DATA, &instr_STORER},
    RawInstruction{"MOV", TWO_REG, NO_DATA, &instr_MOV},
    RawInstruction{"ADD", TWO_REG, NO_DATA, &instr_ADD},
    RawInstruction{"ADDA", ONE_REG, ONE_DATA, &instr_ADDA},
    RawInstruction{"SUB", TWO_REG, NO_DATA, &instr_SUB},
    RawInstruction{"SUBA", ONE_REG, ONE_DATA, &instr_SUBA},
    RawInstruction{"MUL", TWO_REG, NO_DATA, &instr_MUL},
    RawInstruction{"MULA", ONE_REG, ONE_DATA, &instr_MULA},
    RawInstruction{"DIV", TWO_REG, NO_DATA, &instr_DIV},
    RawInstruction{"DIVA", ONE_REG, ONE_DATA, &instr_DIVA},
    RawInstruction{"MIN", TWO_REG, NO_DATA, &instr_MIN},
    RawInstruction{"MINA", ONE_REG, ONE_DATA, &instr_MINA},
    RawInstruction{"MAX", TWO_REG, NO_DATA, &instr_MAX},
    RawInstruction{"MAXA", ONE_REG, ONE_DATA, &instr_MAXA},
    RawInstruction{"MOD", TWO_REG, NO_DATA, &instr_MOD},
    RawInstruction{"MODA", ONE_REG, ONE_DATA, &instr_MODA},
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
    RawInstruction{"CALL", NO_REG, ONE_DATA, &instr_CALL},
    RawInstruction{"RET", NO_REG, NO_DATA, &instr_RET},
    RawInstruction{"PUSH", ONE_REG, NO_DATA, &instr_PUSH},
    RawInstruction{"POP", ONE_REG, NO_DATA, &instr_POP},
    RawInstruction{"RAND", ONE_REG, NO_DATA, &instr_RAND},
    RawInstruction{"WINDOW_CREATE", TWO_REG, ONE_DATA, &instr_WINDOW_CREATE},
    RawInstruction{"WINDOW_CLOSE", NO_REG, NO_DATA, &instr_WINDOW_CLOSE},
    RawInstruction{"WINDOW_SET_VIEWPORT_SIZE", TWO_REG, NO_DATA, &instr_WINDOW_SET_VIEWPORT_SIZE},
    RawInstruction{"WINDOW_DISABLE_VIEWPORT", TWO_REG, NO_DATA, &instr_WINDOW_DISABLE_VIEWPORT},
    RawInstruction{"WINDOW_POOL", NO_REG, NO_DATA, &instr_WINDOW_POOL},
    RawInstruction{"WINDOW_SHOULD_CLOSE", ONE_REG, NO_DATA, &instr_WINDOW_SHOULD_CLOSE},
    RawInstruction{"WINDOW_CLEAR", THREE_REG, NO_DATA, &instr_WINDOW_CLEAR},
    RawInstruction{"WINDOW_PRESENT", NO_REG, NO_DATA, &instr_WINDOW_PRESENT},
    RawInstruction{"WINDOW_KEY_PRESSED", ONE_REG, ONE_DATA, &instr_WINDOW_KEY_PRESSED},
    RawInstruction{"WINDOW_KEY_DOWN", ONE_REG, ONE_DATA, &instr_WINDOW_KEY_DOWN},
    RawInstruction{"WINDOW_SET_TARGET_FPS", NO_REG, ONE_DATA, &instr_WINDOW_SET_TARGET_FPS},
    RawInstruction{"WINDOW_SET_TEXT_SIZE", NO_REG, ONE_DATA, &instr_WINDOW_SET_TEXT_SIZE},
    RawInstruction{"WINDOW_SET_TEXT_COLOR", THREE_REG, NO_DATA, &instr_WINDOW_SET_TEXT_COLOR},
    RawInstruction{"WINDOW_SET_FONT", NO_REG, ONE_DATA, &instr_WINDOW_SET_FONT},
    RawInstruction{"WINDOW_DRAW_TEXT", TWO_REG, ONE_DATA, &instr_WINDOW_DRAW_TEXT},
    RawInstruction{"WINDOW_DRAW_TEXTURE", TWO_REG, ONE_DATA, &instr_WINDOW_DRAW_TEXTURE},
    RawInstruction{"WINDOW_SET_TEXTURE_COLOR_MASK", THREE_REG, NO_DATA, &instr_WINDOW_SET_TEXTURE_COLOR_MASK},
    RawInstruction{"WINDOW_RESET_TEXTURE_COLOR_MASK", NO_REG, NO_DATA, &instr_WINDOW_RESET_TEXTURE_COLOR_MASK},
    RawInstruction{"WINDOW_SET_ICON", NO_REG, ONE_DATA, &instr_WINDOW_SET_ICON},
    RawInstruction{"FILE_OPEN", ONE_REG, ONE_DATA, &instr_FILE_OPEN},
    RawInstruction{"FILE_CREATE", ONE_REG, ONE_DATA, &instr_FILE_CREATE},
    RawInstruction{"FILE_SAVE", ONE_REG, NO_DATA, &instr_FILE_SAVE},
    RawInstruction{"FILE_DELETE", ONE_REG, NO_DATA, &instr_FILE_DELETE},
    RawInstruction{"FILE_CLOSE", ONE_REG, NO_DATA, &instr_FILE_CLOSE},
    RawInstruction{"FILE_RESET_CURSOR", ONE_REG, NO_DATA, &instr_FILE_RESET_CURSOR},
    RawInstruction{"FILE_CLEAR_DATA", ONE_REG, NO_DATA, &instr_FILE_CLEAR_DATA},
    RawInstruction{"FILE_READ_BYTE", TWO_REG, NO_DATA, &instr_FILE_READ_BYTE},
    RawInstruction{"FILE_READ_WORD", TWO_REG, NO_DATA, &instr_FILE_READ_WORD},
    RawInstruction{"FILE_APPEND_BYTE", TWO_REG, NO_DATA, &instr_FILE_APPEND_BYTE},
    RawInstruction{"FILE_APPEND_WORD", TWO_REG, NO_DATA, &instr_FILE_APPEND_WORD},
    RawInstruction{"FILE_IS_BYTE_REMAINING", TWO_REG, NO_DATA, &instr_FILE_IS_BYTE_REMAINING},
    RawInstruction{"FILE_IS_WORD_REMAINING", TWO_REG, NO_DATA, &instr_FILE_IS_WORD_REMAINING},
    RawInstruction{"CLOCK_CREATE", ONE_REG, NO_DATA, &instr_CLOCK_CREATE},
    RawInstruction{"CLOCK_DELETE", ONE_REG, NO_DATA, &instr_CLOCK_DELETE},
    RawInstruction{"CLOCK_GET_ELAPSED_MS", TWO_REG, NO_DATA, &instr_CLOCK_GET_ELAPSED_MS},
    RawInstruction{"CLOCK_GET_ELAPSED_S", TWO_REG, NO_DATA, &instr_CLOCK_GET_ELAPSED_S},
    RawInstruction{"CLOCK_RESET", ONE_REG, NO_DATA, &instr_CLOCK_RESET},
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
            rawInstructionSet[i].fn,
            rawInstructionSet[i].user_usable
        };
    }
    return result;
}

constexpr std::array instructionSet = compute_instructions();

#include "check_instructions.hpp"
#include "instruction_utils.hpp"
