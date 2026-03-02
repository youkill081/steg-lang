//
// Created by Roumite on 21/02/2026.
//

#include <iostream>
#include <random>

#include "instructions.h"

#include "Logger.h"
#include "interpreter/exceptions.h"
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

void instr_EOF(Runtime& runtime, InstructionView view)
{
    std::cout << "Reach EOF" << std::endl;
    runtime.return_value = 0;
    runtime.is_running = false;
}

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
        view.get_d1(runtime),
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

void instr_ADDA(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.registries.read(view.r1()) + view.get_d1(runtime)
    );
}


void instr_SUB(Runtime &runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.registries.read(view.r1()) - runtime.registries.read(view.r2())
    );
}

void instr_SUBA(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.registries.read(view.r1()) - view.get_d1(runtime)
    );
}

void instr_MUL(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.registries.read(view.r1()) * runtime.registries.read(view.r2())
    );
}

void instr_MULA(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.registries.read(view.r1()) * view.get_d1(runtime)
    );
}

void instr_DIV(Runtime& runtime, InstructionView view)
{
    if (runtime.registries.read(view.r2()) == 0)
        throw InterpreterError("[DIV] Division by zero !");
    runtime.registries.write(
        view.r1(),
        runtime.registries.read(view.r1()) / runtime.registries.read(view.r2())
    );
}

void instr_DIVA(Runtime& runtime, InstructionView view)
{
    if (view.get_d1(runtime) == 0)
        throw InterpreterError("[DIV] Division by zero !");
    runtime.registries.write(
        view.r1(),
        runtime.registries.read(view.r1()) / view.get_d1(runtime)
    );
}

void instr_MOD(Runtime& runtime, InstructionView view)
{
    if (runtime.registries.read(view.r2()) == 0)
        throw InterpreterError("[MOD] Modulo by zero !");
    runtime.registries.write(
        view.r1(),
        runtime.registries.read(view.r1()) % runtime.registries.read(view.r2())
    );
}

void instr_MODA(Runtime& runtime, InstructionView view)
{
    if (view.get_d1(runtime) == 0)
        throw InterpreterError("[MOD] Modulo by zero !");
    runtime.registries.write(
        view.r1(),
        runtime.registries.read(view.r1()) % view.get_d1(runtime)
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
    std::cout << static_cast<char>(runtime.registries.read(view.r1())) << std::flush;
}

void instr_DISPLAY_AC(Runtime &runtime, InstructionView view)
{
    std::cout << static_cast<char>(runtime.memory.read(runtime.registries.read(view.r1()))) << std::flush;
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

void instr_DEBUG_M(Runtime& runtime, InstructionView view)
{
    Logger::log("DEBUG_MEMORY", "instr_DEBUG_MEMORY");
    runtime.memory.display();
}

void instr_CALL(Runtime& runtime, InstructionView view)
{
    runtime.stack.push(runtime.instruction_pointer);
    runtime.instruction_pointer = view.get_d1(runtime);
}

void instr_RET(Runtime& runtime, InstructionView view)
{
    if (runtime.stack.empty())
    {
        throw InterpreterError("[RET] Try to pop empty stack");
    }

    runtime.instruction_pointer = runtime.stack.top();
    runtime.stack.pop();
}

void instr_PUSH(Runtime& runtime, InstructionView view)
{
    runtime.stack.push(runtime.registries.read(view.r1()));
}

void instr_POP(Runtime& runtime, InstructionView view)
{
    if (runtime.stack.empty())
        throw InterpreterError("[POP] Try to pop empty stack");
    runtime.registries.write(view.r1(), runtime.stack.top());
    runtime.stack.pop();
}

void instr_RAND(Runtime& runtime, InstructionView view)
{
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<uint16_t> dist(0, UINT16_MAX);

    runtime.registries.write(
        view.r1(),
        dist(rng)
    );
}

void instr_WINDOW_CREATE(Runtime& runtime, InstructionView view)
{
    uint16_t width = runtime.registries.read(view.r1());
    uint16_t height = runtime.registries.read(view.r2());
    std::string name = runtime.utils.get_string_from_address(view.get_d1(runtime));

    runtime.graphical_backend.create_window(width, height, name);
}

void instr_WINDOW_CLOSE(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.close_window();
}

void instr_WINDOW_SET_VIEWPORT_SIZE(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.set_viewport_size(
        runtime.registries.read(view.r1()),
        runtime.registries.read(view.r2())
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
        static_cast<uint8_t>(runtime.registries.read(view.r1())),
        static_cast<uint8_t>(runtime.registries.read(view.r2())),
        static_cast<uint8_t>(runtime.registries.read(view.r3())),
    });
}

void instr_WINDOW_PRESENT(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.present_window();
}

void instr_WINDOW_KEY_PRESSED(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.graphical_backend.key_pressed(view.get_d1(runtime))
    );
}

