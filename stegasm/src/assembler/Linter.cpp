//
// Created by Roumite on 26/02/2026.
//

#include "./Linter.h"

#include <iostream>

#include "Colors.h"

using namespace assembler;

Linter* Linter::current = nullptr;

void Linter::display_error(const LinterMessage& message)
{
    std::cout   << AnsiColors::Red << AnsiColors::Bold << "[ERROR] "
                << AnsiColors::Reset << AnsiColors::Red << "line " << message.line_number << ": " << message.message
                  << "\n  >> " << message.original_line << "\n" << AnsiColors::Reset;
}

void Linter::display_warning(const LinterMessage& message)
{
    std::cout   << AnsiColors::Yellow << AnsiColors::Bold << "[WARNING] "
                << AnsiColors::Reset << AnsiColors::Yellow << "line " << message.line_number << ": " << message.message
                  << "\n  >> " << message.original_line << "\n" << AnsiColors::Reset;
}

void Linter::error(const std::string &message, uint32_t token_index)
{
    throw LinterError{ message, token_index };
}

void Linter::inline_error(const std::string& message)
{
    auto final_message = LinterMessage{
        .line_number = 0,
        .original_line = "",
        .message = message,
        .token_index = 0
    };
    errors.push_back(final_message);
    display_error(final_message);
}

void Linter::inline_warning(const std::string& message)
{
    auto final_message = LinterMessage{
        .line_number = 0,
        .original_line = "",
        .message = message,
        .token_index = 0
    };
    warnings.push_back(final_message);
    display_warning(final_message);
}

void Linter::foreach(std::span<const ParsedLine> lines, const std::function<void(const ParsedLine&)> &callback)
{
    current = this;
    for (const auto &line : lines)
    {
        try { callback(line); }
        catch (const LinterError &e) {
            auto message = LinterMessage{ line.line_number, line.original_line, e.message, e.token_index };
            errors.push_back(message);
            display_error(message);
        }
    }
    current = nullptr;
}