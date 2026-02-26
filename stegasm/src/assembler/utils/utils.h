//
// Created by Roumite on 26/02/2026.
//

#pragma once

#include <span>
#include <vector>
#include <string>

namespace assembler
{
    struct ParsedLine;

    std::span<const ParsedLine> get_section_lines(
        const std::vector<ParsedLine>& lines,
        const std::string& sectionName,
        bool throwIfNotFound = false
    );
}
