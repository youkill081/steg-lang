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
void instr_FADDR(Runtime &runtime, InstructionView view);
void instr_FADDD(Runtime &runtime, InstructionView view);
void instr_SUBR(Runtime &runtime, InstructionView view);
void instr_SUBD(Runtime &runtime, InstructionView view);
void instr_FSUBR(Runtime &runtime, InstructionView view);
void instr_FSUBD(Runtime &runtime, InstructionView view);
void instr_MULR(Runtime &runtime, InstructionView view);
void instr_MULD(Runtime &runtime, InstructionView view);
void instr_FTANR(Runtime &runtime, InstructionView view);
void instr_FTAND(Runtime &runtime, InstructionView view);
void instr_FATANR(Runtime &runtime, InstructionView view);
void instr_FATAND(Runtime &runtime, InstructionView view);
void instr_SMULR(Runtime &runtime, InstructionView view);
void instr_SMULD(Runtime &runtime, InstructionView view);
void instr_FMULR(Runtime &runtime, InstructionView view);
void instr_FMULD(Runtime &runtime, InstructionView view);
void instr_DIVR(Runtime &runtime, InstructionView view);
void instr_DIVD(Runtime &runtime, InstructionView view);
void instr_SDIVR(Runtime &runtime, InstructionView view);
void instr_SDIVD(Runtime &runtime, InstructionView view);
void instr_FDIVR(Runtime &runtime, InstructionView view);
void instr_FDIVD(Runtime &runtime, InstructionView view);
void instr_NOT(Runtime &runtime, InstructionView view);
void instr_FNOT(Runtime &runtime, InstructionView view);
void instr_MINR(Runtime &runtime, InstructionView view);
void instr_MIND(Runtime &runtime, InstructionView view);
void instr_FMINR(Runtime &runtime, InstructionView view);
void instr_FMIND(Runtime &runtime, InstructionView view);
void instr_MAXR(Runtime &runtime, InstructionView view);
void instr_MAXD(Runtime &runtime, InstructionView view);
void instr_FMAXR(Runtime &runtime, InstructionView view);
void instr_FMAXD(Runtime &runtime, InstructionView view);
void instr_MODR(Runtime &runtime, InstructionView view);
void instr_MODD(Runtime &runtime, InstructionView view);

void instr_BIT_ANDR2(Runtime &runtime, InstructionView view);
void instr_BIT_ANDD2(Runtime &runtime, InstructionView view);
void instr_BIT_ANDR3(Runtime &runtime, InstructionView view);
void instr_BIT_ANDD3(Runtime &runtime, InstructionView view);

void instr_BIT_ORR2(Runtime &runtime, InstructionView view);
void instr_BIT_ORD2(Runtime &runtime, InstructionView view);
void instr_BIT_ORR3(Runtime &runtime, InstructionView view);
void instr_BIT_ORD3(Runtime &runtime, InstructionView view);

void instr_BIT_XORR2(Runtime &runtime, InstructionView view);
void instr_BIT_XORD2(Runtime &runtime, InstructionView view);
void instr_BIT_XORR3(Runtime &runtime, InstructionView view);
void instr_BIT_XORD3(Runtime &runtime, InstructionView view);

void instr_BIT_NOTR(Runtime &runtime, InstructionView view);
void instr_BIT_NOTD(Runtime &runtime, InstructionView view);

void instr_BIT_SHIFT_LEFTR2(Runtime &runtime, InstructionView view);
void instr_BIT_SHIFT_LEFTD2(Runtime &runtime, InstructionView view);
void instr_BIT_SHIFT_LEFTR3(Runtime &runtime, InstructionView view);
void instr_BIT_SHIFT_LEFTD3(Runtime &runtime, InstructionView view);

void instr_BIT_SHIFT_RIGHTR2(Runtime &runtime, InstructionView view);
void instr_BIT_SHIFT_RIGHTD2(Runtime &runtime, InstructionView view);
void instr_BIT_SHIFT_RIGHTR3(Runtime &runtime, InstructionView view);
void instr_BIT_SHIFT_RIGHTD3(Runtime &runtime, InstructionView view);

void instr_BIT_S_SHIFT_RIGHTR2(Runtime &runtime, InstructionView view);
void instr_BIT_S_SHIFT_RIGHTD2(Runtime &runtime, InstructionView view);
void instr_BIT_S_SHIFT_RIGHTR3(Runtime &runtime, InstructionView view);
void instr_BIT_S_SHIFT_RIGHTD3(Runtime &runtime, InstructionView view);




