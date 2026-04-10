//
// Created by Roumite on 21/02/2026.
//

#include <iostream>
#include <numbers>
#include <random>
#include <cmath>
#include <bit>

#include "Logger.h"
#include "interpreter/exceptions.h"
#include "interpreter/runtime/Runtime.h"

#include "interpreter/runtime/InstructionViewImpl.h"
#include "interpreter/runtime/TimeDebugger.h"

// ASM Instructions

void instr_EOF(Runtime& runtime, InstructionView view)
{
    std::cout << "Reach EOF" << std::endl;
    runtime.return_value = 0;
    runtime.is_running = false;
}

void instr_LOADD_32(Runtime &runtine, InstructionView view)
{
    runtine.registries.write(view.r1(), view.get_data(runtine));
}

void instr_LOADR_32(Runtime &runtine, InstructionView view)
{
    runtine.registries.write(
        view.r1(),
        view.get_r2(runtine)
    );
}

void instr_LOADD_16(Runtime &runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_data(runtime, 2) & 0xFFFF
    );
}

void instr_LOADR_16(Runtime &runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r2(runtime, 2) & 0xFFFF
    );
}

void instr_LOADD_8(Runtime &runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_data(runtime, 1) & 0xFF
    );
}

void instr_LOADR_8(Runtime &runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r2(runtime, 1) & 0xFF
    );
}

void instr_STORED_32(Runtime &runtime, InstructionView view)
{
    runtime.memory.write_uint32(
        view.get_data(runtime),
        view.get_r1(runtime) & 0xFFFFFFFF
    );
}

void instr_STORER_32(Runtime &runtime, InstructionView view)
{
    runtime.memory.write_uint32(
        view.get_r2(runtime),
        view.get_r1(runtime) & 0xFFFFFFFF
    );
}

void instr_STORED_16(Runtime &runtime, InstructionView view)
{
    runtime.memory.write_uint16(
        view.get_data(runtime),
        view.get_r1(runtime, 2) & 0xFFFF
    );
}

void instr_STORER_16(Runtime &runtime, InstructionView view)
{
    runtime.memory.write_uint16(
        view.get_r2(runtime),
        view.get_r1(runtime, 2) & 0xFFFF
    );
}

void instr_STORED_8(Runtime &runtime, InstructionView view)
{
    runtime.memory.write_uint8(
        view.get_data(runtime),
        view.get_r1(runtime, 1) & 0xFF
    );
}

void instr_STORER_8(Runtime &runtime, InstructionView view)
{
    runtime.memory.write_uint8(
        view.get_r2(runtime),
        view.get_r1(runtime, 1) & 0xFF
    );
}

void instr_EXTEND_8(Runtime& runtime, InstructionView view)
{
    const auto val8 = static_cast<int8_t>(view.get_r2(runtime));
    runtime.registries.write(view.r1(), static_cast<uint32_t>(val8));
}

void instr_EXTEND_16(Runtime& runtime, InstructionView view)
{
    const auto val16 = static_cast<int16_t>(view.get_r2(runtime));
    runtime.registries.write(view.r1(), static_cast<uint32_t>(val16));
}

void instr_TRUNC_8(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(view.r1(), view.get_r2(runtime) & 0xFF);
}

void instr_TRUNC_16(Runtime &runtime, InstructionView view)
{
    runtime.registries.write(view.r1(), view.get_r2(runtime) & 0xFFFF);
}
void instr_ADDR(Runtime &runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r1(runtime) + view.get_r2(runtime)
    );
}

void instr_ADDD(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r1(runtime) + view.get_data(runtime)
    );
}

void instr_FADDR(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::bit_cast<float>(view.get_r1(runtime)) + std::bit_cast<float>(view.get_r2(runtime))
        )
    );
}

void instr_FADDD(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::bit_cast<float>(view.get_r1(runtime)) + std::bit_cast<float>(view.get_data(runtime))
        )
    );
}


void instr_SUBR(Runtime &runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r1(runtime) - view.get_r2(runtime)
    );
}

void instr_SUBD(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r1(runtime) - view.get_data(runtime)
    );
}

void instr_FSUBR(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::bit_cast<float>(view.get_r1(runtime)) - std::bit_cast<float>(view.get_r2(runtime))
        )
    );
}

void instr_FSUBD(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::bit_cast<float>(view.get_r1(runtime)) - std::bit_cast<float>(view.get_data(runtime))
        )
    );
}

void instr_MULR(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r1(runtime) * view.get_r2(runtime)
    );
}

void instr_MULD(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r1(runtime) * view.get_data(runtime)
    );
}

void instr_FTANR(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::tan(std::bit_cast<float>(view.get_r2(runtime)))
        )
    );
}

void instr_FTAND(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::tan(std::bit_cast<float>(view.get_data(runtime)))
        )
    );
}

void instr_FATANR(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::atan(std::bit_cast<float>(view.get_r2(runtime)))
        )
    );
}

void instr_FATAND(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::atan(std::bit_cast<float>(view.get_data(runtime)))
        )
    );
}

void instr_SMULR(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::bit_cast<int32_t>(view.get_r1(runtime)) * std::bit_cast<int32_t>(view.get_r2(runtime))
        )
    );
}

void instr_SMULD(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::bit_cast<int32_t>(view.get_r1(runtime)) * std::bit_cast<int32_t>(view.get_data(runtime))
        )
    );
}

void instr_FMULR(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::bit_cast<float>(view.get_r1(runtime)) *std::bit_cast<float>( view.get_r2(runtime))
        )
    );
}

void instr_FMULD(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::bit_cast<float>(view.get_r1(runtime)) * std::bit_cast<float>(view.get_data(runtime))
        )
    );
}

