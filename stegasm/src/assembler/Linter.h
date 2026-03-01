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
        ParsedLine original_parsed_line;

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

        bool display_error_when_occur;

        static void display_error(const LinterMessage &message);
        static void display_warning(const LinterMessage &message);
    public:
        explicit Linter(bool display_error_when_occur = true) : display_error_when_occur(display_error_when_occur) {}

        [[noreturn]] static void error(const std::string &message, uint32_t token_index = 0); // Call this function in Linter::foreach loop
        void inline_error(const std::string &message); // Call this function output Linter::foreach loop
        void inline_warning(const std::string &message); // Call this function output Linter::foreach loop

        void foreach_lines(std::span<const ParsedLine> lines, const std::function<void(const ParsedLine&)> &callback);

        template<std::ranges::range Range, typename Callback>
        void foreach(const Range &lines, Callback &&callback)
        {
            current = this;
            for (const auto &line : lines)
            {
                try { callback(line); }
                catch (const LinterError &e) {
                    auto message = LinterMessage{
                        0,
                        "",
                        {},
                        e.message,
                        e.token_index };
                    errors.push_back(message);
                    display_error(message);
                }
            }
            current = nullptr;
        }

        void error_guard(const std::function<void()> &callback);

        [[nodiscard]] bool has_errors() const { return not errors.empty(); }
        [[nodiscard]] bool has_warning() const { return not warnings.empty(); }

        [[nodiscard]] const std::vector<LinterMessage> &get_errors() const { return errors; }
        [[nodiscard]] const std::vector<LinterMessage> &get_warnings() const { return warnings; }
    };
}
