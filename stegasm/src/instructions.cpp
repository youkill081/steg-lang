//
// Created by Roumite on 21/02/2026.
//


#include <iostream>

#include "instructions.h"

#include "Logger.h"
#include "interpreter/runtime/Runtime.h"

uint16_t InstructionView::get_d1(const Runtime &rt, bool force_as_address) const
{
    const auto val = static_cast<uint16_t>((raw_data >> 32) & 0xFFFF);
    if (is_d1_addr() && not force_as_address) {
        return rt.memory.read(val);
    }
    return val;
}

uint16_t InstructionView::get_d2(const Runtime &rt, bool force_as_address) const
{
    const auto val = static_cast<uint16_t>(raw_data & 0xFFFF);
    if (is_d2_addr() && not force_as_address) {
        return rt.memory.read(val);
    }
    return val;
}


// ASM Instructions

inline void instr_LOADA(Runtime &runtine, InstructionView view)
{
    runtine.registries.write(view.r1(), view.get_d1(runtine));
}

inline void instr_LOADR(Runtime &runtine, InstructionView view)
{
    runtine.registries.write(
        view.r1(),
        runtine.memory.read(runtine.registries.read(view.r2()))
    );
}

void instr_STOREA(Runtime &runtime, InstructionView view)
{
    runtime.memory.write(
        view.get_d1(runtime, true),
        runtime.registries.read(view.r1())
    );
}

void instr_STORER(Runtime &runtime, InstructionView view)
{
    runtime.memory.write(
        runtime.registries.read(view.r2()),
        runtime.registries.read(view.r1())
    );
}

void instr_MOV(Runtime &runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.registries.read(view.r2())
    );
}

void instr_ADD(Runtime &runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.registries.read(view.r1()) + runtime.registries.read(view.r2())
    );
}

void instr_SUB(Runtime &runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.registries.read(view.r1()) - runtime.registries.read(view.r2())
    );
}

void instr_JMP(Runtime &runtime, InstructionView view)
{
    runtime.instruction_pointer = view.get_d1(runtime);
}

void compute_CMP(Runtime &runtime, uint16_t first_value, uint16_t second_value)
{
    runtime.comparison_flag.equal = first_value == second_value;
    runtime.comparison_flag.greater = first_value > second_value;
    runtime.comparison_flag.lower = first_value < second_value;
}

void instr_CMPR(Runtime &runtime, InstructionView view)
{
    compute_CMP(
        runtime,
        runtime.registries.read(view.r1()),
        runtime.registries.read(view.r2())
    );
}

void instr_CMPA(Runtime &runtime, InstructionView view)
{
    compute_CMP(
        runtime,
        runtime.registries.read(view.r1()),
        view.get_d1(runtime)
    );
}

void instr_JE(Runtime& runtime, InstructionView view)
{
    if (runtime.comparison_flag.equal)
        runtime.instruction_pointer = view.get_d1(runtime);
}

void instr_JNE(Runtime& runtime, InstructionView view)
{
    if (not runtime.comparison_flag.equal)
        runtime.instruction_pointer = view.get_d1(runtime);
}

void instr_JA(Runtime& runtime, InstructionView view)
{
    if (not runtime.comparison_flag.greater)
        runtime.instruction_pointer = view.get_d1(runtime);
}

void instr_JB(Runtime& runtime, InstructionView view)
{
    if (not runtime.comparison_flag.lower)
        runtime.instruction_pointer = view.get_d1(runtime);
}

void instr_DISPLAY_N(Runtime &runtime, InstructionView view)
{
    std::cout << runtime.registries.read(view.r1()) << std::endl;
}

void instr_DISPLAY_AN(Runtime &runtime, InstructionView view)
{
    std::cout << runtime.memory.read(runtime.registries.read(view.r1())) << std::endl;
}

void instr_DISPLAY_C(Runtime &runtime, InstructionView view)
{
    std::cout << static_cast<char>(runtime.registries.read(view.r1())) << std::endl;
}

void instr_DISPLAY_AC(Runtime &runtime, InstructionView view)
{
    std::cout << static_cast<char>(runtime.memory.read(runtime.registries.read(view.r1()))) << std::endl;
}

void instr_DISPLAY_B(Runtime &runtime, InstructionView view)
{
    Logger::log_uint16_as_bit(runtime.registries.read(view.r1()));
}

void instr_DISPLAY_AB(Runtime &runtime, InstructionView view)
{
    Logger::log_uint16_as_bit(runtime.memory.read(runtime.registries.read(view.r1())));
}

void instr_HALT(Runtime &runtime, InstructionView view)
{
    runtime.return_value = runtime.registries.read(R0);
    runtime.is_running = false;
}

void instr_ALOCA(Runtime &runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.memory.allocate(view.get_d1(runtime))
    );
}

void instr_ALOCR(Runtime &runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.memory.allocate(runtime.registries.read(view.r2()))
    );
}

void instr_FREE(Runtime &runtime, InstructionView view)
{
    runtime.memory.free(
        runtime.registries.read(view.r1())
    );
}

void instr_DEBUG_R(Runtime& runtime, InstructionView view)
{
    Logger::log("DEBUG_REGISTRIES", "instr_DEBUG_REGISTRIES");
    runtime.registries.display();
}

void instr_DEBUG_A(Runtime& runtime, InstructionView view)
{
    Logger::log("DEBUG_MEMORY", "instr_DEBUG_MEMORY");
    runtime.memory.display();
}
