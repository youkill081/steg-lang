//
// Created by Roumite on 23/02/2026.
//

#include "assembler/assembler_exception.h"
#include "FileSet.h"
#include "utils.h"
#include "assembler/Assembler.h"
#include "assembler/TextParser.h"

#include <filesystem>
#include <fstream>

using namespace assembler;

void FileSet::push_file(
    const std::string &user_name,
    const std::string &path,
    const std::string &extension)
{
    File new_file = {user_name, path, extension, current_descriptor++};
    if (not std::filesystem::exists(path))
        throw AssemblerError("File \"" + path + "\" not found !");
    std::ifstream file_stream(path, std::ios::binary);
    int byte = 0;
    while ((byte = file_stream.get()) != EOF)
        new_file.file_data.write_uint8(byte);
    this->push_back(new_file);
}

void FileSet::push_file_from_parsed_line(const ParsedLine &line)
{
    if (line.tokens.size() != 2)
        throw AssemblerError("Invalid file declaration: " + line.original_line);
    push_file(
        line.tokens[0],
        line.tokens[1],
        std::filesystem::path(line.tokens[1]).extension().string().substr(1)
    );
}

FileSet FileSet::from_parsed_lines(const std::vector<ParsedLine> &lines)
{
    const auto files_lines = get_section_lines(lines, FILES_SECTION_NAME);
    if (files_lines.empty())
        return {};

    FileSet files{};
    for (const auto &line : files_lines)
        files.push_file_from_parsed_line(line);
    return files;
}