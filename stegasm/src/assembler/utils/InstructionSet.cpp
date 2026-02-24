//
// Created by Roumite on 23/02/2026.
//

#include "InstructionSet.h"

#include <iostream>

void assembler::InstructionSet::display() const
{
    for (const auto &instruction : *this)
    {
        std::cout << instruction.desc.name << " registrie(s) -> ";
        for (int i = 0; i < instruction.desc.regCount; i++)
            std::cout << registryToString.at(instruction.registries[i]) << " ";
        std::cout << "data(s) -> ";
        for (int i = 0; i < instruction.desc.dataCount; i++)
            std::cout << "\"" << instruction.datas[i].value << "\"" << (instruction.datas[i].is_address ? " as address " : " as constant ");
        std::cout << std::endl;
    }
}
