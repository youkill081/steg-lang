//
// Created by Roumite on 20/02/2026.
//

#pragma once

#include <array>
#include <vector>
#include <cstdint>
#include <string_view>

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
    R7 = 0b00111
};

constexpr uint16_t number_of_registries = 8;

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
    REG_ADDRESS
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
    [[nodiscard]] uint32_t get_r1(const Runtime& rt) const;

    [[nodiscard]] bool is_r2_addr() const { return (header() >> 15) & 0x1; }
    [[nodiscard]] RegNames r2() const { return static_cast<RegNames>((header() >> 10) & 0x1F); }
    [[nodiscard]] uint32_t get_r2(const Runtime& rt) const;

    [[nodiscard]] bool is_r3_addr() const { return (header() >> 9) & 0x1; }
    [[nodiscard]] RegNames r3() const { return static_cast<RegNames>((header() >> 4) & 0x1F); }
    [[nodiscard]] uint32_t get_r3(const Runtime& rt) const;

    [[nodiscard]] uint8_t data_type() const { return static_cast<uint8_t>(header() & 0x3); }
    [[nodiscard]] uint32_t get_raw_data() const { return static_cast<uint32_t>(raw_data & 0xFFFFFFFF); }
    [[nodiscard]] uint32_t get_data(const Runtime& rt) const;
};

using InstructionFct = void(*)(Runtime &, InstructionView raw_data);

void instr_EOF(Runtime &runtime, InstructionView view);
void instr_LOADD(Runtime &runtime, InstructionView view);
void instr_LOADR(Runtime &runtime, InstructionView view);
void instr_STORED(Runtime &runtime, InstructionView view);
void instr_STORER(Runtime &runtime, InstructionView view);
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

struct InstructionHandler
{
    constexpr InstructionHandler() : fn(nullptr) {}

    constexpr explicit InstructionHandler(const InstructionFct &fct, DataCount data = NO_DATA)
        : fn(fct), data_type(data) {}
    constexpr InstructionHandler(const InstructionFct &fct, RegTypes first, DataCount data = NO_DATA)
        : fn(fct), first_reg_type(first), data_type(data) {}
    constexpr InstructionHandler(const InstructionFct &fct, RegTypes first, RegTypes second, DataCount data = NO_DATA)
        : fn(fct), first_reg_type(first), second_reg_type(second), data_type(data) {}
    constexpr InstructionHandler(const InstructionFct &fct, RegTypes first, RegTypes second, RegTypes third, DataCount data = NO_DATA)
        : fn(fct), first_reg_type(first), second_reg_type(second), third_reg_type(third), data_type(data) {}

    InstructionFct fn;
    RegTypes first_reg_type = REG_NO;
    RegTypes second_reg_type = REG_NO;
    RegTypes third_reg_type = REG_NO;
    DataCount data_type = NO_DATA;
};

enum HandlerNumber
{
    HANDLER_0 = 0b00,
    HANDLER_1 = 0b01,
    HANDLER_2 = 0b10,
    HANDLER_3 = 0b11,
};

constexpr std::size_t max_instruction_handlers = 4; // This value can be moved if needed

struct RawInstruction
{
    std::string_view name;
    bool user_usable;
    std::array<InstructionHandler, max_instruction_handlers> handlers;
    size_t handler_count;

    template<typename... Args>
    constexpr RawInstruction(std::string_view n, bool u, Args... h)
            : name(n), user_usable(u),
              handlers{{h...}},
              handler_count(sizeof...(Args)) {}
};

struct InstructionDesc
{
    std::string_view name;
    uint8_t opcode;
    bool user_usable;
    size_t handler_count;
    std::array<InstructionHandler, max_instruction_handlers> handlers;
};