void instr_DIVR(Runtime& runtime, InstructionView view)
{
    if (view.get_r2(runtime) == 0)
        throw InterpreterError("[DIV] Division by zero !");
    runtime.registries.write(
        view.r1(),
        view.get_r1(runtime) / view.get_r2(runtime)
    );
}

void instr_DIVD(Runtime& runtime, InstructionView view)
{
    if (view.get_data(runtime) == 0)
        throw InterpreterError("[DIV] Division by zero !");
    runtime.registries.write(
        view.r1(),
        view.get_r1(runtime) / view.get_data(runtime)
    );
}

void instr_SDIVR(Runtime& runtime, InstructionView view)
{
    if (std::bit_cast<int32_t>(view.get_r2(runtime)) == 0)
        throw InterpreterError("[DIV] Division by zero !");
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::bit_cast<int32_t>(view.get_r1(runtime)) / std::bit_cast<int32_t>(view.get_r2(runtime))
        )
    );
}

void instr_SDIVD(Runtime& runtime, InstructionView view)
{
    if (std::bit_cast<int32_t>(view.get_data(runtime)) == 0)
        throw InterpreterError("[DIV] Division by zero !");
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::bit_cast<int32_t>(view.get_r1(runtime)) / std::bit_cast<int32_t>(view.get_data(runtime))
        )
    );
}

void instr_FDIVR(Runtime& runtime, InstructionView view)
{
    if (std::bit_cast<float>(view.get_r2(runtime)) == 0)
        throw InterpreterError("[DIV] Division by zero !");
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::bit_cast<float>(view.get_r1(runtime)) / std::bit_cast<float>(view.get_r2(runtime))
        )
    );
}

void instr_FDIVD(Runtime& runtime, InstructionView view)
{
    if (std::bit_cast<float>(view.get_data(runtime)) == 0)
        throw InterpreterError("[DIV] Division by zero !");
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::bit_cast<float>(view.get_r1(runtime)) / std::bit_cast<float>(view.get_data(runtime))
        )
    );
}

void instr_NOT(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        !view.get_r2(runtime)
    );
}

void instr_FNOT(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>((std::bit_cast<float>(view.get_r2(runtime)) == 0.0f) ? 1.0f : 0.0f)
    );
}

void instr_MINR(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::min(
            view.get_r1(runtime),
            view.get_r2(runtime)
        )
    );
}

void instr_MIND(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::min(
            view.get_r1(runtime),
            view.get_data(runtime)
        )
    );
}

void instr_FMINR(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::fmin(
                std::bit_cast<float>(view.get_r1(runtime)),
                std::bit_cast<float>(view.get_r2(runtime))
            )
        )
    );
}

void instr_FMIND(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::fmin(
                std::bit_cast<float>(view.get_r1(runtime)),
                std::bit_cast<float>(view.get_data(runtime))
            )
        )
    );
}

void instr_MAXR(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::max(
            view.get_r1(runtime),
            view.get_r2(runtime)
        )
    );
}

void instr_MAXD(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::max(
            view.get_r1(runtime),
            view.get_data(runtime)
        )
    );
}

void instr_FMAXR(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::fmax(
                std::bit_cast<float>(view.get_r1(runtime)),
                std::bit_cast<float>(view.get_r2(runtime))
            )
        )
    );
}

void instr_FMAXD(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::fmax(
                std::bit_cast<float>(view.get_r1(runtime)),
                std::bit_cast<float>(view.get_data(runtime))
            )
        )
    );
}

void instr_MODR(Runtime& runtime, InstructionView view)
{
    if (view.get_r2(runtime) == 0)
        throw InterpreterError("[MOD] Modulo by zero !");
    runtime.registries.write(
        view.r1(),
        view.get_r1(runtime) % view.get_r2(runtime)
    );
}

void instr_MODD(Runtime& runtime, InstructionView view)
{
    if (view.get_data(runtime) == 0)
        throw InterpreterError("[MOD] Modulo by zero !");
    runtime.registries.write(
        view.r1(),
        view.get_r1(runtime) % view.get_data(runtime)
    );
}

void instr_BIT_ANDR2(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r1(runtime) & view.get_r2(runtime)
    );
}

void instr_BIT_ANDD2(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r1(runtime) & view.get_data(runtime)
    );
}

void instr_BIT_ANDR3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r2(runtime) & view.get_r3(runtime)
    );
}

void instr_BIT_ANDD3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r2(runtime) & view.get_data(runtime)
    );
}

void instr_BIT_ORR2(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r1(runtime) | view.get_r2(runtime)
    );
}

void instr_BIT_ORD2(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r1(runtime) | view.get_data(runtime)
    );
}

void instr_BIT_ORR3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r2(runtime) | view.get_r3(runtime)
    );
}

void instr_BIT_ORD3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r2(runtime) | view.get_data(runtime)
    );
}

void instr_BIT_XORR2(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r1(runtime) ^ view.get_r2(runtime)
    );
}

void instr_BIT_XORD2(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r1(runtime) ^ view.get_data(runtime)
    );
}

void instr_BIT_XORR3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r2(runtime) ^ view.get_r3(runtime)
    );
}

void instr_BIT_XORD3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r2(runtime) ^ view.get_data(runtime)
    );
}

void instr_BIT_NOTR(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        ~view.get_r2(runtime)
    );
}

void instr_BIT_NOTD(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        ~view.get_data(runtime)
    );
}

void instr_BIT_SHIFT_LEFTR2(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r1(runtime) << view.get_r2(runtime)
    );
}

void instr_BIT_SHIFT_LEFTD2(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r1(runtime) << view.get_data(runtime)
    );
}

void instr_BIT_SHIFT_LEFTR3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r2(runtime) << view.get_r3(runtime)
    );
}

void instr_BIT_SHIFT_LEFTD3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r2(runtime) << view.get_data(runtime)
    );
}

