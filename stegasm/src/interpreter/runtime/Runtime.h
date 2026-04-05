//
// Created by Roumite on 20/02/2026.
//

#pragma once

#include <stack>
#include <vector>

#include "Clocks.h"
#include "GraphicalBackend.h"
#include "InstructionView.h"
#include "Memory.h"
#include "Registries.h"
#include "Stack.h"
#include "Utils.h"
#include "files/Files.h"

using ComparisonFlag = struct ComparisonFlag
{
    bool equal = false;
    bool greater = false;
    bool lower = false;

    bool signed_greater = false;
    bool signed_lower = false;
};

using LoadedInstruction = struct LoadedInstruction
{
    InstructionView view;
    const InstructionDesc &desc;
    const InstructionHandler &handler;
};

class Runtime
{
public:
    Memory memory;
    Registries registries;
    ComparisonFlag comparison_flag;
    Files files;
    Clocks clocks;
    Stack stack{}; // Force usage of vector, more efficient than deque
    GraphicalBackend graphical_backend;
    Utils utils{*this};

    std::vector<LoadedInstruction> instructions{};

    uint64_t instruction_pointer = 0;

    bool is_running = true;
    uint32_t return_value = 0;
};
