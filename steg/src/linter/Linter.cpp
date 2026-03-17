//
// Created by Roumite on 17/03/2026.
//


#include <iostream>
#include <fstream>

#include "Linter.h"

#include <unordered_map>

#include "Colors.h"

using namespace compiler;


static std::unordered_map<std::string, std::vector<std::string>> file_cache;

std::pair<std::string, const char*> severity_label_and_color(LintError::Severity s)
{
    switch (s)
    {
    case LintError::Severity::ERR: return {"error", AnsiColors::Red};
    case LintError::Severity::WARN: return {"warning", AnsiColors::Yellow};
    case LintError::Severity::HINT: return {"hint", AnsiColors::Cyan};
    default: return {"note", AnsiColors::Cyan};
    }
}

std::filesystem::path make_relative_path(const std::filesystem::path& p)
{
    try
    {
        return std::filesystem::relative(p, std::filesystem::current_path());
    }
    catch (...) {return p;}
}

const std::vector<std::string>& load_file_lines(const std::filesystem::path &file)
{
    auto key = file.string();
    auto it = file_cache.find(key);
    if (it != file_cache.end()) return it->second;

    std::vector<std::string> lines;
    std::ifstream ifs(file);
    if (!ifs.is_open())
    {
        auto res = file_cache.emplace(key, std::move(lines));
        return res.first->second;
    }

    std::string line;
    while (std::getline(ifs, line))
        lines.push_back(line);

    auto res = file_cache.emplace(key, std::move(lines));
    return res.first->second;
}

std::string expand_tabs(const std::string &s)
{
    std::string out;
    out.reserve(s.size());
    for (const char c : s)
    {
        if (c == '\t')
            out.append(4, ' ');
        else
            out.push_back(c);
    }
    return out;
}

void print_header_and_location(
    const LintError &err,
    const std::filesystem::path &relativePath,
    const char *color,
    const std::string &label)
{
    std::cout << AnsiColors::Bold << color << label << AnsiColors::Reset
        << " : " << err.message << "\n";

    std::cout << "\t-> "
        << relativePath.string()
        << ":" << err.line
        << ":" << err.column << "\n";

    std::cout << "\t|\t\n";
}

void print_code_line_with_caret(const LintError& err, const std::vector<std::string>& file_lines)
{
    const uint32_t requested = err.line;
    std::string line_content;
    if (requested >= 1 && requested <= file_lines.size())
        line_content = file_lines[requested - 1];
    else
        line_content = "";

    const std::string expanded = expand_tabs(line_content);

    std::ostringstream oss;
    oss << err.line;
    const std::string line_no_str = oss.str();

    std::cout << "\t" << AnsiColors::Blue << line_no_str << AnsiColors::Reset
        << "|\t" << expanded << "\n";
    std::cout << "\t|\t";

    uint32_t display_col = 1;
    if (err.column > 0)
    {
        uint32_t target = err.column;
        uint32_t idx = 0;
        while (idx < line_content.size() && idx < target - 1)
        {
            if (line_content[idx] == '\t')
                display_col += 4;
            else
                display_col += 1;
            ++idx;
        }
    }

    for (uint32_t i = 1; i < display_col; ++i)
        std::cout << " ";

    const uint32_t len = (err.length == 0 ? 1u : err.length);
    std::cout << AnsiColors::Bold << "";

    std::cout << "^";
    if (len > 1)
    {
        for (uint32_t i = 1; i < len; ++i)
            std::cout << "~";
    }

    std::cout << AnsiColors::Reset << "\n\n";
}


void Linter::display_diagnostics() const
{
    if (errors.empty())
    {
        std::cout << "[Linter] No errors found\n";
        return;
    }

    for (const auto& err : errors)
    {
        auto [label, color] = severity_label_and_color(err.severity);
        std::filesystem::path relativePath = make_relative_path(err.file);

        print_header_and_location(err, relativePath, color, label.c_str());

        const auto& lines = load_file_lines(err.file);
        if (lines.empty())
        {
            std::cout << "\t| (unable to open file: " << relativePath.string() << ")\n\n";
            continue;
        }

        std::cout << color;
        print_code_line_with_caret(err, lines);
        std::cout << AnsiColors::Reset;
    }
}