void instr_BIT_SHIFT_RIGHTR2(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r1(runtime) >> view.get_r2(runtime)
    );
}

void instr_BIT_SHIFT_RIGHTD2(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r1(runtime) >> view.get_data(runtime)
    );
}

void instr_BIT_SHIFT_RIGHTR3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r2(runtime) >> view.get_r3(runtime)
    );
}

void instr_BIT_SHIFT_RIGHTD3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r2(runtime) >> view.get_data(runtime)
    );
}

void instr_BIT_S_SHIFT_RIGHTR2(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::bit_cast<int>(view.get_r1(runtime)) >> view.get_r2(runtime)
        )
    );
}

void instr_BIT_S_SHIFT_RIGHTD2(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::bit_cast<int>(view.get_r1(runtime)) >> view.get_data(runtime)
        )
    );
}

void instr_BIT_S_SHIFT_RIGHTR3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::bit_cast<int>(view.get_r2(runtime)) >> view.get_r3(runtime)
        )
    );
}

void instr_BIT_S_SHIFT_RIGHTD3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::bit_cast<int>(view.get_r2(runtime)) >> view.get_data(runtime)
        )
    );
}

void instr_FMODR(Runtime& runtime, InstructionView view)
{
    if (std::bit_cast<float>(view.get_r2(runtime)) == 0)
        throw InterpreterError("[MOD] Modulo by zero !");
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::fmod(std::bit_cast<float>(view.get_r1(runtime)), std::bit_cast<float>(view.get_r2(runtime)))
        )
    );
}

void instr_FMODD(Runtime& runtime, InstructionView view)
{
    if (std::bit_cast<float>(view.get_data(runtime)) == 0)
        throw InterpreterError("[MOD] Modulo by zero !");
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::fmod(std::bit_cast<float>(view.get_r1(runtime)), std::bit_cast<float>(view.get_data(runtime)))
        )
    );
}

void instr_FSINR(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::sin(std::bit_cast<float>(view.get_r2(runtime)))
        )
    );
}

void instr_FSIND(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::sin(std::bit_cast<float>(view.get_data(runtime)))
        )
    );
}

void instr_FCOSR(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::cos(std::bit_cast<float>(view.get_r2(runtime)))
        )
    );
}

void instr_FCOSD(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::cos(std::bit_cast<float>(view.get_data(runtime)))
        )
    );
}

void instr_FSQRTR(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::sqrt(std::bit_cast<float>(view.get_r2(runtime)))
        )
    );
}

void instr_FSQRTD(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::sqrt(std::bit_cast<float>(view.get_data(runtime)))
        )
    );
}

void instr_FABSR(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::fabs(std::bit_cast<float>(view.get_r2(runtime)))
        )
    );
}

void instr_FABSD(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::fabs(std::bit_cast<float>(view.get_data(runtime)))
        )
    );
}

inline float degToRad(float degrees) {
    return degrees * (std::numbers::pi / 180.0);
}


void instr_FDEG2RADR(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            degToRad(std::bit_cast<float>(view.get_r2(runtime)))
        )
    );
}

void instr_FDEG2RADD(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            degToRad(std::bit_cast<float>(view.get_data(runtime)))
        )
    );
}

float radToDeg(float radians) {
    return radians * (180.0 / std::numbers::pi);
}

void instr_FRAD2DEGR(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            radToDeg(std::bit_cast<float>(view.get_r2(runtime)))
        )
    );
}

void instr_FRAD2DEGD(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            radToDeg(std::bit_cast<float>(view.get_data(runtime)))
        )
    );
}

/* Three operand opérations */

void instr_ADDR3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r2(runtime) + view.get_r3(runtime)
    );
}

void instr_ADDD3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r2(runtime) + view.get_data(runtime)
    );
}

void instr_FADDR3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
                std::bit_cast<float>(view.get_r2(runtime)) + std::bit_cast<float>(view.get_r3(runtime))
        )
    );
}

void instr_FADDD3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
                std::bit_cast<float>(view.get_r2(runtime)) + std::bit_cast<float>(view.get_data(runtime))
        )
    );
}

void instr_SUBR3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r2(runtime) - view.get_r3(runtime)
    );
}

void instr_SUBD3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r2(runtime) - view.get_data(runtime)
    );
}

void instr_FSUBR3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
                std::bit_cast<float>(view.get_r2(runtime)) - std::bit_cast<float>(view.get_r3(runtime))
        )
    );
}

void instr_FSUBD3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::bit_cast<float>(view.get_r2(runtime)) - std::bit_cast<float>(view.get_data(runtime))
        )
    );
}

void instr_MULR3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r2(runtime) * view.get_r3(runtime)
    );
}

void instr_MULD3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r2(runtime) * view.get_data(runtime)
    );
}

void instr_SMULR3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::bit_cast<int32_t>(view.get_r2(runtime)) * std::bit_cast<int32_t>(view.get_r3(runtime))
        )
    );
}

void instr_SMULD3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::bit_cast<int32_t>(view.get_r2(runtime)) * std::bit_cast<int32_t>(view.get_data(runtime))
        )
    );
}

void instr_FMULR3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
                std::bit_cast<float>(view.get_r2(runtime)) * std::bit_cast<float>(view.get_r3(runtime))
        )
    );
}

void instr_FMULD3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
                std::bit_cast<float>(view.get_r2(runtime)) * std::bit_cast<float>(view.get_data(runtime))
        )
    );
}

void instr_DIVR3(Runtime& runtime, InstructionView view)
{
    if (view.get_r3(runtime) == 0)
        throw InterpreterError("[DIV] Division by zero !");
    runtime.registries.write(
        view.r1(),
        view.get_r2(runtime) / view.get_r3(runtime)
    );
}