void instr_FMODR(Runtime &runtime, InstructionView view);
void instr_FMODD(Runtime &runtime, InstructionView view);
void instr_FSINR(Runtime &runtime, InstructionView view);
void instr_FSIND(Runtime &runtime, InstructionView view);
void instr_FCOSR(Runtime &runtime, InstructionView view);
void instr_FCOSD(Runtime &runtime, InstructionView view);
void instr_FSQRTR(Runtime &runtime, InstructionView view);
void instr_FSQRTD(Runtime &runtime, InstructionView view);
void instr_FABSR(Runtime &runtime, InstructionView view);
void instr_FABSD(Runtime &runtime, InstructionView view);
void instr_FDEG2RADR(Runtime &runtime, InstructionView view);
void instr_FDEG2RADD(Runtime &runtime, InstructionView view);
void instr_FRAD2DEGR(Runtime &runtime, InstructionView view);
void instr_FRAD2DEGD(Runtime &runtime, InstructionView view);
void instr_ADDR3(Runtime &runtime, InstructionView view);
void instr_ADDD3(Runtime &runtime, InstructionView view);
void instr_FADDD3(Runtime &runtime, InstructionView view);
void instr_FADDR3(Runtime &runtime, InstructionView view);
void instr_SUBR3(Runtime &runtime, InstructionView view);
void instr_SUBD3(Runtime &runtime, InstructionView view);
void instr_FSUBR3(Runtime &runtime, InstructionView view);
void instr_FSUBD3(Runtime &runtime, InstructionView view);
void instr_MULR3(Runtime &runtime, InstructionView view);
void instr_MULD3(Runtime &runtime, InstructionView view);
void instr_SMULR3(Runtime &runtime, InstructionView view);
void instr_SMULD3(Runtime &runtime, InstructionView view);
void instr_FMULD3(Runtime &runtime, InstructionView view);
void instr_FMULR3(Runtime &runtime, InstructionView view);
void instr_DIVR3(Runtime &runtime, InstructionView view);
void instr_DIVD3(Runtime &runtime, InstructionView view);
void instr_SDIVR3(Runtime &runtime, InstructionView view);
void instr_SDIVD3(Runtime &runtime, InstructionView view);
void instr_FDIVR3(Runtime &runtime, InstructionView view);
void instr_FDIVD3(Runtime &runtime, InstructionView view);
void instr_MINR3(Runtime &runtime, InstructionView view);
void instr_MIND3(Runtime &runtime, InstructionView view);
void instr_FMINR3(Runtime &runtime, InstructionView view);
void instr_FMIND3(Runtime &runtime, InstructionView view);
void instr_MAXR3(Runtime &runtime, InstructionView view);
void instr_MAXD3(Runtime &runtime, InstructionView view);
void instr_FMAXR3(Runtime &runtime, InstructionView view);
void instr_FMAXD3(Runtime &runtime, InstructionView view);
void instr_MODR3(Runtime &runtime, InstructionView view);
void instr_MODD3(Runtime &runtime, InstructionView view);
void instr_FMODR3(Runtime &runtime, InstructionView view);
void instr_FMODD3(Runtime &runtime, InstructionView view);
void instr_JMP(Runtime &runtime, InstructionView view);
void instr_CMPR(Runtime &runtime, InstructionView view);
void instr_CMPD(Runtime &runtime, InstructionView view);
void instr_FCMPR(Runtime &runtime, InstructionView view);
void instr_FCMPD(Runtime &runtime, InstructionView view);
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
void instr_DISPLAY_F(Runtime &runtime, InstructionView view);
void instr_DISPLAY_STR(Runtime &runtime, InstructionView view);
void instr_HALT(Runtime &runtime, InstructionView view);
void instr_HALT1(Runtime &runtime, InstructionView view);
void instr_ALOCA(Runtime &runtime, InstructionView view);
void instr_ALOCR(Runtime &runtime, InstructionView view);
void instr_FREE(Runtime &runtime, InstructionView view);
void instr_DEBUG_R(Runtime &runtime, InstructionView view);
void instr_DEBUG_M(Runtime &runtime, InstructionView view);
void instr_DEBUG_TR(Runtime &runtime, InstructionView view);
void instr_DEBUG_TD(Runtime &runtime, InstructionView view);
void instr_CALL(Runtime &runtime, InstructionView view);
void instr_RET(Runtime &runtime, InstructionView view);
void instr_PUSH(Runtime &runtime, InstructionView view);
void instr_POP(Runtime &runtime, InstructionView view);
void instr_RAND(Runtime &runtime, InstructionView view);
void instr_ITOF(Runtime &runtime, InstructionView view);
void instr_UTOF(Runtime &runtime, InstructionView view);
void instr_FTOI(Runtime &runtime, InstructionView view);
void instr_FTOU(Runtime &runtime, InstructionView view);
void instr_MAP_IMAGER(Runtime &runtime, InstructionView view);
void instr_MAP_IMAGED(Runtime &runtime, InstructionView view);
void instr_MAP_IMAGE_X(Runtime &runtime, InstructionView view);
void instr_MAP_IMAGE_SIZE_Y(Runtime &runtime, InstructionView view);
void instr_WINDOW_CREATE(Runtime &runtime, InstructionView view);
void instr_WINDOW_CREATE3(Runtime &runtime, InstructionView view);
void instr_WINDOW_CLOSE(Runtime &runtime, InstructionView view);
void instr_WINDOW_SET_VIEWPORT_SIZE(Runtime &runtime, InstructionView view);
void instr_WINDOW_DISABLE_VIEWPORT(Runtime &runtime, InstructionView view);
void instr_WINDOW_POOL(Runtime &runtime, InstructionView view);
void instr_WINDOW_SHOULD_CLOSE(Runtime &runtime, InstructionView view);
void instr_WINDOW_CLEAR(Runtime &runtime, InstructionView view);
void instr_WINDOW_PRESENT(Runtime &runtime, InstructionView view);
void instr_WINDOW_TOGGLE_FULLSCREEN(Runtime &runtime, InstructionView view);
void instr_WINDOW_KEY_PRESSED(Runtime &runtime, InstructionView view);
void instr_WINDOW_KEY_PRESSED2(Runtime &runtime, InstructionView view);
void instr_WINDOW_KEY_DOWN(Runtime &runtime, InstructionView view);
void instr_WINDOW_KEY_DOWN2(Runtime &runtime, InstructionView view);
void instr_WINDOW_SET_TARGET_FPS(Runtime &runtime, InstructionView view);
void instr_WINDOW_SET_TARGET_FPS1(Runtime &runtime, InstructionView view);
void instr_WINDOW_GET_DELTA(Runtime &runtime, InstructionView view);
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
void instr_WINDOW_DISABLE_CURSOR(Runtime &runtime, InstructionView view);
void instr_WINDOW_ENABLE_CURSOR(Runtime &runtime, InstructionView view);
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
void instr_FILE_MAP_FROM_CURSOR(Runtime &runtime, InstructionView view);
void instr_FILE_RESET_CURSOR(Runtime &runtime, InstructionView view);
void instr_FILE_SEEK_CURSOR(Runtime &runtime, InstructionView view);
void instr_FILE_GET_CURSOR(Runtime &runtime, InstructionView view);
void instr_FILE_CLEAR_DATA(Runtime &runtime, InstructionView view);
void instr_FILE_READ_BYTE(Runtime &runtime, InstructionView view);
void instr_FILE_READ_BYTE_ATR(Runtime &runtime, InstructionView view);
void instr_FILE_READ_BYTE_ATD(Runtime &runtime, InstructionView view);
void instr_FILE_READ_WORD(Runtime &runtime, InstructionView view);
void instr_FILE_READ_WORD_ATR(Runtime &runtime, InstructionView view);
void instr_FILE_READ_WORD_ATD(Runtime &runtime, InstructionView view);
void instr_FILE_READ_WORD_LITTLE(Runtime &runtime, InstructionView view);
void instr_FILE_READ_WORD_LITTLE_ATR(Runtime &runtime, InstructionView view);
void instr_FILE_READ_WORD_LITTLE_ATD(Runtime &runtime, InstructionView view);
void instr_FILE_READ_DOUBLEWORD(Runtime &runtime, InstructionView view);
void instr_FILE_READ_DOUBLEWORD_ATR(Runtime &runtime, InstructionView view);
void instr_FILE_READ_DOUBLEWORD_ATD(Runtime &runtime, InstructionView view);
void instr_FILE_READ_DOUBLEWORD_LITTLE(Runtime &runtime, InstructionView view);
void instr_FILE_READ_DOUBLEWORD_LITTLE_ATR(Runtime &runtime, InstructionView view);
void instr_FILE_READ_DOUBLEWORD_LITTLE_ATD(Runtime &runtime, InstructionView view);
void instr_FILE_APPEND_BYTE(Runtime &runtime, InstructionView view);
void instr_FILE_APPEND_WORD(Runtime &runtime, InstructionView view);
void instr_FILE_APPEND_WORD_LITTLE(Runtime &runtime, InstructionView view);
void instr_FILE_APPEND_DOUBLEWORD(Runtime &runtime, InstructionView view);
void instr_FILE_APPEND_DOUBLEWORD_LITTLE(Runtime &runtime, InstructionView view);
void instr_FILE_WRITE_BYTE(Runtime &runtime, InstructionView view);
void instr_FILE_WRITE_WORD(Runtime &runtime, InstructionView view);
void instr_FILE_WRITE_WORD_LITTLE(Runtime &runtime, InstructionView view);
void instr_FILE_WRITE_DOUBLEWORD(Runtime &runtime, InstructionView view);
void instr_FILE_WRITE_DOUBLEWORD_LITTLE(Runtime &runtime, InstructionView view);
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

