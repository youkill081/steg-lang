//
// Created by Roumite on 21/02/2026.
//

#include "Vm.h"

#include "Loader.h"
#include "runtime/Runtime.h"

#include <iostream>

#include "Logger.h"

uint32_t Vm::run(ByteBuffer& buffer)
{
/*
 * Vm Ignition
 */

    Runtime runtime = Loader::load(buffer);

    const LoadedInstruction *curr_instruction = runtime.instructions.data();
    const LoadedInstruction *instructions_beg  = runtime.instructions.data();
    const LoadedInstruction *instructions_end = instructions_beg + runtime.instructions.size();

    runtime.curr_instruction = &curr_instruction;
    runtime.instructions_beg = instructions_beg;
    runtime.instructions_end = instructions_end;

#ifdef ENABLE_INSTRUCTION_COUNTER
    uint64_t instruction_count = 0;
    uint64_t batch_counter = 0;

    constexpr uint32_t BATCH_SIZE = 10000; // wait 10,000 instructions before time checking
    auto last_time = std::chrono::steady_clock::now();
#endif


/*
 * Vm Running
 */


    while (runtime.is_running)
    {
        if (curr_instruction >= instructions_end)
            break;
        const LoadedInstruction *current_instr = curr_instruction;
        curr_instruction++;
        current_instr->handler.fn(runtime, current_instr->view);

#ifdef ENABLE_INSTRUCTION_COUNTER
        instruction_count++;

        if (++batch_counter >= BATCH_SIZE) {
            batch_counter = 0;
            auto current_time = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(current_time - last_time);

            if (elapsed.count() >= 1) {
                std::cout << "[Counter] Ips/sec: " << instruction_count << std::endl;
                instruction_count = 0;
                last_time = current_time;
            }
        }
#endif
    }
    return runtime.return_value;
}