void instr_DIVD3(Runtime& runtime, InstructionView view)
{
    if (view.get_data(runtime) == 0)
        throw InterpreterError("[DIV] Division by zero !");
    runtime.registries.write(
        view.r1(),
        view.get_r2(runtime) / view.get_data(runtime)
    );
}

void instr_SDIVR3(Runtime& runtime, InstructionView view)
{
    if (view.get_r3(runtime) == 0)
        throw InterpreterError("[DIV] Division by zero !");
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::bit_cast<int32_t>(view.get_r2(runtime)) / std::bit_cast<int32_t>(view.get_r3(runtime))
        )
    );
}

void instr_SDIVD3(Runtime& runtime, InstructionView view)
{
    if (view.get_data(runtime) == 0)
        throw InterpreterError("[DIV] Division by zero !");
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::bit_cast<int32_t>(view.get_r2(runtime)) / std::bit_cast<int32_t>(view.get_data(runtime))
        )
    );
}

void instr_FDIVR3(Runtime& runtime, InstructionView view)
{
    if (std::bit_cast<float>(view.get_r3(runtime)) == 0)
        throw InterpreterError("[DIV] Division by zero !");
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
                std::bit_cast<float>(view.get_r2(runtime)) / std::bit_cast<float>(view.get_r3(runtime))
        )
    );
}

void instr_FDIVD3(Runtime& runtime, InstructionView view)
{
    if (std::bit_cast<float>(view.get_data(runtime)) == 0)
        throw InterpreterError("[DIV] Division by zero !");
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::bit_cast<float>(view.get_r2(runtime)) / std::bit_cast<float>(view.get_data(runtime))
        )
    );
}

void instr_MINR3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::min(view.get_r2(runtime), view.get_r3(runtime))
    );
}

void instr_MIND3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::min(view.get_r2(runtime), view.get_data(runtime))
    );
}

void instr_FMINR3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::fmin(std::bit_cast<float>(view.get_r2(runtime)), std::bit_cast<float>(view.get_r3(runtime)))
        )
    );
}

void instr_FMIND3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::fmin(std::bit_cast<float>(view.get_r2(runtime)), std::bit_cast<float>(view.get_data(runtime)))
        )
    );
}

void instr_MAXR3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::max(view.get_r2(runtime), view.get_r3(runtime))
    );
}

void instr_MAXD3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::max(view.get_r2(runtime), view.get_data(runtime))
    );
}

void instr_FMAXR3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::fmax(std::bit_cast<float>(view.get_r2(runtime)), std::bit_cast<float>(view.get_r3(runtime)))
        )
    );
}

void instr_FMAXD3(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::fmax(std::bit_cast<float>(view.get_r2(runtime)), std::bit_cast<float>(view.get_data(runtime)))
        )
    );
}

void instr_MODR3(Runtime& runtime, InstructionView view)
{
    if (view.get_r3(runtime) == 0)
        throw InterpreterError("[MOD] Modulo by zero !");
    runtime.registries.write(
        view.r1(),
        view.get_r2(runtime) % view.get_r3(runtime)
    );
}

void instr_MODD3(Runtime& runtime, InstructionView view)
{
    if (view.get_data(runtime) == 0)
        throw InterpreterError("[MOD] Modulo by zero !");
    runtime.registries.write(
        view.r1(),
        view.get_r2(runtime) % view.get_data(runtime)
    );
}

void instr_FMODR3(Runtime& runtime, InstructionView view)
{
    if (std::bit_cast<float>(view.get_r3(runtime)) == 0)
        throw InterpreterError("[MOD] Modulo by zero !");
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::fmod(std::bit_cast<float>(view.get_r2(runtime)), std::bit_cast<float>(view.get_r3(runtime)))
        )
    );
}

void instr_FMODD3(Runtime& runtime, InstructionView view)
{
    if (std::bit_cast<float>(view.get_data(runtime)) == 0)
        throw InterpreterError("[MOD] Modulo by zero !");
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            std::fmod(std::bit_cast<float>(view.get_r2(runtime)), std::bit_cast<float>(view.get_data(runtime)))
        )
    );
}

void instr_JMP(Runtime &runtime, InstructionView view)
{
    *runtime.curr_instruction = runtime.instructions_beg + view.get_data(runtime);
}

void compute_CMP(Runtime &runtime, uint32_t first_value, uint32_t second_value)
{
    runtime.comparison_flag.equal = first_value == second_value;
    runtime.comparison_flag.greater = first_value > second_value;
    runtime.comparison_flag.lower = first_value < second_value;

    runtime.comparison_flag.signed_greater = static_cast<int32_t>(first_value) > static_cast<int32_t>(second_value);
    runtime.comparison_flag.signed_lower = static_cast<int32_t>(first_value) < static_cast<int32_t>(second_value);
}

void compute_FCMP(Runtime &runtime, float first_value, float second_value)
{
    runtime.comparison_flag.equal = false;
    runtime.comparison_flag.greater = false;
    runtime.comparison_flag.lower = false;

    if (!std::isnan(first_value) && !std::isnan(second_value)) {
        runtime.comparison_flag.equal = (first_value == second_value);
        runtime.comparison_flag.greater = (first_value > second_value);
        runtime.comparison_flag.lower = (first_value < second_value);
    }

    runtime.comparison_flag.signed_greater = runtime.comparison_flag.greater;
    runtime.comparison_flag.signed_lower = runtime.comparison_flag.lower;
}

void instr_CMPR(Runtime &runtime, InstructionView view)
{
    compute_CMP(
        runtime,
        view.get_r1(runtime),
        view.get_r2(runtime)
    );
}

void instr_CMPD(Runtime &runtime, InstructionView view)
{
    compute_CMP(
        runtime,
        view.get_r1(runtime),
        view.get_data(runtime)
    );
}

void instr_FCMPR(Runtime& runtime, InstructionView view)
{
    compute_FCMP(
        runtime,
        std::bit_cast<float>(view.get_r1(runtime)),
        std::bit_cast<float>(view.get_r2(runtime))
    );
}

