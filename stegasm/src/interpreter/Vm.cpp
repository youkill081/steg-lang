//
// Created by Roumite on 21/02/2026.
//

#include "Vm.h"

#include "Loader.h"
#include "runtime/Runtime.h"

#include <iostream>

#include "Logger.h"

void Vm::run(ByteBuffer& buffer)
{
    Runtime runtime = Loader::load(buffer);

    while (runtime.is_running)
    {
        if (runtime.instructions.size() <= runtime.instruction_pointer)
        {
            Logger::log("Reach end of program", "Vm");
            break;
        }

        const auto &current_instr = runtime.instructions[runtime.instruction_pointer];
        runtime.instruction_pointer++;
        current_instr.desc.fn(runtime, current_instr.view);
    }
}