#define INSTRUCTION_LIST(X) \
    X(EOF, false, InstructionHandler(&instr_EOF)) \
    X(LOAD_32, true, \
        InstructionHandler(&instr_LOADD_32, REG, ONE_DATA), \
        InstructionHandler(&instr_LOADR_32, REG, REG_BOTH)) \
    X(LOAD_16, true, \
        InstructionHandler(&instr_LOADD_16, REG, ONE_DATA), \
        InstructionHandler(&instr_LOADR_16, REG, REG_BOTH)) \
    X(LOAD_8, true, \
        InstructionHandler(&instr_LOADD_8, REG, ONE_DATA), \
        InstructionHandler(&instr_LOADR_8, REG, REG_BOTH)) \
    X(STORE_32, true, \
        InstructionHandler(&instr_STORED_32, REG_BOTH, ONE_DATA), \
        InstructionHandler(&instr_STORER_32, REG_BOTH, REG)) \
    X(STORE_16, true, \
        InstructionHandler(&instr_STORED_16, REG_BOTH, ONE_DATA), \
        InstructionHandler(&instr_STORER_16, REG_BOTH, REG)) \
    X(STORE_8, true, \
        InstructionHandler(&instr_STORED_8, REG_BOTH, ONE_DATA), \
        InstructionHandler(&instr_STORER_8, REG_BOTH, REG)) \
    X(EXTEND_8, true, InstructionHandler(&instr_EXTEND_8, REG, REG_BOTH)) \
    X(EXTEND_16, true, InstructionHandler(&instr_EXTEND_16, REG, REG_BOTH)) \
    X(TRUNC_8, true, InstructionHandler(&instr_TRUNC_8, REG, REG_BOTH)) \
    X(TRUNC_16, true, InstructionHandler(&instr_TRUNC_16, REG, REG_BOTH)) \
    X(ADD, true, \
        InstructionHandler(&instr_ADDD, REG, ONE_DATA), \
        InstructionHandler(&instr_ADDR, REG, REG_BOTH), \
        InstructionHandler(&instr_ADDR3, REG, REG_BOTH, REG_BOTH), \
        InstructionHandler(&instr_ADDD3, REG, REG_BOTH, ONE_DATA) \
    ) \
    X(FADD, true, \
        InstructionHandler(&instr_FADDD, REG, ONE_DATA), \
        InstructionHandler(&instr_FADDR, REG, REG_BOTH), \
        InstructionHandler(&instr_FADDR3, REG, REG_BOTH, REG_BOTH), \
        InstructionHandler(&instr_FADDD3, REG, REG_BOTH, ONE_DATA) \
    ) \
    X(SUB, true, \
        InstructionHandler(&instr_SUBR, REG, REG_BOTH), \
        InstructionHandler(&instr_SUBD, REG, ONE_DATA), \
        InstructionHandler(&instr_SUBR3, REG, REG_BOTH, REG_BOTH), \
        InstructionHandler(&instr_SUBD3, REG, REG_BOTH, ONE_DATA) \
    ) \
    X(FSUB, true, \
        InstructionHandler(&instr_FSUBR, REG, REG_BOTH), \
        InstructionHandler(&instr_FSUBD, REG, ONE_DATA), \
        InstructionHandler(&instr_FSUBR3, REG, REG_BOTH, REG_BOTH), \
        InstructionHandler(&instr_FSUBD3, REG, REG_BOTH, ONE_DATA) \
    ) \
    X(MUL, true, \
        InstructionHandler(&instr_MULR, REG, REG_BOTH), \
        InstructionHandler(&instr_MULD, REG, ONE_DATA), \
        InstructionHandler(&instr_MULR3, REG, REG_BOTH, REG_BOTH), \
        InstructionHandler(&instr_MULD3, REG, REG_BOTH, ONE_DATA) \
    ) \
    X(FTAN, true, \
        InstructionHandler(&instr_FTANR, REG, REG_BOTH), \
        InstructionHandler(&instr_FTAND, REG, ONE_DATA) \
    ) \
    X(FATAN, true, \
        InstructionHandler(&instr_FATANR, REG, REG_BOTH), \
        InstructionHandler(&instr_FATAND, REG, ONE_DATA) \
    ) \
    X(FMUL, true, \
        InstructionHandler(&instr_FMULR, REG, REG_BOTH), \
        InstructionHandler(&instr_FMULD, REG, ONE_DATA), \
        InstructionHandler(&instr_FMULR3, REG, REG_BOTH, REG_BOTH), \
        InstructionHandler(&instr_FMULD3, REG, REG_BOTH, ONE_DATA) \
    ) \
    X(SMUL, true, \
        InstructionHandler(&instr_SMULR, REG, REG_BOTH), \
        InstructionHandler(&instr_SMULD, REG, ONE_DATA), \
        InstructionHandler(&instr_SMULR3, REG, REG_BOTH, REG_BOTH), \
        InstructionHandler(&instr_SMULD3, REG, REG_BOTH, ONE_DATA) \
    ) \
    X(DIV, true, \
        InstructionHandler(&instr_DIVR, REG, REG_BOTH), \
        InstructionHandler(&instr_DIVD, REG, ONE_DATA), \
        InstructionHandler(&instr_DIVR3, REG, REG_BOTH, REG_BOTH), \
        InstructionHandler(&instr_DIVD3, REG, REG_BOTH, ONE_DATA) \
    ) \
    X(FDIV, true, \
        InstructionHandler(&instr_FDIVR, REG, REG_BOTH), \
        InstructionHandler(&instr_FDIVD, REG, ONE_DATA), \
        InstructionHandler(&instr_FDIVR3, REG, REG_BOTH, REG_BOTH), \
        InstructionHandler(&instr_FDIVD3, REG, REG_BOTH, ONE_DATA) \
    ) \
    X(SDIV, true, \
        InstructionHandler(&instr_SDIVR, REG, REG_BOTH), \
        InstructionHandler(&instr_SDIVD, REG, ONE_DATA), \
        InstructionHandler(&instr_SDIVR3, REG, REG_BOTH, REG_BOTH), \
        InstructionHandler(&instr_SDIVD3, REG, REG_BOTH, ONE_DATA) \
    ) \
    X(NOT, true, InstructionHandler(&instr_NOT, REG, REG_BOTH)) \
    X(FNOT, true, InstructionHandler(&instr_FNOT, REG, REG_BOTH)) \
    X(MIN, true, \
        InstructionHandler(&instr_MINR, REG, REG_BOTH), \
        InstructionHandler(&instr_MIND, REG, ONE_DATA), \
        InstructionHandler(&instr_MINR3, REG, REG_BOTH, REG_BOTH), \
        InstructionHandler(&instr_MIND3, REG, REG_BOTH, ONE_DATA) \
    ) \
    X(FMIN, true, \
        InstructionHandler(&instr_FMINR, REG, REG_BOTH), \
        InstructionHandler(&instr_FMIND, REG, ONE_DATA), \
        InstructionHandler(&instr_FMINR3, REG, REG_BOTH, REG_BOTH), \
        InstructionHandler(&instr_FMIND3, REG, REG_BOTH, ONE_DATA) \
    ) \
    X(MAX, true, \
        InstructionHandler(&instr_MAXR, REG, REG_BOTH), \
        InstructionHandler(&instr_MAXD, REG, ONE_DATA), \
        InstructionHandler(&instr_MAXR3, REG, REG_BOTH, REG_BOTH), \
        InstructionHandler(&instr_MAXD3, REG, REG_BOTH, ONE_DATA) \
    ) \
    X(FMAX, true, \
        InstructionHandler(&instr_FMAXR, REG, REG_BOTH), \
        InstructionHandler(&instr_FMAXD, REG, ONE_DATA), \
        InstructionHandler(&instr_FMAXR3, REG, REG_BOTH, REG_BOTH), \
        InstructionHandler(&instr_FMAXD3, REG, REG_BOTH, ONE_DATA) \
    ) \
    X(MOD, true, \
        InstructionHandler(&instr_MODR, REG, REG_BOTH), \
        InstructionHandler(&instr_MODD, REG, ONE_DATA), \
        InstructionHandler(&instr_MODR3, REG, REG_BOTH, REG_BOTH), \
        InstructionHandler(&instr_MODD3, REG, REG_BOTH, ONE_DATA) \
    ) \
    X(FMOD, true, \
        InstructionHandler(&instr_FMODR, REG, REG_BOTH), \
        InstructionHandler(&instr_FMODD, REG, ONE_DATA), \
        InstructionHandler(&instr_FMODR3, REG, REG_BOTH, REG_BOTH), \
        InstructionHandler(&instr_FMODD3, REG, REG_BOTH, ONE_DATA) \
    ) \
    X(AND, true, \
        InstructionHandler(&instr_BIT_ANDR2, REG, REG_BOTH), \
        InstructionHandler(&instr_BIT_ANDD2, REG, ONE_DATA), \
        InstructionHandler(&instr_BIT_ANDR3, REG, REG_BOTH, REG_BOTH), \
        InstructionHandler(&instr_BIT_ANDD3, REG, REG_BOTH, ONE_DATA) \
    ) \
    X(OR, true, \
        InstructionHandler(&instr_BIT_ORR2, REG, REG_BOTH), \
        InstructionHandler(&instr_BIT_ORD2, REG, ONE_DATA), \
        InstructionHandler(&instr_BIT_ORR3, REG, REG_BOTH, REG_BOTH), \
        InstructionHandler(&instr_BIT_ORD3, REG, REG_BOTH, ONE_DATA) \
    ) \
    X(XOR, true, \
        InstructionHandler(&instr_BIT_XORR2, REG, REG_BOTH), \
        InstructionHandler(&instr_BIT_XORD2, REG, ONE_DATA), \
        InstructionHandler(&instr_BIT_XORR3, REG, REG_BOTH, REG_BOTH), \
        InstructionHandler(&instr_BIT_XORD3, REG, REG_BOTH, ONE_DATA) \
    ) \
    X(BIT_NOT, true, \
        InstructionHandler(&instr_BIT_NOTR, REG, REG_BOTH), \
        InstructionHandler(&instr_BIT_NOTD, REG, ONE_DATA) \
    ) \
    X(SHIFT_LEFT, true, \
        InstructionHandler(&instr_BIT_SHIFT_LEFTR2, REG, REG_BOTH), \
        InstructionHandler(&instr_BIT_SHIFT_LEFTD2, REG, ONE_DATA), \
        InstructionHandler(&instr_BIT_SHIFT_LEFTR3, REG, REG_BOTH, REG_BOTH), \
        InstructionHandler(&instr_BIT_SHIFT_LEFTD3, REG, REG_BOTH, ONE_DATA) \
    ) \
    X(SHIFT_RIGHT, true, \
        InstructionHandler(&instr_BIT_SHIFT_RIGHTR2, REG, REG_BOTH), \
        InstructionHandler(&instr_BIT_SHIFT_RIGHTD2, REG, ONE_DATA), \
        InstructionHandler(&instr_BIT_SHIFT_RIGHTR3, REG, REG_BOTH, REG_BOTH), \
        InstructionHandler(&instr_BIT_SHIFT_RIGHTD3, REG, REG_BOTH, ONE_DATA) \
    ) \
    X(SSHIFT_RIGHT, true, \
        InstructionHandler(&instr_BIT_S_SHIFT_RIGHTR2, REG, REG_BOTH), \
        InstructionHandler(&instr_BIT_S_SHIFT_RIGHTD2, REG, ONE_DATA), \
        InstructionHandler(&instr_BIT_S_SHIFT_RIGHTR3, REG, REG_BOTH, REG_BOTH), \
        InstructionHandler(&instr_BIT_S_SHIFT_RIGHTD3, REG, REG_BOTH, ONE_DATA) \
    ) \
    X(FSIN, true, \
        InstructionHandler(&instr_FSINR, REG, REG_BOTH), \
        InstructionHandler(&instr_FSIND, REG, ONE_DATA) \
    ) \
    X(FCOS, true, \
        InstructionHandler(&instr_FCOSR, REG, REG_BOTH), \
        InstructionHandler(&instr_FCOSD, REG, ONE_DATA) \
    ) \
    X(FSQRT, true, \
        InstructionHandler(&instr_FSQRTR, REG, REG_BOTH), \
        InstructionHandler(&instr_FSQRTD, REG, ONE_DATA) \
    ) \
    X(FABS, true, \
        InstructionHandler(&instr_FABSR, REG, REG_BOTH), \
        InstructionHandler(&instr_FABSD, REG, ONE_DATA) \
    ) \
    X(FDEG2RAD, true, \
        InstructionHandler(&instr_FDEG2RADR, REG, REG_BOTH), \
        InstructionHandler(&instr_FDEG2RADD, REG, ONE_DATA) \
    ) \
    X(FRAD2DEG, true, \
        InstructionHandler(&instr_FRAD2DEGR, REG, REG_BOTH), \
        InstructionHandler(&instr_FRAD2DEGD, REG, ONE_DATA) \
    ) \
    X(JMP, true, InstructionHandler(&instr_JMP, ONE_DATA)) \
    X(CMP, true, \
        InstructionHandler(&instr_CMPR, REG_BOTH, REG_BOTH), \
        InstructionHandler(&instr_CMPD, REG, ONE_DATA)) \
    X(FCMP, true, \
        InstructionHandler(&instr_FCMPR, REG_BOTH, REG_BOTH), \
        InstructionHandler(&instr_FCMPD, REG, ONE_DATA)) \
    X(JE, true, InstructionHandler(&instr_JE, ONE_DATA)) \
    X(JNE, true, InstructionHandler(&instr_JNE, ONE_DATA)) \
    X(JA, true, InstructionHandler(&instr_JA, ONE_DATA)) \
    X(JSA, true, InstructionHandler(&instr_JSA, ONE_DATA)) \
    X(JB, true, InstructionHandler(&instr_JB, ONE_DATA)) \
    X(JSB, true, InstructionHandler(&instr_JSB, ONE_DATA)) \
    X(DISPLAY_N, true, InstructionHandler(&instr_DISPLAY_N, REG_BOTH)) \
    X(DISPLAY_SN, true, InstructionHandler(&instr_DISPLAY_SN, REG_BOTH)) \
    X(DISPLAY_C, true, InstructionHandler(&instr_DISPLAY_C, REG_BOTH)) \
    X(DISPLAY_B, true, InstructionHandler(&instr_DISPLAY_B, REG_BOTH)) \
    X(DISPLAY_F, true, InstructionHandler(&instr_DISPLAY_F, REG_BOTH)) \
    X(DISPLAY_STR, true, InstructionHandler(&instr_DISPLAY_STR, REG_BOTH)) \
    X(HALT, true, \
        InstructionHandler(&instr_HALT), \
        InstructionHandler(&instr_HALT1, REG_BOTH)) \
    X(ALOC, true, \
        InstructionHandler(&instr_ALOCA, REG, ONE_DATA), \
        InstructionHandler(&instr_ALOCR, REG, REG_BOTH)) \
    X(FREE, true, InstructionHandler(&instr_FREE, REG)) \
    X(DEBUG_R, true, InstructionHandler(&instr_DEBUG_R)) \
    X(DEBUG_M, true, InstructionHandler(&instr_DEBUG_M)) \
    X(DEBUG_T, true, \
        InstructionHandler(&instr_DEBUG_TR, REG_BOTH), \
        InstructionHandler(&instr_DEBUG_TD, ONE_DATA)) \
    X(CALL, true, InstructionHandler(&instr_CALL, ONE_DATA)) \
    X(RET, true, InstructionHandler(&instr_RET)) \
    X(PUSH, true, InstructionHandler(&instr_PUSH, REG)) \
    X(POP, true, InstructionHandler(&instr_POP, REG)) \
    X(RAND, true, InstructionHandler(&instr_RAND, REG)) \
    X(ITOF, true, InstructionHandler(&instr_ITOF, REG, REG_BOTH)) \
    X(UTOF, true, InstructionHandler(&instr_UTOF, REG, REG_BOTH)) \
    X(FTOI, true, InstructionHandler(&instr_FTOI, REG, REG_BOTH)) \
    X(FTOU, true, InstructionHandler(&instr_FTOU, REG, REG_BOTH)) \
    X(MAP_IMAGE, true, \
        InstructionHandler(&instr_MAP_IMAGER, REG, REG_BOTH), \
        InstructionHandler(&instr_MAP_IMAGED, REG, ONE_DATA)) \
    X(MAP_IMAGE_SIZE_X, true, InstructionHandler(&instr_MAP_IMAGE_X, REG)) \
    X(MAP_IMAGE_SIZE_Y, true, InstructionHandler(&instr_MAP_IMAGE_SIZE_Y, REG)) \
    X(WINDOW_CREATE, true, \
        InstructionHandler(&instr_WINDOW_CREATE, REG_BOTH, REG_BOTH, ONE_DATA), \
        InstructionHandler(&instr_WINDOW_CREATE3, REG_BOTH, REG_BOTH, REG_BOTH)) \
    X(WINDOW_CLOSE, true, InstructionHandler(&instr_WINDOW_CLOSE)) \
    X(WINDOW_SET_VIEWPORT_SIZE, true, InstructionHandler(&instr_WINDOW_SET_VIEWPORT_SIZE, REG_BOTH, REG_BOTH)) \
    X(WINDOW_DISABLE_VIEWPORT, true, InstructionHandler(&instr_WINDOW_DISABLE_VIEWPORT, REG_BOTH, REG_BOTH)) \
    X(WINDOW_POOL, true, InstructionHandler(&instr_WINDOW_POOL)) \
    X(WINDOW_SHOULD_CLOSE, true, InstructionHandler(&instr_WINDOW_SHOULD_CLOSE, REG)) \
    X(WINDOW_CLEAR, true, InstructionHandler(&instr_WINDOW_CLEAR, REG_BOTH, REG_BOTH, REG_BOTH)) \
    X(WINDOW_PRESENT, true, InstructionHandler(&instr_WINDOW_PRESENT)) \
    X(WINDOW_TOGGLE_FULLSCREEN, true, InstructionHandler(&instr_WINDOW_TOGGLE_FULLSCREEN)) \
    X(WINDOW_KEY_PRESSED, true, \
        InstructionHandler(&instr_WINDOW_KEY_PRESSED, REG, ONE_DATA), \
        InstructionHandler(&instr_WINDOW_KEY_PRESSED2, REG, REG_BOTH)) \
    X(WINDOW_KEY_DOWN, true, \
        InstructionHandler(&instr_WINDOW_KEY_DOWN, REG, ONE_DATA), \
        InstructionHandler(&instr_WINDOW_KEY_DOWN2, REG, REG_BOTH)) \
    X(WINDOW_SET_TARGET_FPS, true, \
        InstructionHandler(&instr_WINDOW_SET_TARGET_FPS, ONE_DATA), \
        InstructionHandler(&instr_WINDOW_SET_TARGET_FPS1, REG_BOTH)) \
    X(WINDOW_GET_DELTA, true, InstructionHandler(&instr_WINDOW_GET_DELTA, REG)) \
    X(WINDOW_SET_TEXT_SIZE, true, \
        InstructionHandler(&instr_WINDOW_SET_TEXT_SIZE, ONE_DATA), \
        InstructionHandler(&instr_WINDOW_SET_TEXT_SIZE1, REG_BOTH)) \
    X(WINDOW_SET_TEXT_COLOR, true, InstructionHandler(&instr_WINDOW_SET_TEXT_COLOR, REG_BOTH, REG_BOTH, REG_BOTH)) \
    X(WINDOW_SET_FONT, true, \
        InstructionHandler(&instr_WINDOW_SET_FONT, ONE_DATA), \
        InstructionHandler(&instr_WINDOW_SET_FONT1, REG_BOTH)) \
    X(WINDOW_DRAW_TEXT, true, \
        InstructionHandler(&instr_WINDOW_DRAW_TEXT, REG_BOTH, REG_BOTH, ONE_DATA), \
        InstructionHandler(&instr_WINDOW_DRAW_TEXT3, REG_BOTH, REG_BOTH, REG_BOTH)) \
    X(WINDOW_DRAW_TEXTURE, true, \
        InstructionHandler(&instr_WINDOW_DRAW_TEXTURE, REG_BOTH, REG_BOTH, ONE_DATA), \
        InstructionHandler(&instr_WINDOW_DRAW_TEXTURE3, REG_BOTH, REG_BOTH, REG_BOTH)) \
    X(WINDOW_SET_TEXTURE_COLOR_MASK, true, InstructionHandler(&instr_WINDOW_SET_TEXTURE_COLOR_MASK, REG_BOTH, REG_BOTH, REG_BOTH)) \
    X(WINDOW_RESET_TEXTURE_COLOR_MASK, true, InstructionHandler(&instr_WINDOW_RESET_TEXTURE_COLOR_MASK)) \
    X(WINDOW_SET_ICON, true, \
        InstructionHandler(&instr_WINDOW_SET_ICON, ONE_DATA), \
        InstructionHandler(&instr_WINDOW_SET_ICON1, REG_BOTH)) \
    X(WINDOW_MOUSE_X, true, InstructionHandler(&instr_WINDOW_MOUSE_X, REG)) \
    X(WINDOW_MOUSE_Y, true, InstructionHandler(&instr_WINDOW_MOUSE_Y, REG)) \
    X(WINDOW_MOUSE_DELTA_X, true, InstructionHandler(&instr_WINDOW_MOUSE_DELTA_X, REG)) \
    X(WINDOW_MOUSE_DELTA_Y, true, InstructionHandler(&instr_WINDOW_MOUSE_DELTA_Y, REG)) \
    X(WINDOW_MOUSE_BUTTON_PRESSED, true, \
        InstructionHandler(&instr_WINDOW_MOUSE_BUTTON_PRESSED, REG, REG_BOTH), \
        InstructionHandler(&instr_WINDOW_MOUSE_BUTTON_PRESSED2, REG, ONE_DATA)) \
    X(WINDOW_MOUSE_BUTTON_DOWN, true, \
        InstructionHandler(&instr_WINDOW_MOUSE_BUTTON_DOWN, REG, REG_BOTH), \
        InstructionHandler(&instr_WINDOW_MOUSE_BUTTON_DOWN2, REG, ONE_DATA)) \
    X(WINDOW_MOUSE_BUTTON_RELEASED, true, \
        InstructionHandler(&instr_WINDOW_MOUSE_BUTTON_RELEASED, REG, REG_BOTH), \
        InstructionHandler(&instr_WINDOW_MOUSE_BUTTON_RELEASED2, REG, ONE_DATA)) \
    X(WINDOW_MOUSE_WHEEL_DELTA, true, \
        InstructionHandler(&instr_WINDOW_MOUSE_WHEEL_DELTA, REG)) \
    X(WINDOW_HIDE_CURSOR, true, \
        InstructionHandler(&instr_WINDOW_HIDE_CURSOR)) \
    X(WINDOW_WINDOW_SHOW_CURSOR, true, \
        InstructionHandler(&instr_WINDOW_SHOW_CURSOR)) \
    X(WINDOW_DISABLE_CURSOR, true, \
        InstructionHandler(&instr_WINDOW_DISABLE_CURSOR)) \
    X(WINDOW_ENABLE_CURSOR, true, \
        InstructionHandler(&instr_WINDOW_ENABLE_CURSOR)) \
    X(WINDOW_TEXTURE_FRAMEBUFFER_CREATE, true, \
        InstructionHandler(&instr_WINDOW_TEXTURE_FRAMEBUFFER_CREATE, REG, REG_BOTH, REG_BOTH)) \
    X(WINDOW_TEXTURE_FRAMEBUFFER_ADDRESS, true, \
        InstructionHandler(&instr_WINDOW_TEXTURE_FRAMEBUFFER_ADDRESS, REG, REG_BOTH)) \
    X(WINDOW_TEXTURE_FRAMEBUFFER_SYNC, true, \
        InstructionHandler(&instr_WINDOW_TEXTURE_FRAMEBUFFER_SYNC, REG_BOTH)) \
    X(WINDOW_TEXTURE_FRAMEBUFFER_DRAW, true, \
        InstructionHandler(&instr_WINDOW_TEXTURE_FRAMEBUFFER_DRAW, REG_BOTH, REG_BOTH, REG_BOTH)) \
    X(FILE_OPEN, true, \
        InstructionHandler(&instr_FILE_OPEN, REG, ONE_DATA), \
        InstructionHandler(&instr_FILE_OPEN2, REG, REG_BOTH)) \
    X(FILE_CREATE, true, \
        InstructionHandler(&instr_FILE_CREATE, REG, ONE_DATA), \
        InstructionHandler(&instr_FILE_CREATE2, REG, REG_BOTH)) \
    X(FILE_SAVE, true, InstructionHandler(&instr_FILE_SAVE, REG)) \
    X(FILE_DELETE, true, InstructionHandler(&instr_FILE_DELETE, REG)) \
    X(FILE_CLOSE, true, InstructionHandler(&instr_FILE_CLOSE, REG)) \
    X(FILE_GET_SIZE, true, InstructionHandler(&instr_FILE_GET_SIZE, REG, REG_BOTH)) \
    X(FILE_MAP, true, InstructionHandler(&instr_FILE_MAP, REG, REG_BOTH)) \
    X(FILE_MAP_FROM_CURSOR, true, InstructionHandler(&instr_FILE_MAP_FROM_CURSOR, REG, REG_BOTH, REG_BOTH)) \
    X(FILE_RESET_CURSOR, true, InstructionHandler(&instr_FILE_RESET_CURSOR, REG)) \
    X(FILE_SEEK_CURSOR, true, InstructionHandler(&instr_FILE_SEEK_CURSOR, REG, REG)) \
    X(FILE_GET_CURSOR, true, InstructionHandler(&instr_FILE_GET_CURSOR, REG, REG)) \
    X(FILE_CLEAR_DATA, true, InstructionHandler(&instr_FILE_CLEAR_DATA, REG)) \
    X(FILE_READ_BYTE, true, InstructionHandler(&instr_FILE_READ_BYTE, REG, REG)) \
    X(FILE_READ_BYTE_AT, true, \
        InstructionHandler(&instr_FILE_READ_BYTE_ATR, REG, REG_BOTH, REG_BOTH), \
        InstructionHandler(&instr_FILE_READ_BYTE_ATD, REG, REG_BOTH, ONE_DATA)) \
    X(FILE_READ_WORD, true, InstructionHandler(&instr_FILE_READ_WORD, REG, REG)) \
    X(FILE_READ_WORD_AT, true, \
        InstructionHandler(&instr_FILE_READ_WORD_ATR, REG, REG_BOTH, REG_BOTH), \
        InstructionHandler(&instr_FILE_READ_WORD_ATD, REG, REG_BOTH, ONE_DATA)) \
    X(FILE_READ_WORD_LITTLE, true, InstructionHandler(&instr_FILE_READ_WORD_LITTLE, REG, REG)) \
    X(FILE_READ_WORD_LITTLE_AT, true, \
        InstructionHandler(&instr_FILE_READ_WORD_LITTLE_ATR, REG, REG_BOTH, REG_BOTH), \
        InstructionHandler(&instr_FILE_READ_WORD_LITTLE_ATD, REG, REG_BOTH, ONE_DATA)) \
    X(FILE_READ_DOUBLEWORD, true, InstructionHandler(&instr_FILE_READ_DOUBLEWORD, REG, REG)) \
    X(FILE_READ_DOUBLEWORD_AT, true, \
        InstructionHandler(&instr_FILE_READ_DOUBLEWORD_ATR, REG, REG_BOTH, REG_BOTH), \
        InstructionHandler(&instr_FILE_READ_DOUBLEWORD_ATD, REG, REG_BOTH, ONE_DATA)) \
    X(FILE_READ_DOUBLEWORD_LITTLE, true, InstructionHandler(&instr_FILE_READ_DOUBLEWORD_LITTLE, REG, REG)) \
    X(FILE_READ_DOUBLEWORD_LITTLE_AT, true, \
        InstructionHandler(&instr_FILE_READ_DOUBLEWORD_LITTLE_ATR, REG, REG_BOTH, REG_BOTH), \
        InstructionHandler(&instr_FILE_READ_DOUBLEWORD_LITTLE_ATD, REG, REG_BOTH, ONE_DATA)) \
    X(FILE_APPEND_BYTE, true, InstructionHandler(&instr_FILE_APPEND_BYTE, REG, REG)) \
    X(FILE_APPEND_WORD, true, InstructionHandler(&instr_FILE_APPEND_WORD, REG, REG)) \
    X(FILE_APPEND_WORD_LITTLE, true, InstructionHandler(&instr_FILE_APPEND_WORD_LITTLE, REG, REG)) \
    X(FILE_APPEND_DOUBLEWORD, true, InstructionHandler(&instr_FILE_APPEND_DOUBLEWORD, REG, REG)) \
    X(FILE_APPEND_DOUBLEWORD_LITTLE, true, InstructionHandler(&instr_FILE_APPEND_DOUBLEWORD_LITTLE, REG, REG)) \
    X(FILE_WRITE_BYTE, true, InstructionHandler(&instr_FILE_WRITE_BYTE, REG, REG)) \
    X(FILE_WRITE_WORD, true, InstructionHandler(&instr_FILE_WRITE_WORD, REG, REG)) \
    X(FILE_WRITE_WORD_LITTLE, true, InstructionHandler(&instr_FILE_WRITE_WORD_LITTLE, REG, REG)) \
    X(FILE_WRITE_DOUBLEWORD, true, InstructionHandler(&instr_FILE_WRITE_DOUBLEWORD, REG, REG)) \
    X(FILE_WRITE_DOUBLEWORD_LITTLE, true, InstructionHandler(&instr_FILE_WRITE_DOUBLEWORD_LITTLE, REG, REG)) \
    X(FILE_IS_BYTE_REMAINING, true, InstructionHandler(&instr_FILE_IS_BYTE_REMAINING, REG, REG)) \
    X(FILE_IS_WORD_REMAINING, true, InstructionHandler(&instr_FILE_IS_WORD_REMAINING, REG, REG)) \
    X(FILE_IS_DOUBLEWORD_REMAINING, true, InstructionHandler(&instr_FILE_IS_DOUBLEWORD_REMAINING, REG, REG)) \
    X(CLOCK_CREATE, true, InstructionHandler(&instr_CLOCK_CREATE, REG)) \
    X(CLOCK_DELETE, true, InstructionHandler(&instr_CLOCK_DELETE, REG)) \
    X(CLOCK_GET_ELAPSED_MS, true, InstructionHandler(&instr_CLOCK_GET_ELAPSED_MS, REG, REG)) \
    X(CLOCK_GET_ELAPSED_S, true, InstructionHandler(&instr_CLOCK_GET_ELAPSED_S, REG, REG)) \
    X(CLOCK_RESET, true, InstructionHandler(&instr_CLOCK_RESET, REG))

constexpr std::array rawInstructionSet = {
    #define MAKE_RAW(name, usable, ...) RawInstruction{#name, usable, __VA_ARGS__},
        INSTRUCTION_LIST(MAKE_RAW)
    #undef MAKE_RAW
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
