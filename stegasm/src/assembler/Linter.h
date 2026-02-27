//
// Created by Roumite on 26/02/2026.
//

#pragma once

#include "TextParser.h"

#include <span>
#include <string>
#include <functional>

namespace assembler
{
    struct LinterMessage
    {
        uint32_t line_number;
        std::string original_line;
        std::string message;
        uint32_t token_index;
    };

    struct LinterError { std::string message; uint32_t token_index;};
    struct LinterWarning { std::string message; uint32_t token_index; };

    class Linter
    {
    private:
        static Linter* current;
        std::vector<LinterMessage> errors;
        std::vector<LinterMessage> warnings;

        static void display_error(const LinterMessage &message);
        static void display_warning(const LinterMessage &message);
    public:
        [[noreturn]] static void error(const std::string &message, uint32_t token_index = 0); // Call this function in Linter::foreach loop
        void inline_error(const std::string &message); // Call this function output Linter::foreach loop
        void inline_warning(const std::string &message); // Call this function output Linter::foreach loop

        void foreach(std::span<const ParsedLine> lines, const std::function<void(const ParsedLine&)> &callback);

        [[nodiscard]] bool has_errors() const { return not errors.empty(); }
        [[nodiscard]] bool has_warning() const { return not warnings.empty(); }
    };
}
