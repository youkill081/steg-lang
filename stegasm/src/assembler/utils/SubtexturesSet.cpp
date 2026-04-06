//
// Created by Roumite on 26/02/2026.
//

#include "SubtexturesSet.h"

#include <utils.h>

#include "utils.h"
#include "assembler/Assembler.h"
#include "assembler/assembler_exception.h"

const assembler::File& assembler::SubtexturesSet::get_file(const std::string& name, const FileSet& files)
{
    for (const auto &file : files)
    {
        if (file.user_name == name)
            return file;
    }
    Linter::error("File \"" + name + "\" not found !");
}

void assembler::SubtexturesSet::push_subtexture_from_parsed_line(const ParsedLine &line, FileSet &files)
{
    if (line.tokens.size() != 6)
        throw AssemblerError("Invalid subtexture declaration: " + line.original_line);

    if (not token_is_numeric_value(line.tokens[2]) ||
        not token_is_numeric_value(line.tokens[3]) ||
        not token_is_numeric_value(line.tokens[4]) ||
        not token_is_numeric_value(line.tokens[5]))
        Linter::error("x, y, w, h must be integer in subtexture declaration !");

    const File &file = get_file(line.tokens[1], files);
    const uint16_t x = token_to_numeric(line.tokens[2]);
    const uint16_t y = token_to_numeric(line.tokens[3]);
    const uint16_t w = token_to_numeric(line.tokens[4]);
    const uint16_t h = token_to_numeric(line.tokens[5]);

    Subtexture new_subtexture = {
        .origin_file = file,
        .name = line.tokens[0],
        .descriptor = FileSet::get_next_descriptor(),
        .x = x,
        .y = y,
        .width = w,
        .height = h
    };
    if (this->contains(new_subtexture))
        Linter::error("Duplicate subtexture \"" + new_subtexture.name + "\" !");
    this->insert(new_subtexture);
}

assembler::SubtexturesSet assembler::SubtexturesSet::from_parsed_lines(
    const std::vector<ParsedLine> &lines,
    FileSet &files,
    Linter &linter
) {
    const auto files_lines = get_section_lines(lines, SUBTEXTURES_SECTION_NAME);
    if (files_lines.empty())
        return {};

    SubtexturesSet textures{};
    linter.foreach_lines(files_lines, [&](const ParsedLine &line)
    {
        textures.push_subtexture_from_parsed_line(line, files);
    });
    return textures;
}

assembler::SymbolSet assembler::SubtexturesSet::get_symbols() const
{
    SymbolSet symbols;
    for (const auto &sub : *this)
        symbols.insert_symbol(sub.name, sub.descriptor, SymbolType::Subtexture);
    return symbols;
}

void assembler::SubtexturesSet::merge(const SubtexturesSet& other, Linter& linter)
{
    linter.foreach(other, [&](const Subtexture& sub)
    {
        if (this->contains(sub))
            Linter::error("Duplicate subtexture \"" + sub.name + "\" !");
        this->insert(sub);
    });
}