constexpr std::array rawInstructionSet =
{
    RawInstruction{"EOF", false, InstructionHandler(&instr_EOF)},
    RawInstruction{"LOAD", true,
        InstructionHandler(&instr_LOADD, REG, ONE_DATA),
        InstructionHandler(&instr_LOADR, REG, REG_ADDRESS),
        InstructionHandler(&instr_LOADR, REG, REG)},
    RawInstruction{"STOREA", true, InstructionHandler(&instr_STORED, REG, ONE_DATA)},
    RawInstruction{"STORER", true, InstructionHandler(&instr_STORER, REG, REG)},
    RawInstruction{"ADD", true, InstructionHandler(&instr_ADD, REG, REG)},
    RawInstruction{"ADDA", true, InstructionHandler(&instr_ADDA, REG, ONE_DATA)},
    RawInstruction{"SUB", true, InstructionHandler(&instr_SUB, REG, REG)},
    RawInstruction{"SUBA", true, InstructionHandler(&instr_SUBA, REG, ONE_DATA)},
    RawInstruction{"MUL", true, InstructionHandler(&instr_MUL, REG, REG)},
    RawInstruction{"MULA", true, InstructionHandler(&instr_MULA, REG, ONE_DATA)},
    RawInstruction{"DIV", true, InstructionHandler(&instr_DIV, REG, REG)},
    RawInstruction{"DIVA", true, InstructionHandler(&instr_DIVA, REG, ONE_DATA)},
    RawInstruction{"MIN", true, InstructionHandler(&instr_MIN, REG, REG)},
    RawInstruction{"MINA", true, InstructionHandler(&instr_MINA, REG, ONE_DATA)},
    RawInstruction{"MAX", true, InstructionHandler(&instr_MAX, REG, REG)},
    RawInstruction{"MAXA", true, InstructionHandler(&instr_MAXA, REG, ONE_DATA)},
    RawInstruction{"MOD", true, InstructionHandler(&instr_MOD, REG, REG)},
    RawInstruction{"MODA", true, InstructionHandler(&instr_MODA, REG, ONE_DATA)},
    RawInstruction{"JMP", true, InstructionHandler(&instr_JMP, ONE_DATA)},
    RawInstruction{"CMPR", true, InstructionHandler(&instr_CMPR, REG, REG)},
    RawInstruction{"CMPA", true, InstructionHandler(&instr_CMPA, REG, ONE_DATA)},
    RawInstruction{"JE", true, InstructionHandler(&instr_JE, ONE_DATA)},
    RawInstruction{"JNE", true, InstructionHandler(&instr_JNE, ONE_DATA)},
    RawInstruction{"JA", true, InstructionHandler(&instr_JA, ONE_DATA)},
    RawInstruction{"JB", true, InstructionHandler(&instr_JB, ONE_DATA)},
    RawInstruction{"DISPLAY_N", true, InstructionHandler(&instr_DISPLAY_N, REG)},
    RawInstruction{"DISPLAY_AN", true, InstructionHandler(&instr_DISPLAY_AN, REG)},
    RawInstruction{"DISPLAY_C", true, InstructionHandler(&instr_DISPLAY_C, REG)},
    RawInstruction{"DISPLAY_AC", true, InstructionHandler(&instr_DISPLAY_AC, REG)},
    RawInstruction{"DISPLAY_B", true, InstructionHandler(&instr_DISPLAY_B, REG)},
    RawInstruction{"DISPLAY_AB", true, InstructionHandler(&instr_DISPLAY_AB, REG)},
    RawInstruction{"HALT", true, InstructionHandler(&instr_HALT)},
    RawInstruction{"ALOCA", true, InstructionHandler(&instr_ALOCA, REG, ONE_DATA)},
    RawInstruction{"ALOCR", true, InstructionHandler(&instr_ALOCR, REG, REG)},
    RawInstruction{"FREE", true, InstructionHandler(&instr_FREE, REG)},
    RawInstruction{"DEBUG_R", true, InstructionHandler(&instr_DEBUG_R)},
    RawInstruction{"DEBUG_M", true, InstructionHandler(&instr_DEBUG_M)},
    RawInstruction{"CALL", true, InstructionHandler(&instr_CALL, ONE_DATA)},
    RawInstruction{"RET", true, InstructionHandler(&instr_RET)},
    RawInstruction{"PUSH", true, InstructionHandler(&instr_PUSH, REG)},
    RawInstruction{"POP", true, InstructionHandler(&instr_POP, REG)},
    RawInstruction{"RAND", true, InstructionHandler(&instr_RAND, REG)},
    RawInstruction{"WINDOW_CREATE", true, InstructionHandler(&instr_WINDOW_CREATE, REG, REG, ONE_DATA)},
    RawInstruction{"WINDOW_CLOSE", true, InstructionHandler(&instr_WINDOW_CLOSE)},
    RawInstruction{"WINDOW_SET_VIEWPORT_SIZE", true, InstructionHandler(&instr_WINDOW_SET_VIEWPORT_SIZE, REG, REG)},
    RawInstruction{"WINDOW_DISABLE_VIEWPORT", true, InstructionHandler(&instr_WINDOW_DISABLE_VIEWPORT, REG, REG)},
    RawInstruction{"WINDOW_POOL", true, InstructionHandler(&instr_WINDOW_POOL)},
    RawInstruction{"WINDOW_SHOULD_CLOSE", true, InstructionHandler(&instr_WINDOW_SHOULD_CLOSE, REG)},
    RawInstruction{"WINDOW_CLEAR", true, InstructionHandler(&instr_WINDOW_CLEAR, REG, REG, REG)},
    RawInstruction{"WINDOW_PRESENT", true, InstructionHandler(&instr_WINDOW_PRESENT)},
    RawInstruction{"WINDOW_KEY_PRESSED", true, InstructionHandler(&instr_WINDOW_KEY_PRESSED, REG, ONE_DATA)},
    RawInstruction{"WINDOW_KEY_DOWN", true, InstructionHandler(&instr_WINDOW_KEY_DOWN, REG, ONE_DATA)},
    RawInstruction{"WINDOW_SET_TARGET_FPS", true, InstructionHandler(&instr_WINDOW_SET_TARGET_FPS, ONE_DATA)},
    RawInstruction{"WINDOW_SET_TEXT_SIZE", true, InstructionHandler(&instr_WINDOW_SET_TEXT_SIZE, ONE_DATA)},
    RawInstruction{"WINDOW_SET_TEXT_COLOR", true, InstructionHandler(&instr_WINDOW_SET_TEXT_COLOR, REG, REG, REG)},
    RawInstruction{"WINDOW_SET_FONT", true, InstructionHandler(&instr_WINDOW_SET_FONT, ONE_DATA)},
    RawInstruction{"WINDOW_DRAW_TEXT", true, InstructionHandler(&instr_WINDOW_DRAW_TEXT, REG, REG, ONE_DATA)},
    RawInstruction{"WINDOW_DRAW_TEXTURE", true, InstructionHandler(&instr_WINDOW_DRAW_TEXTURE, REG, REG, ONE_DATA)},
    RawInstruction{"WINDOW_SET_TEXTURE_COLOR_MASK", true, InstructionHandler(&instr_WINDOW_SET_TEXTURE_COLOR_MASK, REG, REG, REG)},
    RawInstruction{"WINDOW_RESET_TEXTURE_COLOR_MASK", true, InstructionHandler(&instr_WINDOW_RESET_TEXTURE_COLOR_MASK)},
    RawInstruction{"WINDOW_SET_ICON", true, InstructionHandler(&instr_WINDOW_SET_ICON, ONE_DATA)},
    RawInstruction{"FILE_OPEN", true, InstructionHandler(&instr_FILE_OPEN, REG, ONE_DATA)},
    RawInstruction{"FILE_CREATE", true, InstructionHandler(&instr_FILE_CREATE, REG, ONE_DATA)},
    RawInstruction{"FILE_SAVE", true, InstructionHandler(&instr_FILE_SAVE, REG)},
    RawInstruction{"FILE_DELETE", true, InstructionHandler(&instr_FILE_DELETE, REG)},
    RawInstruction{"FILE_CLOSE", true, InstructionHandler(&instr_FILE_CLOSE, REG)},
    RawInstruction{"FILE_RESET_CURSOR", true, InstructionHandler(&instr_FILE_RESET_CURSOR, REG)},
    RawInstruction{"FILE_CLEAR_DATA", true, InstructionHandler(&instr_FILE_CLEAR_DATA, REG)},
    RawInstruction{"FILE_READ_BYTE", true, InstructionHandler(&instr_FILE_READ_BYTE, REG, REG)},
    RawInstruction{"FILE_READ_WORD", true, InstructionHandler(&instr_FILE_READ_WORD, REG, REG)},
    RawInstruction{"FILE_APPEND_BYTE", true, InstructionHandler(&instr_FILE_APPEND_BYTE, REG, REG)},
    RawInstruction{"FILE_APPEND_WORD", true, InstructionHandler(&instr_FILE_APPEND_WORD, REG, REG)},
    RawInstruction{"FILE_IS_BYTE_REMAINING", true, InstructionHandler(&instr_FILE_IS_BYTE_REMAINING, REG, REG)},
    RawInstruction{"FILE_IS_WORD_REMAINING", true, InstructionHandler(&instr_FILE_IS_WORD_REMAINING, REG, REG)},
    RawInstruction{"CLOCK_CREATE", true, InstructionHandler(&instr_CLOCK_CREATE, REG)},
    RawInstruction{"CLOCK_DELETE", true, InstructionHandler(&instr_CLOCK_DELETE, REG)},
    RawInstruction{"CLOCK_GET_ELAPSED_MS", true, InstructionHandler(&instr_CLOCK_GET_ELAPSED_MS, REG, REG)},
    RawInstruction{"CLOCK_GET_ELAPSED_S", true, InstructionHandler(&instr_CLOCK_GET_ELAPSED_S, REG, REG)},
    RawInstruction{"CLOCK_RESET", true, InstructionHandler(&instr_CLOCK_RESET, REG)}
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
            rawInstructionSet[i].user_usable,
            rawInstructionSet[i].handler_count,
            rawInstructionSet[i].handlers
        };
    }
    return result;
}

constexpr std::array instructionSet = compute_instructions();

#include "check_instructions.hpp"
#include "instruction_utils.hpp"