void instr_FCMPD(Runtime& runtime, InstructionView view)
{
    compute_FCMP(
        runtime,
        std::bit_cast<float>(view.get_r1(runtime)),
        std::bit_cast<float>(view.get_data(runtime))
    );
}

void instr_JE(Runtime& runtime, InstructionView view)
{
    if (runtime.comparison_flag.equal)
        *runtime.curr_instruction = runtime.instructions_beg + view.get_data(runtime);
}

void instr_JNE(Runtime& runtime, InstructionView view)
{
    if (not runtime.comparison_flag.equal)
        *runtime.curr_instruction = runtime.instructions_beg + view.get_data(runtime);
}

void instr_JA(Runtime& runtime, InstructionView view)
{
    if (runtime.comparison_flag.greater)
        *runtime.curr_instruction = runtime.instructions_beg + view.get_data(runtime);
}

void instr_JSA(Runtime& runtime, InstructionView view)
{
    if (runtime.comparison_flag.signed_greater)
        *runtime.curr_instruction = runtime.instructions_beg + view.get_data(runtime);
}

void instr_JB(Runtime& runtime, InstructionView view)
{
    if (runtime.comparison_flag.lower)
        *runtime.curr_instruction = runtime.instructions_beg + view.get_data(runtime);
}

void instr_JSB(Runtime& runtime, InstructionView view)
{
    if (runtime.comparison_flag.signed_lower)
        *runtime.curr_instruction = runtime.instructions_beg + view.get_data(runtime);
}

void instr_DISPLAY_N(Runtime &runtime, InstructionView view)
{
    std::cout << view.get_r1(runtime) << std::endl;
}

void instr_DISPLAY_SN(Runtime& runtime, InstructionView view)
{
    std::cout << std::bit_cast<int>(view.get_r1(runtime)) << std::endl;
}

void instr_DISPLAY_C(Runtime &runtime, InstructionView view)
{
    std::cout << static_cast<char>(view.get_r1(runtime)) << std::flush;
}

void instr_DISPLAY_B(Runtime &runtime, InstructionView view)
{
    Logger::log_uint32_as_bit(view.get_r1(runtime));
}

void instr_DISPLAY_F(Runtime& runtime, InstructionView view)
{
    std::cout << std::bit_cast<float>(view.get_r1(runtime)) << std::endl;
}

void instr_DISPLAY_STR(Runtime& runtime, InstructionView view)
{
    std::cout << runtime.utils.get_string_from_address(view.get_r1(runtime)) << std::endl;
}

void instr_HALT(Runtime &runtime, InstructionView view)
{
    runtime.return_value = runtime.registries.read(R0);
    runtime.is_running = false;
}

void instr_HALT1(Runtime& runtime, InstructionView view)
{
    runtime.return_value = view.get_r1(runtime);
    runtime.is_running = false;
}

void instr_ALOCA(Runtime &runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.memory.allocate(view.get_data(runtime))
    );
}

void instr_ALOCR(Runtime &runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.memory.allocate(view.get_r2(runtime))
    );
}

void instr_FREE(Runtime &runtime, InstructionView view)
{
    runtime.memory.free(
        view.get_r1(runtime)
    );
}

void instr_DEBUG_R(Runtime& runtime, InstructionView view)
{
    Logger::log("DEBUG_REGISTRIES", "instr_DEBUG_REGISTRIES");
    runtime.registries.display();
}

void instr_DEBUG_M(Runtime& runtime, InstructionView view)
{
    Logger::log("DEBUG_MEMORY", "instr_DEBUG_MEMORY");
    runtime.memory.display();
}



void instr_DEBUG_TR(Runtime& runtime, InstructionView view)
{
    const uint32_t clock_id = view.get_r1(runtime);
    TimeDebugger::process_debug_t(clock_id);
}

void instr_DEBUG_TD(Runtime& runtime, InstructionView view)
{
    const uint32_t clock_id = view.get_data(runtime);
    TimeDebugger::process_debug_t(clock_id);
}

void instr_CALL(Runtime& runtime, InstructionView view)
{
    runtime.stack.push(*runtime.curr_instruction - runtime.instructions_beg);
    *runtime.curr_instruction = runtime.instructions_beg + view.get_data(runtime);
}

void instr_RET(Runtime& runtime, InstructionView view)
{
    *runtime.curr_instruction = runtime.instructions_beg + runtime.stack.pop();
}

void instr_PUSH(Runtime& runtime, InstructionView view)
{
    runtime.stack.push(view.get_r1(runtime));
}

void instr_POP(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(view.r1(), runtime.stack.pop());
}

void instr_RAND(Runtime& runtime, InstructionView view)
{
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);

    runtime.registries.write(
        view.r1(),
        dist(rng)
    );
}

void instr_ITOF(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            static_cast<float>(std::bit_cast<int>(view.get_r2(runtime)))
        )
    );
}

void instr_UTOF(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(
            static_cast<float>(view.get_r2(runtime))
        )
    );
}

void instr_FTOI(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        static_cast<int32_t>(
            std::bit_cast<float>(view.get_r2(runtime))
        )
    );
}

void instr_FTOU(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        static_cast<uint32_t>(
            std::bit_cast<float>(view.get_r2(runtime))
        )
    );
}

void instr_MAP_IMAGER(Runtime& runtime, InstructionView view)
{
    const auto path = runtime.files.get_file(view.get_r2(runtime))->get_path();
    runtime.registries.write(
        view.r1(),
        runtime.utils.load_image_and_map(path)
    );
}

void instr_MAP_IMAGED(Runtime& runtime, InstructionView view)
{
    const auto path = runtime.files.get_file(view.get_data(runtime))->get_path();
    runtime.registries.write(
        view.r1(),
        runtime.utils.load_image_and_map(path)
    );
}

