//
// Created by Roumite on 26/02/2026.
//

#include "SubtexturesSet.h"

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

void assembler::SubtexturesSet::push_subtexture_from_parsed_line(const ParsedLine &line,  const FileSet &files)
{
    if (line.tokens.size() != 6)
        throw AssemblerError("Invalid subtexture declaration: " + line.original_line);

    if (not token_is_uint16_value(line.tokens[2]) ||
        not token_is_uint16_value(line.tokens[3]) ||
        not token_is_uint16_value(line.tokens[4]) ||
        not token_is_uint16_value(line.tokens[5]))
        Linter::error("x, y, w, h must be integer in subtexture declaration !");

    const File &file = get_file(line.tokens[1], files);
    const uint16_t x = token_to_uint16(line.tokens[2]);
    const uint16_t y = token_to_uint16(line.tokens[3]);
    const uint16_t w = token_to_uint16(line.tokens[4]);
    const uint16_t h = token_to_uint16(line.tokens[5]);
    this->push_back({
        .origin_file = file,
        .name = line.tokens[0],
        .descriptor = files.get_next_descriptor(),
        .x = x,
        .y = y,
        .width = w,
        .height = h
    });
}

assembler::SubtexturesSet assembler::SubtexturesSet::from_parsed_lines(
    const std::vector<ParsedLine> &lines,
    const FileSet &files,
    Linter &linter
) {
    const auto files_lines = get_section_lines(lines, SUBTEXTURES_SECTION_NAME);
    if (files_lines.empty())
        return {};

    SubtexturesSet textures{};
    linter.foreach(files_lines, [&](const ParsedLine &line)
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


