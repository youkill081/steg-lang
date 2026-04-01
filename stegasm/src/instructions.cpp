//
// Created by Roumite on 21/02/2026.
//

#include <iostream>
#include <random>

#include "instructions.h"

#include "Logger.h"
#include "interpreter/exceptions.h"
#include "interpreter/runtime/Runtime.h"

uint32_t InstructionView::get_data(const Runtime& rt, uint8_t size) const
{
    uint32_t val = get_raw_data();
    if (data_type() == 0b10) {
        switch (size) {
        case 1: return rt.memory.read_uint8(val);
        case 2: return rt.memory.read_uint16(val);
        default: return rt.memory.read_uint32(val);
        }
    }
    return val;
}

uint32_t InstructionView::get_r1(const Runtime& rt, uint8_t size) const
{
    uint32_t value = rt.registries.read(this->r1());
    if (is_r1_addr())
    {
        switch (size) {
        case 1: return rt.memory.read_uint8(value);
        case 2: return rt.memory.read_uint16(value);
        default: return rt.memory.read_uint32(value);
        }
    }
    return value;
}

uint32_t InstructionView::get_r2(const Runtime& rt, uint8_t size) const
{

    uint32_t value = rt.registries.read(this->r2());
    if (is_r2_addr())
    {
        switch (size) {
        case 1: return rt.memory.read_uint8(value);
        case 2: return rt.memory.read_uint16(value);
        default: return rt.memory.read_uint32(value);
        }
    }
    return value;
}

uint32_t InstructionView::get_r3(const Runtime& rt, uint8_t size) const
{
    uint32_t value = rt.registries.read(this->r3());
    if (is_r3_addr())
    {
        switch (size) {
        case 1: return rt.memory.read_uint8(value);
        case 2: return rt.memory.read_uint16(value);
        default: return rt.memory.read_uint32(value);
        }
    }
    return value;
}


// ASM Instructions

void instr_EOF(Runtime& runtime, InstructionView view)
{
    std::cout << "Reach EOF" << std::endl;
    runtime.return_value = 0;
    runtime.is_running = false;
}

inline void instr_LOADD_32(Runtime &runtine, InstructionView view)
{
    runtine.registries.write(view.r1(), view.get_data(runtine));
}

inline void instr_LOADR_32(Runtime &runtine, InstructionView view)
{
    runtine.registries.write(
        view.r1(),
        view.get_r2(runtine)
    );
}

inline void instr_LOADD_16(Runtime &runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_data(runtime, 2) & 0xFFFF
    );
}

inline void instr_LOADR_16(Runtime &runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_r2(runtime, 2) & 0xFFFF
    );
}

inline void instr_LOADD_8(Runtime &runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        view.get_data(runtime, 1) & 0xFF
    );
}

inline void instr_LOADR_8(Runtime &runtime, InstructionView view)
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
        runtime.registries.read(view.r2()),
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
        runtime.registries.read(view.r2()),
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
        runtime.registries.read(view.r2()),
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
        runtime.registries.read(view.r1()) + view.get_r2(runtime)
    );
}

void instr_ADDD(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.registries.read(view.r1()) + view.get_data(runtime)
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

void instr_MULR(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.registries.read(view.r1()) * view.get_r2(runtime)
    );
}

void instr_MULD(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.registries.read(view.r1()) * view.get_data(runtime)
    );
}

void instr_DIVR(Runtime& runtime, InstructionView view)
{
    if (view.get_r2(runtime) == 0)
        throw InterpreterError("[DIV] Division by zero !");
    runtime.registries.write(
        view.r1(),
        runtime.registries.read(view.r1()) / view.get_r2(runtime)
    );
}

void instr_DIVD(Runtime& runtime, InstructionView view)
{
    if (view.get_data(runtime) == 0)
        throw InterpreterError("[DIV] Division by zero !");
    runtime.registries.write(
        view.r1(),
        runtime.registries.read(view.r1()) / view.get_data(runtime)
    );
}

void instr_MINR(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::min(
            runtime.registries.read(view.r1()),
            view.get_r2(runtime)
        )
    );
}

void instr_MIND(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::min(
            runtime.registries.read(view.r1()),
            view.get_data(runtime)
        )
    );
}

void instr_MAXR(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::max(
            runtime.registries.read(view.r1()),
            view.get_r2(runtime)
        )
    );
}

void instr_MAXD(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        std::max(
            runtime.registries.read(view.r1()),
            view.get_data(runtime)
        )
    );
}

void instr_MODR(Runtime& runtime, InstructionView view)
{
    if (view.get_r2(runtime) == 0)
        throw InterpreterError("[MOD] Modulo by zero !");
    runtime.registries.write(
        view.r1(),
        runtime.registries.read(view.r1()) % view.get_r2(runtime)
    );
}