void instr_MAP_IMAGE_X(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.utils.get_last_image_width()
    );
}

void instr_MAP_IMAGE_SIZE_Y(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.utils.get_last_image_height()
    );
}

void instr_WINDOW_CREATE(Runtime& runtime, InstructionView view)
{
    uint32_t width = view.get_r1(runtime);
    uint32_t height = view.get_r2(runtime);
    std::string name = runtime.utils.get_string_from_address(view.get_data(runtime));

    runtime.graphical_backend.create_window(width, height, name);
}

void instr_WINDOW_CREATE3(Runtime& runtime, InstructionView view)
{
    uint32_t width = view.get_r1(runtime);
    uint32_t height = view.get_r2(runtime);
    std::string name = runtime.utils.get_string_from_address(view.get_r3(runtime));

    runtime.graphical_backend.create_window(width, height, name);
}

void instr_WINDOW_CLOSE(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.close_window();
}

void instr_WINDOW_SET_VIEWPORT_SIZE(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.set_viewport_size(
        view.get_r1(runtime),
        view.get_r2(runtime)
    );
}

void instr_WINDOW_DISABLE_VIEWPORT(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.disable_viewport();
}

void instr_WINDOW_POOL(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.poll_events();
}

void instr_WINDOW_SHOULD_CLOSE(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.graphical_backend.should_close()
    );
}

void instr_WINDOW_CLEAR(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.clear_window({
        static_cast<uint8_t>(view.get_r1(runtime)),
        static_cast<uint8_t>(view.get_r2(runtime)),
        static_cast<uint8_t>(view.get_r3(runtime)),
    });
}

void instr_WINDOW_PRESENT(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.present_window();
}

void instr_WINDOW_TOGGLE_FULLSCREEN(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.toogle_fullscreen();
}

void instr_WINDOW_KEY_PRESSED(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.graphical_backend.key_pressed(view.get_data(runtime))
    );
}

void instr_WINDOW_KEY_PRESSED2(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.graphical_backend.key_pressed(view.get_r2(runtime))
    );
}

void instr_WINDOW_KEY_DOWN(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.graphical_backend.key_down(view.get_data(runtime))
    );
}

void instr_WINDOW_KEY_DOWN2(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.graphical_backend.key_down(view.get_r2(runtime))
    );
}

void instr_WINDOW_SET_TARGET_FPS(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.set_target_fps(
        view.get_data(runtime)
    );
}

void instr_WINDOW_SET_TARGET_FPS1(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.set_target_fps(
        view.get_r1(runtime)
    );
}

void instr_WINDOW_GET_DELTA(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::bit_cast<uint32_t>(runtime.graphical_backend.get_time_delta())
    );
}

void instr_WINDOW_SET_TEXT_SIZE(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.set_text_size(
        view.get_data(runtime)
    );
}

void instr_WINDOW_SET_TEXT_SIZE1(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.set_text_size(
        view.get_r1(runtime)
    );
}

void instr_WINDOW_SET_TEXT_COLOR(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.set_text_color({
        static_cast<uint8_t>(view.get_r1(runtime)),
        static_cast<uint8_t>(view.get_r2(runtime)),
        static_cast<uint8_t>(view.get_r3(runtime)),
        255 // Alpha
    });
}

void instr_WINDOW_SET_FONT(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.set_font(
        runtime.files[view.get_data(runtime)]
    );
}

void instr_WINDOW_SET_FONT1(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.set_font(
        runtime.files[view.get_r1(runtime)]
    );
}

void instr_WINDOW_DRAW_TEXT(Runtime& runtime, InstructionView view)
{
    uint32_t x = view.get_r1(runtime);
    uint32_t y = view.get_r2(runtime);
    std::string text = runtime.utils.get_string_from_address(view.get_data(runtime));

    runtime.graphical_backend.draw_text(text, x, y);
}

void instr_WINDOW_DRAW_TEXT3(Runtime& runtime, InstructionView view)
{
    uint32_t x = view.get_r1(runtime);
    uint32_t y = view.get_r2(runtime);
    std::string text = runtime.utils.get_string_from_address(view.get_r3(runtime));

    runtime.graphical_backend.draw_text(text, x, y);
}

void instr_WINDOW_DRAW_TEXTURE(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.draw_texture(
        runtime.files[view.get_data(runtime)],
        view.get_r1(runtime),
        view.get_r2(runtime)
    );
}

void instr_WINDOW_DRAW_TEXTURE3(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.draw_texture(
        runtime.files[view.get_r3(runtime)],
        view.get_r1(runtime),
        view.get_r2(runtime)
    );
}

void instr_WINDOW_SET_TEXTURE_COLOR_MASK(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.set_texture_color_mask({
        static_cast<uint8_t>(view.get_r1(runtime)),
        static_cast<uint8_t>(view.get_r2(runtime)),
        static_cast<uint8_t>(view.get_r3(runtime)),
        255 // Alpha
    });
}

void instr_WINDOW_RESET_TEXTURE_COLOR_MASK(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.reset_texture_color_mask();
}

void instr_WINDOW_SET_ICON(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.set_window_icon(
        runtime.files[view.get_data(runtime)]
    );
}

void instr_WINDOW_SET_ICON1(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.set_window_icon(
        runtime.files[view.get_r1(runtime)]
    );
}

void instr_WINDOW_MOUSE_X(Runtime &runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.graphical_backend.mouse_x()
    );
}

void instr_WINDOW_MOUSE_Y(Runtime &runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.graphical_backend.mouse_y()
    );
}

void instr_WINDOW_MOUSE_DELTA_X(Runtime &runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.graphical_backend.mouse_delta_x()
    );
}

void instr_WINDOW_MOUSE_DELTA_Y(Runtime &runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.graphical_backend.mouse_delta_y()
    );
}

