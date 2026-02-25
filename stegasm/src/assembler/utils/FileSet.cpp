//
// Created by Roumite on 23/02/2026.
//

#include "assembler/assembler_exception.h"
#include "FileSet.h"

#include <filesystem>
#include <fstream>

void assembler::FileSet::push_file(const std::string& user_name, const std::string& path)
{
    File new_file = {user_name, path, current_descriptor++};

    if (not std::filesystem::exists(path))
        throw AssemblerError("File \"" + path + "\" not found !");

    std::ifstream file_stream(path, std::ios::binary);
    int byte = 0;
    while ((byte = file_stream.get()) != EOF)
    {
        new_file.file_data.write_uint8(byte);
    }
    this->push_back(new_file);
}