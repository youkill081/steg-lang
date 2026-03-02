//
// Created by Roumite on 20/02/2026.
//

#pragma once

#include <stack>
#include <vector>

#include "Clocks.h"
#include "GraphicalBackend.h"
#include "Memory.h"
#include "Registries.h"
#include "Utils.h"
#include "files/Files.h"

using ComparisonFlag = struct ComparisonFlag
{
    bool equal = false;
    bool greater = false;
    bool lower = false;
};

using LoadedInstruction = struct LoadedInstruction
{
    InstructionView view;
    const InstructionDesc &desc;
};

class Runtime
{
public:
    Memory memory;
    Registries registries;
    ComparisonFlag comparison_flag;
    Files files;
    Clocks clocks;
    std::stack<uint16_t> stack{};
    GraphicalBackend graphical_backend;
    Utils utils{*this};

    std::vector<LoadedInstruction> instructions{};

    uint64_t instruction_pointer = 0;

    bool is_running = true;
    uint16_t return_value = 0;
};