void instr_WINDOW_MOUSE_BUTTON_PRESSED(Runtime &runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.graphical_backend.mouse_button_pressed(view.get_r2(runtime))
    );
}

void instr_WINDOW_MOUSE_BUTTON_PRESSED2(Runtime &runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.graphical_backend.mouse_button_pressed(view.get_data(runtime))
    );
}

void instr_WINDOW_MOUSE_BUTTON_DOWN(Runtime &runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.graphical_backend.mouse_button_down(view.get_r2(runtime))
    );
}

void instr_WINDOW_MOUSE_BUTTON_DOWN2(Runtime &runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.graphical_backend.mouse_button_down(view.get_data(runtime))
    );
}

void instr_WINDOW_MOUSE_BUTTON_RELEASED(Runtime &runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.graphical_backend.mouse_button_released(view.get_r2(runtime))
    );
}

void instr_WINDOW_MOUSE_BUTTON_RELEASED2(Runtime &runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.graphical_backend.mouse_button_released(view.get_data(runtime))
    );
}

void instr_WINDOW_MOUSE_WHEEL_DELTA(Runtime &runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.graphical_backend.mouse_wheel_delta()
    );
}

void instr_WINDOW_HIDE_CURSOR(Runtime &runtime, InstructionView view)
{
    runtime.graphical_backend.hide_cursor();
}

void instr_WINDOW_SHOW_CURSOR(Runtime &runtime, InstructionView view)
{
    runtime.graphical_backend.show_cursor();
}

void instr_WINDOW_DISABLE_CURSOR(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.disable_cursor();
}

void instr_WINDOW_ENABLE_CURSOR(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.enable_cursor();
}

void instr_WINDOW_TEXTURE_FRAMEBUFFER_CREATE(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.graphical_backend.create_framebuffer(
            runtime,
            view.get_r2(runtime),
            view.get_r3(runtime)
        )
    );
}

void instr_WINDOW_TEXTURE_FRAMEBUFFER_ADDRESS(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.graphical_backend.framebuffer_get_address(
            view.get_r2(runtime)
        )
    );
}

void instr_WINDOW_TEXTURE_FRAMEBUFFER_SYNC(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.sync_framebuffer(
        runtime,
        view.get_r1(runtime)
    );
}

void instr_WINDOW_TEXTURE_FRAMEBUFFER_DRAW(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.draw_framebuffer(
        view.get_r1(runtime),
        view.get_r2(runtime),
        view.get_r3(runtime)
    );
}

void instr_FILE_OPEN(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.files.open_file(runtime.utils.get_string_from_address(view.get_data(runtime)))
    );
}


void instr_FILE_OPEN2(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.files.open_file(runtime.utils.get_string_from_address(view.get_r2(runtime)))
    );
}

void instr_FILE_CREATE(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.files.create_file(runtime.utils.get_string_from_address(view.get_data(runtime)))
    );
}

void instr_FILE_CREATE2(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.files.create_file(runtime.utils.get_string_from_address(view.get_r2(runtime)))
    );
}

void instr_FILE_SAVE(Runtime& runtime, InstructionView view)
{
    runtime.files.get_file(view.get_r1(runtime))->save();
}

void instr_FILE_DELETE(Runtime& runtime, InstructionView view)
{
    runtime.files.delete_file(
        view.get_r1(runtime)
    );
}

void instr_FILE_CLOSE(Runtime& runtime, InstructionView view)
{
    runtime.files.close_file(
        view.get_r1(runtime)
    );
}

void instr_FILE_GET_SIZE(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.files.get_file(view.get_r2(runtime))->get_size()
    );
}

void instr_FILE_MAP(Runtime& runtime, InstructionView view)
{
    const auto file = runtime.files.get_file(view.get_r2(runtime));
    const uint32_t size = file->get_size();

    const auto addr = runtime.memory.allocate(file->get_size());
    runtime.registries.write(view.r1(), addr);

    auto &buffer = file->get_file_data().get_buffer();
    auto &block = runtime.memory.get_block(addr);

    std::copy_n(buffer.begin(), size, block.data.begin());
}

void instr_FILE_MAP_FROM_CURSOR(Runtime& runtime, InstructionView view)
{
    const auto file = runtime.files.get_file(view.get_r2(runtime));
    const uint32_t qtt_to_copy = view.get_r3(runtime);

    const uint32_t cursor_pos = file->get_cursor();
    if (cursor_pos + qtt_to_copy > file->get_size())
        throw InterpreterError("FILE_MAP_FROM_CURSOR: Not enough data to copy");


    const auto addr = runtime.memory.allocate(qtt_to_copy);
    runtime.registries.write(view.r1(), addr);

    const auto &buffer = file->get_file_data().get_buffer();
    auto &block = runtime.memory.get_block(addr);

    std::copy_n(buffer.begin() + cursor_pos, qtt_to_copy, block.data.begin());
}

void instr_FILE_RESET_CURSOR(Runtime& runtime, InstructionView view)
{
    runtime.files.get_file(view.get_r1(runtime))->reset_cursor();
}

void instr_FILE_SEEK_CURSOR(Runtime& runtime, InstructionView view)
{
    runtime.files.get_file(view.get_r2(runtime))->seek_cursor(view.get_r1(runtime));
}

void instr_FILE_GET_CURSOR(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.files.get_file(view.get_r2(runtime))->get_cursor()
    );
}

void instr_FILE_CLEAR_DATA(Runtime& runtime, InstructionView view)
{
    runtime.files.get_file(view.get_r1(runtime))->clear_data();
}

void instr_FILE_READ_BYTE(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.files.get_file(view.get_r2(runtime))->read_byte()
    );
}