void instr_MODD(Runtime& runtime, InstructionView view)
{
    if (view.get_data(runtime) == 0)
        throw InterpreterError("[MOD] Modulo by zero !");
    runtime.registries.write(
        view.r1(),
        runtime.registries.read(view.r1()) % view.get_data(runtime)
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

void instr_JMP(Runtime &runtime, InstructionView view)
{
    runtime.instruction_pointer = view.get_data(runtime);
}

void compute_CMP(Runtime &runtime, uint32_t first_value, uint32_t second_value)
{
    runtime.comparison_flag.equal = first_value == second_value;
    runtime.comparison_flag.greater = first_value > second_value;
    runtime.comparison_flag.lower = first_value < second_value;

    runtime.comparison_flag.signed_greater = static_cast<int32_t>(first_value) > static_cast<int32_t>(second_value);
    runtime.comparison_flag.signed_lower = static_cast<int32_t>(first_value) < static_cast<int32_t>(second_value);
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
        runtime.registries.read(view.r1()),
        view.get_data(runtime)
    );
}

void instr_JE(Runtime& runtime, InstructionView view)
{
    if (runtime.comparison_flag.equal)
        runtime.instruction_pointer = view.get_data(runtime);
}

void instr_JNE(Runtime& runtime, InstructionView view)
{
    if (not runtime.comparison_flag.equal)
        runtime.instruction_pointer = view.get_data(runtime);
}

void instr_JA(Runtime& runtime, InstructionView view)
{
    if (runtime.comparison_flag.greater)
        runtime.instruction_pointer = view.get_data(runtime);
}

void instr_JSA(Runtime& runtime, InstructionView view)
{
    if (runtime.comparison_flag.signed_greater)
        runtime.instruction_pointer = view.get_data(runtime);
}

void instr_JB(Runtime& runtime, InstructionView view)
{
    if (runtime.comparison_flag.lower)
        runtime.instruction_pointer = view.get_data(runtime);
}

void instr_JSB(Runtime& runtime, InstructionView view)
{
    if (runtime.comparison_flag.signed_lower)
        runtime.instruction_pointer = view.get_data(runtime);
}

void instr_DISPLAY_N(Runtime &runtime, InstructionView view)
{
    std::cout << view.get_r1(runtime) << std::endl;
}

void instr_DISPLAY_C(Runtime &runtime, InstructionView view)
{
    std::cout << static_cast<char>(view.get_r1(runtime)) << std::flush;
}

void instr_DISPLAY_B(Runtime &runtime, InstructionView view)
{
    Logger::log_uint32_as_bit(view.get_r1(runtime));
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
        runtime.memory.allocate(view.get_data(runtime))
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
    runtime.instruction_pointer = view.get_data(runtime);
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
    uint16_t width = view.get_r1(runtime);
    uint16_t height = view.get_r2(runtime);
    std::string name = runtime.utils.get_string_from_address(view.get_data(runtime));

    runtime.graphical_backend.create_window(width, height, name);
}

void instr_WINDOW_CREATE3(Runtime& runtime, InstructionView view)
{
    uint16_t width = view.get_r1(runtime);
    uint16_t height = view.get_r2(runtime);
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
    uint16_t x = view.get_r1(runtime);
    uint16_t y = view.get_r2(runtime);
    std::string text = runtime.utils.get_string_from_address(view.get_data(runtime));

    runtime.graphical_backend.draw_text(text, x, y);
}

void instr_WINDOW_DRAW_TEXT3(Runtime& runtime, InstructionView view)
{
    uint16_t x = view.get_r1(runtime);
    uint16_t y = view.get_r2(runtime);
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

void instr_FILE_OPEN(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.files.open_file(runtime.utils.get_string_from_address(view.get_data(runtime)))
    );
}

void instr_FILE_CREATE(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.files.create_file(runtime.utils.get_string_from_address(view.get_data(runtime)))
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
        runtime.registries.read(view.r1())
    );
}

void instr_CLOCK_GET_ELAPSED_MS(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.clocks.get_clock_time_ms(runtime.registries.read(view.r2()))
    );
}

void instr_CLOCK_GET_ELAPSED_S(Runtime& runtime, InstructionView view)
{
    runtime.registries.write(
        view.r1(),
        runtime.clocks.get_clock_time_s(runtime.registries.read(view.r2()))
    );
}

void instr_CLOCK_RESET(Runtime& runtime, InstructionView view)
{
    runtime.clocks.reset_clock(runtime.registries.read(view.r1()));
}
