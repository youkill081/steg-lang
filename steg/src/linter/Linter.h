//
// Created by Roumite on 17/03/2026.
//

#pragma once

#include <filesystem>

#include "lexer/Lexer.h"

namespace compiler
{
    struct LexerToken;

    struct LintData
    {
        enum class Severity { ERR, HINT };

        std::string message;
        std::filesystem::path file;
        uint32_t line;
        uint32_t column;
        uint32_t length;
        Severity severity;
    };

    class Linter
    {
    private:
        std::vector<LintData> _lints;
        bool _disabled = false;
    public:
        static Linter& instance() // One linter for all compiler process
        {
            static Linter linter;
            return linter;
        }

        void report(
            const std::string& message,
            const std::filesystem::path &file,
            uint32_t line,
            uint32_t column,
            uint32_t length = 1,
            LintData::Severity severity = LintData::Severity::ERR
        ) {
            if (_disabled)
                return;
            _lints.push_back({ message, file, line, column, length, severity });
        }

        void report(
            const std::string &message,
            const LexerToken &token,
            const LintData::Severity severity = LintData::Severity::ERR
        ) {
            if (_disabled)
                return;
            report(message, token.path, token.line_number, token.column_number, token.value.size(), severity);
        }

        [[nodiscard]] const std::vector<LintData>& get_lint_result() const { return _lints; }
        [[nodiscard]] bool has_errors() const
        {
            for (const auto &lint : _lints)
                if (lint.severity == LintData::Severity::ERR)
                    return true;
            return false;
        }

        void display_diagnostics() const;

        void clear() { _lints.clear(); };

        void disable() { _disabled = true; }
        void enable()  { _disabled = false; }
    };
}