void instr_FILE_READ_BYTE_ATR(Runtime& runtime, InstructionView view)
{
    const auto file = runtime.files.get_file(view.get_r2(runtime));
    file->seek_cursor(view.get_r3(runtime));
    runtime.registries.write(
        view.r1(),
        file->read_byte()
    );
}

void instr_FILE_READ_BYTE_ATD(Runtime& runtime, InstructionView view)
{
    const auto file = runtime.files.get_file(view.get_r2(runtime));
    file->seek_cursor(view.get_data(runtime));
    runtime.registries.write(
        view.r1(),
        file->read_byte()
    );
}

void instr_FILE_READ_WORD(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.files.get_file(view.get_r2(runtime))->read_word()
    );
}

void instr_FILE_READ_WORD_ATR(Runtime& runtime, InstructionView view)
{
    const auto file = runtime.files.get_file(view.get_r2(runtime));
    file->seek_cursor(view.get_r3(runtime));
    runtime.registries.write(
        view.r1(),
        file->read_word()
    );
}

void instr_FILE_READ_WORD_ATD(Runtime& runtime, InstructionView view)
{
    const auto file = runtime.files.get_file(view.get_r2(runtime));
    file->seek_cursor(view.get_data(runtime));
    runtime.registries.write(
        view.r1(),
        file->read_word()
    );
}

void instr_FILE_READ_WORD_LITTLE(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.files.get_file(view.get_r2(runtime))->read_word_little()
    );
}

void instr_FILE_READ_WORD_LITTLE_ATR(Runtime& runtime, InstructionView view)
{
    const auto file = runtime.files.get_file(view.get_r2(runtime));
    file->seek_cursor(view.get_r3(runtime));
    runtime.registries.write(
        view.r1(),
        file->read_word_little()
    );
}

void instr_FILE_READ_WORD_LITTLE_ATD(Runtime& runtime, InstructionView view)
{
    const auto file = runtime.files.get_file(view.get_r2(runtime));
    file->seek_cursor(view.get_data(runtime));
    runtime.registries.write(
        view.r1(),
        file->read_word_little()
    );
}

void instr_FILE_READ_DOUBLEWORD(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.files.get_file(view.get_r2(runtime))->read_doubleword()
    );
}

void instr_FILE_READ_DOUBLEWORD_ATR(Runtime& runtime, InstructionView view)
{
    const auto file = runtime.files.get_file(view.get_r2(runtime));
    file->seek_cursor(view.get_r3(runtime));
    runtime.registries.write(
        view.r1(),
        file->read_doubleword()
    );
}

void instr_FILE_READ_DOUBLEWORD_ATD(Runtime& runtime, InstructionView view)
{
    const auto file = runtime.files.get_file(view.get_r2(runtime));
    file->seek_cursor(view.get_data(runtime));
    runtime.registries.write(
        view.r1(),
        file->read_doubleword()
    );
}

void instr_FILE_READ_DOUBLEWORD_LITTLE(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.files.get_file(view.get_r2(runtime))->read_doubleword_little()
    );
}

void instr_FILE_READ_DOUBLEWORD_LITTLE_ATR(Runtime& runtime, InstructionView view)
{
    const auto file = runtime.files.get_file(view.get_r2(runtime));
    file->seek_cursor(view.get_r3(runtime));
    runtime.registries.write(
        view.r1(),
        file->read_doubleword_little()
    );
}

void instr_FILE_READ_DOUBLEWORD_LITTLE_ATD(Runtime& runtime, InstructionView view)
{
    const auto file = runtime.files.get_file(view.get_r2(runtime));
    file->seek_cursor(view.get_data(runtime));
    runtime.registries.write(
        view.r1(),
        file->read_doubleword_little()
    );
}

void instr_FILE_APPEND_BYTE(Runtime& runtime, InstructionView view)
{
    runtime.files.get_file(view.get_r2(runtime))->append_byte(view.get_r1(runtime));
}

void instr_FILE_APPEND_WORD(Runtime& runtime, InstructionView view)
{
    runtime.files.get_file(view.get_r2(runtime))->append_word_big(view.get_r1(runtime));
}

void instr_FILE_APPEND_WORD_LITTLE(Runtime& runtime, InstructionView view)
{
    runtime.files.get_file(view.get_r2(runtime))->append_word_little(view.get_r1(runtime));
}

void instr_FILE_APPEND_DOUBLEWORD(Runtime& runtime, InstructionView view)
{
    runtime.files.get_file(view.get_r2(runtime))->append_doubleword_big(view.get_r1(runtime));
}

void instr_FILE_APPEND_DOUBLEWORD_LITTLE(Runtime& runtime, InstructionView view)
{
    runtime.files.get_file(view.get_r2(runtime))->append_doubleword_little(view.get_r1(runtime));
}

void instr_FILE_IS_BYTE_REMAINING(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.files.get_file(view.get_r2(runtime))->has_byte_remaining()
    );
}

void instr_FILE_IS_WORD_REMAINING(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.files.get_file(view.get_r2(runtime))->has_word_remaining()
    );
}

void instr_FILE_IS_DOUBLEWORD_REMAINING(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.files.get_file(view.get_r2(runtime))->has_doubleword_remaining()
    );
}

void instr_CLOCK_CREATE(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.clocks.create_clock()
    );
}

void instr_CLOCK_DELETE(Runtime& runtime, InstructionView view)
{
    runtime.clocks.delete_clock(
        view.get_r1(runtime)
    );
}

void instr_CLOCK_GET_ELAPSED_MS(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.clocks.get_clock_time_ms(view.get_r2(runtime))
    );
}

void instr_CLOCK_GET_ELAPSED_S(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.clocks.get_clock_time_s(view.get_r2(runtime))
    );
}

void instr_CLOCK_RESET(Runtime& runtime, InstructionView view)
{
    runtime.clocks.reset_clock(view.get_r1(runtime));
}
