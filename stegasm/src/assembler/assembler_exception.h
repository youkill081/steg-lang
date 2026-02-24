//
// Created by Roumite on 23/02/2026.
//

#pragma once

#include <stdexcept>
#include <string>


namespace assembler
{
    struct AssemblerError : public std::runtime_error
    {
        explicit AssemblerError(const std::string& Message) : std::runtime_error(Message) {}
    };
}
