//
// Created by Roumite on 20/02/2026.
//

#pragma once

#include <cstdint>
#include <array>
#include <string_view>

#include "interpreter/runtime/InstructionView.h"

struct InstructionView;
class Runtime;

using InstructionFct = void(*)(Runtime &, InstructionView raw_data);

void instr_EOF(Runtime &runtime, InstructionView view);
void instr_LOADD_32(Runtime &runtime, InstructionView view);
void instr_LOADR_32(Runtime &runtime, InstructionView view);
void instr_LOADD_16(Runtime &runtime, InstructionView view);
void instr_LOADR_16(Runtime &runtime, InstructionView view);
void instr_LOADD_8(Runtime &runtime, InstructionView view);
void instr_LOADR_8(Runtime &runtime, InstructionView view);
void instr_STORED_32(Runtime &runtime, InstructionView view);
void instr_STORER_32(Runtime &runtime, InstructionView view);
void instr_STORED_16(Runtime &runtime, InstructionView view);
void instr_STORER_16(Runtime &runtime, InstructionView view);
void instr_STORED_8(Runtime &runtime, InstructionView view);
void instr_STORER_8(Runtime &runtime, InstructionView view);
void instr_EXTEND_8(Runtime &runtime, InstructionView view);
void instr_EXTEND_16(Runtime &runtime, InstructionView view);
void instr_TRUNC_8(Runtime &runtime, InstructionView view);
void instr_TRUNC_16(Runtime &runtime, InstructionView view);
void instr_ADDR(Runtime &runtime, InstructionView view);
void instr_ADDD(Runtime &runtime, InstructionView view);
void instr_SUBR(Runtime &runtime, InstructionView view);
void instr_SUBD(Runtime &runtime, InstructionView view);
void instr_MULR(Runtime &runtime, InstructionView view);
void instr_MULD(Runtime &runtime, InstructionView view);
void instr_DIVR(Runtime &runtime, InstructionView view);
void instr_DIVD(Runtime &runtime, InstructionView view);
void instr_NOT(Runtime &runtime, InstructionView view);
void instr_MINR(Runtime &runtime, InstructionView view);
void instr_MIND(Runtime &runtime, InstructionView view);
void instr_MAXR(Runtime &runtime, InstructionView view);
void instr_MAXD(Runtime &runtime, InstructionView view);
void instr_MODR(Runtime &runtime, InstructionView view);
void instr_MODD(Runtime &runtime, InstructionView view);
void instr_ADDR3(Runtime &runtime, InstructionView view);
void instr_ADDD3(Runtime &runtime, InstructionView view);
void instr_SUBR3(Runtime &runtime, InstructionView view);
void instr_SUBD3(Runtime &runtime, InstructionView view);
void instr_MULR3(Runtime &runtime, InstructionView view);
void instr_MULD3(Runtime &runtime, InstructionView view);
void instr_DIVR3(Runtime &runtime, InstructionView view);
void instr_DIVD3(Runtime &runtime, InstructionView view);
void instr_MINR3(Runtime &runtime, InstructionView view);
void instr_MIND3(Runtime &runtime, InstructionView view);
void instr_MAXR3(Runtime &runtime, InstructionView view);
void instr_MAXD3(Runtime &runtime, InstructionView view);
void instr_MODR3(Runtime &runtime, InstructionView view);
void instr_MODD3(Runtime &runtime, InstructionView view);
void instr_JMP(Runtime &runtime, InstructionView view);
void instr_CMPR(Runtime &runtime, InstructionView view);
void instr_CMPD(Runtime &runtime, InstructionView view);
void instr_JE(Runtime &runtime, InstructionView view);
void instr_JNE(Runtime &runtime, InstructionView view);
void instr_JA(Runtime &runtime, InstructionView view);
void instr_JSA(Runtime &runtime, InstructionView view);
void instr_JB(Runtime &runtime, InstructionView view);
void instr_JSB(Runtime &runtime, InstructionView view);
void instr_DISPLAY_N(Runtime &runtime, InstructionView view);
void instr_DISPLAY_SN(Runtime &runtime, InstructionView view);
void instr_DISPLAY_C(Runtime &runtime, InstructionView view);
void instr_DISPLAY_B(Runtime &runtime, InstructionView view);
void instr_DISPLAY_STR(Runtime &runtime, InstructionView view);
void instr_HALT(Runtime &runtime, InstructionView view);
void instr_HALT1(Runtime &runtime, InstructionView view);
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
void instr_WINDOW_CREATE3(Runtime &runtime, InstructionView view);
void instr_WINDOW_CLOSE(Runtime &runtime, InstructionView view);
void instr_WINDOW_SET_VIEWPORT_SIZE(Runtime &runtime, InstructionView view);
void instr_WINDOW_DISABLE_VIEWPORT(Runtime &runtime, InstructionView view);
void instr_WINDOW_POOL(Runtime &runtime, InstructionView view);
void instr_WINDOW_SHOULD_CLOSE(Runtime &runtime, InstructionView view);
void instr_WINDOW_CLEAR(Runtime &runtime, InstructionView view);
void instr_WINDOW_PRESENT(Runtime &runtime, InstructionView view);
void instr_WINDOW_KEY_PRESSED(Runtime &runtime, InstructionView view);
void instr_WINDOW_KEY_PRESSED2(Runtime &runtime, InstructionView view);
void instr_WINDOW_KEY_DOWN(Runtime &runtime, InstructionView view);
void instr_WINDOW_KEY_DOWN2(Runtime &runtime, InstructionView view);
void instr_WINDOW_SET_TARGET_FPS(Runtime &runtime, InstructionView view);
void instr_WINDOW_SET_TARGET_FPS1(Runtime &runtime, InstructionView view);
void instr_WINDOW_SET_TEXT_SIZE(Runtime &runtime, InstructionView view);
void instr_WINDOW_SET_TEXT_SIZE1(Runtime &runtime, InstructionView view);
void instr_WINDOW_SET_TEXT_COLOR(Runtime &runtime, InstructionView view);
void instr_WINDOW_SET_FONT(Runtime &runtime, InstructionView view);
void instr_WINDOW_SET_FONT1(Runtime &runtime, InstructionView view);
void instr_WINDOW_DRAW_TEXT(Runtime &runtime, InstructionView view);
void instr_WINDOW_DRAW_TEXT3(Runtime &runtime, InstructionView view);
void instr_WINDOW_DRAW_TEXTURE(Runtime &runtime, InstructionView view);
void instr_WINDOW_DRAW_TEXTURE3(Runtime &runtime, InstructionView view);
void instr_WINDOW_SET_TEXTURE_COLOR_MASK(Runtime &runtime, InstructionView view);
void instr_WINDOW_RESET_TEXTURE_COLOR_MASK(Runtime &runtime, InstructionView view);
void instr_WINDOW_SET_ICON(Runtime &runtime, InstructionView view);
void instr_WINDOW_SET_ICON1(Runtime &runtime, InstructionView view);
void instr_WINDOW_MOUSE_X(Runtime &runtime, InstructionView view);
void instr_WINDOW_MOUSE_Y(Runtime &runtime, InstructionView view);
void instr_WINDOW_MOUSE_DELTA_X(Runtime &runtime, InstructionView view);
void instr_WINDOW_MOUSE_DELTA_Y(Runtime &runtime, InstructionView view);
void instr_WINDOW_MOUSE_BUTTON_PRESSED(Runtime &runtime, InstructionView view);
void instr_WINDOW_MOUSE_BUTTON_PRESSED2(Runtime &runtime, InstructionView view);
void instr_WINDOW_MOUSE_BUTTON_DOWN(Runtime &runtime, InstructionView view);
void instr_WINDOW_MOUSE_BUTTON_DOWN2(Runtime &runtime, InstructionView view);
void instr_WINDOW_MOUSE_BUTTON_RELEASED(Runtime &runtime, InstructionView view);
void instr_WINDOW_MOUSE_BUTTON_RELEASED2(Runtime &runtime, InstructionView view);
void instr_WINDOW_MOUSE_WHEEL_DELTA(Runtime &runtime, InstructionView view);
void instr_WINDOW_HIDE_CURSOR(Runtime &runtime, InstructionView view);
void instr_WINDOW_SHOW_CURSOR(Runtime &runtime, InstructionView view);
void instr_WINDOW_TEXTURE_FRAMEBUFFER_CREATE(Runtime &runtime, InstructionView view);
void instr_WINDOW_TEXTURE_FRAMEBUFFER_ADDRESS(Runtime &runtime, InstructionView view);
void instr_WINDOW_TEXTURE_FRAMEBUFFER_SYNC(Runtime &runtime, InstructionView view);
void instr_WINDOW_TEXTURE_FRAMEBUFFER_DRAW(Runtime &runtime, InstructionView view);
void instr_FILE_OPEN(Runtime &runtime, InstructionView view);
void instr_FILE_OPEN2(Runtime &runtime, InstructionView view);
void instr_FILE_CREATE(Runtime &runtime, InstructionView view);
void instr_FILE_CREATE2(Runtime &runtime, InstructionView view);
void instr_FILE_SAVE(Runtime &runtime, InstructionView view);
void instr_FILE_DELETE(Runtime &runtime, InstructionView view);
void instr_FILE_CLOSE(Runtime &runtime, InstructionView view);
void instr_FILE_GET_SIZE(Runtime &runtime, InstructionView view);
void instr_FILE_MAP(Runtime &runtime, InstructionView view);
void instr_FILE_RESET_CURSOR(Runtime &runtime, InstructionView view);
void instr_FILE_SEEK_CURSOR(Runtime &runtime, InstructionView view);
void instr_FILE_CLEAR_DATA(Runtime &runtime, InstructionView view);
void instr_FILE_READ_BYTE(Runtime &runtime, InstructionView view);
void instr_FILE_READ_WORD(Runtime &runtime, InstructionView view);
void instr_FILE_READ_DOUBLEWORD(Runtime &runtime, InstructionView view);
void instr_FILE_APPEND_BYTE(Runtime &runtime, InstructionView view);
void instr_FILE_APPEND_WORD(Runtime &runtime, InstructionView view);
void instr_FILE_APPEND_DOUBLEWORD(Runtime &runtime, InstructionView view);
void instr_FILE_IS_BYTE_REMAINING(Runtime &runtime, InstructionView view);
void instr_FILE_IS_WORD_REMAINING(Runtime &runtime, InstructionView view);
void instr_FILE_IS_DOUBLEWORD_REMAINING(Runtime &runtime, InstructionView view);
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
    RawInstruction{"LOAD_32", true,
        InstructionHandler(&instr_LOADD_32, REG, ONE_DATA),
        InstructionHandler(&instr_LOADR_32, REG, REG_BOTH)
    },
    RawInstruction{"LOAD_16", true,
        InstructionHandler(&instr_LOADD_16, REG, ONE_DATA),
        InstructionHandler(&instr_LOADR_16, REG, REG_BOTH)},
    RawInstruction{"LOAD_8", true,
        InstructionHandler(&instr_LOADD_8, REG, ONE_DATA),
        InstructionHandler(&instr_LOADR_8, REG, REG_BOTH)},
    RawInstruction{"STORE_32", true,
        InstructionHandler(&instr_STORED_32, REG_BOTH, ONE_DATA),
        InstructionHandler(&instr_STORER_32, REG_BOTH, REG)},
    RawInstruction{"STORE_16", true,
        InstructionHandler(&instr_STORED_16, REG_BOTH, ONE_DATA),
        InstructionHandler(&instr_STORER_16, REG_BOTH, REG)},
    RawInstruction{"STORE_8", true,
        InstructionHandler(&instr_STORED_8, REG_BOTH, ONE_DATA),
        InstructionHandler(&instr_STORER_8, REG_BOTH, REG)},
    RawInstruction{"EXTEND_8", true, InstructionHandler(&instr_EXTEND_8, REG, REG_BOTH)},
    RawInstruction{"EXTEND_16", true, InstructionHandler(&instr_EXTEND_16, REG, REG_BOTH)},
    RawInstruction{"TRUNC_8", true, InstructionHandler(&instr_TRUNC_8, REG, REG_BOTH)},
    RawInstruction{"TRUNC_16", true, InstructionHandler(&instr_TRUNC_16, REG, REG_BOTH)},
    RawInstruction{"ADD", true,
        InstructionHandler(&instr_ADDD, REG, ONE_DATA),
        InstructionHandler(&instr_ADDR, REG, REG_BOTH),
        InstructionHandler(&instr_ADDR3, REG, REG_BOTH, REG_BOTH),
        InstructionHandler(&instr_ADDD3, REG, REG_BOTH, ONE_DATA)
    },
    RawInstruction{"SUB", true,
        InstructionHandler(&instr_SUBR, REG, REG_BOTH),
        InstructionHandler(&instr_SUBD, REG, ONE_DATA),
        InstructionHandler(&instr_SUBR3, REG, REG_BOTH, REG_BOTH),
        InstructionHandler(&instr_SUBD3, REG, REG_BOTH, ONE_DATA)
    },
    RawInstruction{"MUL", true,
        InstructionHandler(&instr_MULR, REG, REG_BOTH),
        InstructionHandler(&instr_MULD, REG, ONE_DATA),
        InstructionHandler(&instr_MULR3, REG, REG_BOTH, REG_BOTH),
        InstructionHandler(&instr_MULD3, REG, REG_BOTH, ONE_DATA)
    },
    RawInstruction{"DIV", true,
        InstructionHandler(&instr_DIVR, REG, REG_BOTH),
        InstructionHandler(&instr_DIVD, REG, ONE_DATA),
        InstructionHandler(&instr_DIVR3, REG, REG_BOTH, REG_BOTH),
        InstructionHandler(&instr_DIVD3, REG, REG_BOTH, ONE_DATA)
    },
    RawInstruction{"NOT", true, InstructionHandler(&instr_NOT, REG, REG_BOTH)},
    RawInstruction{"MIN", true,
        InstructionHandler(&instr_MINR, REG, REG_BOTH),
        InstructionHandler(&instr_MIND, REG, ONE_DATA),
        InstructionHandler(&instr_MINR3, REG, REG_BOTH, REG_BOTH),
        InstructionHandler(&instr_MIND3, REG, REG_BOTH, ONE_DATA)
    },
    RawInstruction{"MAX", true,
        InstructionHandler(&instr_MAXR, REG, REG_BOTH),
        InstructionHandler(&instr_MAXD, REG, ONE_DATA),
        InstructionHandler(&instr_MAXR3, REG, REG_BOTH, REG_BOTH),
        InstructionHandler(&instr_MAXD3, REG, REG_BOTH, ONE_DATA)
    },
    RawInstruction{"MOD", true,
        InstructionHandler(&instr_MODR, REG, REG_BOTH),
        InstructionHandler(&instr_MODD, REG, ONE_DATA),
        InstructionHandler(&instr_MODR3, REG, REG_BOTH, REG_BOTH),
        InstructionHandler(&instr_MODD3, REG, REG_BOTH, ONE_DATA)},
    RawInstruction{"JMP", true, InstructionHandler(&instr_JMP, ONE_DATA)},
    RawInstruction{"CMP", true,
        InstructionHandler(&instr_CMPR, REG_BOTH, REG_BOTH),
        InstructionHandler(&instr_CMPD, REG, ONE_DATA)},
    RawInstruction{"JE", true, InstructionHandler(&instr_JE, ONE_DATA)},
    RawInstruction{"JNE", true, InstructionHandler(&instr_JNE, ONE_DATA)},
    RawInstruction{"JA", true, InstructionHandler(&instr_JA, ONE_DATA)},
    RawInstruction{"JSA", true, InstructionHandler(&instr_JSA, ONE_DATA)},
    RawInstruction{"JB", true, InstructionHandler(&instr_JB, ONE_DATA)},
    RawInstruction{"JSB", true, InstructionHandler(&instr_JSB, ONE_DATA)},
    RawInstruction{"DISPLAY_N", true,
        InstructionHandler(&instr_DISPLAY_N, REG_BOTH)},
    RawInstruction{"DISPLAY_SN", true,
        InstructionHandler(&instr_DISPLAY_SN, REG_BOTH)},
    RawInstruction{"DISPLAY_C", true,
        InstructionHandler(&instr_DISPLAY_C, REG_BOTH)},
    RawInstruction{"DISPLAY_B", true,
        InstructionHandler(&instr_DISPLAY_B, REG_BOTH)},
    RawInstruction{"DISPLAY_STR", true,
        InstructionHandler(&instr_DISPLAY_STR, REG_BOTH)},
    RawInstruction{"HALT", true,
        InstructionHandler(&instr_HALT),
        InstructionHandler(&instr_HALT1, REG_BOTH)},
    RawInstruction{"ALOC", true,
        InstructionHandler(&instr_ALOCA, REG, ONE_DATA),
        InstructionHandler(&instr_ALOCR, REG, REG_BOTH)},
    RawInstruction{"FREE", true, InstructionHandler(&instr_FREE, REG)},
    RawInstruction{"DEBUG_R", true, InstructionHandler(&instr_DEBUG_R)},
    RawInstruction{"DEBUG_M", true, InstructionHandler(&instr_DEBUG_M)},
    RawInstruction{"CALL", true, InstructionHandler(&instr_CALL, ONE_DATA)},
    RawInstruction{"RET", true, InstructionHandler(&instr_RET)},
    RawInstruction{"PUSH", true, InstructionHandler(&instr_PUSH, REG)},
    RawInstruction{"POP", true, InstructionHandler(&instr_POP, REG)},
    RawInstruction{"RAND", true, InstructionHandler(&instr_RAND, REG)},
    RawInstruction{"WINDOW_CREATE", true,
        InstructionHandler(&instr_WINDOW_CREATE, REG_BOTH, REG_BOTH, ONE_DATA),
        InstructionHandler(&instr_WINDOW_CREATE3, REG_BOTH, REG_BOTH, REG_BOTH)},
    RawInstruction{"WINDOW_CLOSE", true, InstructionHandler(&instr_WINDOW_CLOSE)},
    RawInstruction{"WINDOW_SET_VIEWPORT_SIZE", true, InstructionHandler(&instr_WINDOW_SET_VIEWPORT_SIZE, REG_BOTH, REG_BOTH)},
    RawInstruction{"WINDOW_DISABLE_VIEWPORT", true, InstructionHandler(&instr_WINDOW_DISABLE_VIEWPORT, REG_BOTH, REG_BOTH)},
    RawInstruction{"WINDOW_POOL", true, InstructionHandler(&instr_WINDOW_POOL)},
    RawInstruction{"WINDOW_SHOULD_CLOSE", true, InstructionHandler(&instr_WINDOW_SHOULD_CLOSE, REG)},
    RawInstruction{"WINDOW_CLEAR", true, InstructionHandler(&instr_WINDOW_CLEAR, REG_BOTH, REG_BOTH, REG_BOTH)},
    RawInstruction{"WINDOW_PRESENT", true, InstructionHandler(&instr_WINDOW_PRESENT)},
    RawInstruction{"WINDOW_KEY_PRESSED", true,
        InstructionHandler(&instr_WINDOW_KEY_PRESSED, REG, ONE_DATA),
        InstructionHandler(&instr_WINDOW_KEY_PRESSED2, REG, REG_BOTH)},
    RawInstruction{"WINDOW_KEY_DOWN", true,
        InstructionHandler(&instr_WINDOW_KEY_DOWN, REG, ONE_DATA),
        InstructionHandler(&instr_WINDOW_KEY_DOWN2, REG, REG_BOTH)},
    RawInstruction{"WINDOW_SET_TARGET_FPS", true,
        InstructionHandler(&instr_WINDOW_SET_TARGET_FPS, ONE_DATA),
        InstructionHandler(&instr_WINDOW_SET_TARGET_FPS1, REG_BOTH)},
    RawInstruction{"WINDOW_SET_TEXT_SIZE", true,
        InstructionHandler(&instr_WINDOW_SET_TEXT_SIZE, ONE_DATA),
        InstructionHandler(&instr_WINDOW_SET_TEXT_SIZE1, REG_BOTH)},
    RawInstruction{"WINDOW_SET_TEXT_COLOR", true, InstructionHandler(&instr_WINDOW_SET_TEXT_COLOR, REG_BOTH, REG_BOTH, REG_BOTH)},
    RawInstruction{"WINDOW_SET_FONT", true,
        InstructionHandler(&instr_WINDOW_SET_FONT, ONE_DATA),
        InstructionHandler(&instr_WINDOW_SET_FONT1, REG_BOTH)},
    RawInstruction{"WINDOW_DRAW_TEXT", true,
        InstructionHandler(&instr_WINDOW_DRAW_TEXT, REG_BOTH, REG_BOTH, ONE_DATA),
        InstructionHandler(&instr_WINDOW_DRAW_TEXT3, REG_BOTH, REG_BOTH, REG_BOTH)},
    RawInstruction{"WINDOW_DRAW_TEXTURE", true,
        InstructionHandler(&instr_WINDOW_DRAW_TEXTURE, REG_BOTH, REG_BOTH, ONE_DATA),
        InstructionHandler(&instr_WINDOW_DRAW_TEXTURE3, REG_BOTH, REG_BOTH, REG_BOTH)},
    RawInstruction{"WINDOW_SET_TEXTURE_COLOR_MASK", true, InstructionHandler(&instr_WINDOW_SET_TEXTURE_COLOR_MASK, REG_BOTH, REG_BOTH, REG_BOTH)},
    RawInstruction{"WINDOW_RESET_TEXTURE_COLOR_MASK", true, InstructionHandler(&instr_WINDOW_RESET_TEXTURE_COLOR_MASK)},
    RawInstruction{"WINDOW_SET_ICON", true,
        InstructionHandler(&instr_WINDOW_SET_ICON, ONE_DATA),
        InstructionHandler(&instr_WINDOW_SET_ICON1, REG_BOTH)},
    RawInstruction{"WINDOW_MOUSE_X", true, InstructionHandler(&instr_WINDOW_MOUSE_X, REG)},
    RawInstruction{"WINDOW_MOUSE_Y", true, InstructionHandler(&instr_WINDOW_MOUSE_Y, REG)},
    RawInstruction{"WINDOW_MOUSE_DELTA_X", true, InstructionHandler(&instr_WINDOW_MOUSE_DELTA_X, REG)},
    RawInstruction{"WINDOW_MOUSE_DELTA_Y", true, InstructionHandler(&instr_WINDOW_MOUSE_DELTA_Y, REG)},
    RawInstruction{"WINDOW_MOUSE_BUTTON_PRESSED", true,
        InstructionHandler(&instr_WINDOW_MOUSE_BUTTON_PRESSED, REG, REG_BOTH),
        InstructionHandler(&instr_WINDOW_MOUSE_BUTTON_PRESSED2, REG, ONE_DATA)},
    RawInstruction{"WINDOW_MOUSE_BUTTON_DOWN", true,
        InstructionHandler(&instr_WINDOW_MOUSE_BUTTON_DOWN, REG, REG_BOTH),
        InstructionHandler(&instr_WINDOW_MOUSE_BUTTON_DOWN2, REG, ONE_DATA)},
    RawInstruction{"WINDOW_MOUSE_BUTTON_RELEASED", true,
        InstructionHandler(&instr_WINDOW_MOUSE_BUTTON_RELEASED, REG, REG_BOTH),
        InstructionHandler(&instr_WINDOW_MOUSE_BUTTON_RELEASED2, REG, ONE_DATA)},
    RawInstruction{"WINDOW_MOUSE_WHEEL_DELTA", true,
        InstructionHandler(&instr_WINDOW_MOUSE_WHEEL_DELTA, REG)},
    RawInstruction{"WINDOW_HIDE_CURSOR", true,
        InstructionHandler(&instr_WINDOW_HIDE_CURSOR)},
    RawInstruction{"WINDOW_WINDOW_SHOW_CURSOR", true,
        InstructionHandler(&instr_WINDOW_SHOW_CURSOR)},
    RawInstruction{"WINDOW_TEXTURE_FRAMEBUFFER_CREATE", true,
        InstructionHandler(&instr_WINDOW_TEXTURE_FRAMEBUFFER_CREATE, REG, REG_BOTH, REG_BOTH)},
    RawInstruction{"WINDOW_TEXTURE_FRAMEBUFFER_ADDRESS", true,
        InstructionHandler(&instr_WINDOW_TEXTURE_FRAMEBUFFER_ADDRESS, REG, REG_BOTH)},
    RawInstruction{"WINDOW_TEXTURE_FRAMEBUFFER_SYNC", true,
        InstructionHandler(&instr_WINDOW_TEXTURE_FRAMEBUFFER_SYNC, REG_BOTH)},
    RawInstruction{"WINDOW_TEXTURE_FRAMEBUFFER_DRAW", true,
        InstructionHandler(&instr_WINDOW_TEXTURE_FRAMEBUFFER_DRAW, REG_BOTH, REG_BOTH, REG_BOTH)},
    RawInstruction{"FILE_OPEN", true,
        InstructionHandler(&instr_FILE_OPEN, REG, ONE_DATA),
        InstructionHandler(&instr_FILE_OPEN2, REG, REG_BOTH)},
    RawInstruction{"FILE_CREATE", true,
        InstructionHandler(&instr_FILE_CREATE, REG, ONE_DATA),
        InstructionHandler(&instr_FILE_CREATE2, REG, REG_BOTH)},
    RawInstruction{"FILE_SAVE", true, InstructionHandler(&instr_FILE_SAVE, REG)},
    RawInstruction{"FILE_DELETE", true, InstructionHandler(&instr_FILE_DELETE, REG)},
    RawInstruction{"FILE_CLOSE", true, InstructionHandler(&instr_FILE_CLOSE, REG)},
    RawInstruction{"FILE_GET_SIZE", true, InstructionHandler(&instr_FILE_GET_SIZE, REG, REG_BOTH)},
    RawInstruction{"FILE_MAP", true, InstructionHandler(&instr_FILE_MAP, REG, REG_BOTH)},
    RawInstruction{"FILE_RESET_CURSOR", true, InstructionHandler(&instr_FILE_RESET_CURSOR, REG)},
    RawInstruction{"FILE_SEEK_CURSOR", true, InstructionHandler(&instr_FILE_SEEK_CURSOR, REG, REG)},
    RawInstruction{"FILE_CLEAR_DATA", true, InstructionHandler(&instr_FILE_CLEAR_DATA, REG)},
    RawInstruction{"FILE_READ_BYTE", true, InstructionHandler(&instr_FILE_READ_BYTE, REG, REG)},
    RawInstruction{"FILE_READ_WORD", true, InstructionHandler(&instr_FILE_READ_WORD, REG, REG)},
    RawInstruction{"FILE_READ_DOUBLEWORD", true, InstructionHandler(&instr_FILE_READ_DOUBLEWORD, REG, REG)},
    RawInstruction{"FILE_APPEND_BYTE", true, InstructionHandler(&instr_FILE_APPEND_BYTE, REG, REG)},
    RawInstruction{"FILE_APPEND_WORD", true, InstructionHandler(&instr_FILE_APPEND_WORD, REG, REG)},
    RawInstruction{"FILE_APPEND_DOUBLEWORD", true, InstructionHandler(&instr_FILE_APPEND_DOUBLEWORD, REG, REG)},
    RawInstruction{"FILE_IS_BYTE_REMAINING", true, InstructionHandler(&instr_FILE_IS_BYTE_REMAINING, REG, REG)},
    RawInstruction{"FILE_IS_WORD_REMAINING", true, InstructionHandler(&instr_FILE_IS_WORD_REMAINING, REG, REG)},
    RawInstruction{"FILE_IS_DOUBLEWORD_REMAINING", true, InstructionHandler(&instr_FILE_IS_DOUBLEWORD_REMAINING, REG, REG)},
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