void instr_WINDOW_KEY_DOWN(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.graphical_backend.key_down(view.get_d1(runtime))
    );
}

void instr_WINDOW_SET_TARGET_FPS(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.set_target_fps(
        view.get_d1(runtime)
    );
}

void instr_WINDOW_SET_TEXT_SIZE(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.set_text_size(
        view.get_d1(runtime)
    );
}

void instr_WINDOW_SET_TEXT_COLOR(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.set_text_color({
        static_cast<uint8_t>(runtime.registries.read(view.r1())),
        static_cast<uint8_t>(runtime.registries.read(view.r2())),
        static_cast<uint8_t>(runtime.registries.read(view.r3())),
    });
}

void instr_WINDOW_SET_FONT(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.set_font(
        runtime.files[view.get_d1(runtime)]
    );
}

void instr_WINDOW_DRAW_TEXT(Runtime& runtime, InstructionView view)
{
    uint16_t x = runtime.registries.read(view.r1());
    uint16_t y = runtime.registries.read(view.r2());
    std::string text = runtime.utils.get_string_from_address(view.get_d1(runtime));

    runtime.graphical_backend.draw_text(text, x, y);
}

void instr_WINDOW_DRAW_TEXTURE(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.draw_texture(
        runtime.files[view.get_d1(runtime)],
        runtime.registries.read(view.r1()),
        runtime.registries.read(view.r2())
    );
}

void instr_WINDOW_SET_TEXTURE_COLOR_MASK(Runtime& runtime, InstructionView view)
{
    runtime.graphical_backend.set_texture_color_mask({
        static_cast<uint8_t>(runtime.registries.read(view.r1())),
        static_cast<uint8_t>(runtime.registries.read(view.r2())),
        static_cast<uint8_t>(runtime.registries.read(view.r3())),
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
        runtime.files[view.get_d1(runtime)]
    );
}

void instr_FILE_OPEN(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.files.open_file(runtime.utils.get_string_from_address(view.get_d1(runtime)))
    );
}

void instr_FILE_CREATE(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.files.create_file(runtime.utils.get_string_from_address(view.get_d1(runtime)))
    );
}

void instr_FILE_SAVE(Runtime& runtime, InstructionView view)
{
    runtime.files.get_file(runtime.registries.read(view.r1()))->save();
}

void instr_FILE_DELETE(Runtime& runtime, InstructionView view)
{
    runtime.files.delete_file(
        runtime.registries.read(view.r1())
    );
}

void instr_FILE_CLOSE(Runtime& runtime, InstructionView view)
{
    runtime.files.close_file(
        runtime.registries.read(view.r1())
    );
}

void instr_FILE_RESET_CURSOR(Runtime& runtime, InstructionView view)
{
    runtime.files.get_file(runtime.registries.read(view.r1()))->reset_cursor();
}

void instr_FILE_CLEAR_DATA(Runtime& runtime, InstructionView view)
{
    runtime.files.get_file(runtime.registries.read(view.r1()))->clear_data();
}

void instr_FILE_READ_BYTE(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.files.get_file(runtime.registries.read(view.r2()))->read_byte()
    );
}

void instr_FILE_READ_WORD(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.files.get_file(runtime.registries.read(view.r2()))->read_word()
    );
}

void instr_FILE_APPEND_BYTE(Runtime& runtime, InstructionView view)
{
    runtime.files.get_file(runtime.registries.read(view.r2()))->append_byte(runtime.registries.read(view.r1()));
}

void instr_FILE_APPEND_WORD(Runtime& runtime, InstructionView view)
{
    runtime.files.get_file(runtime.registries.read(view.r2()))->append_word(runtime.registries.read(view.r1()));
}

void instr_FILE_IS_BYTE_REMAINING(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.files.get_file(runtime.registries.read(view.r2()))->has_byte_remaining()
    );
}

void instr_FILE_IS_WORD_REMAINING(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.files.get_file(runtime.registries.read(view.r2()))->has_word_remaining()
    );
}
