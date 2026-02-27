//
// Created by Roumite on 20/02/2026.
//

#pragma once

#include <filesystem>
#include "ByteBuffer.h"
#include "utils/FileSet.h"
#include "utils/InstructionSet.h"
#include "utils/SubtexturesSet.h"
#include "utils/VariableSet.h"

#define SECTION_KEYWORD "section"
#define VARIABLE_SECTION_NAME ".data"
#define FILES_SECTION_NAME ".files"
#define SUBTEXTURES_SECTION_NAME ".subtextures"
#define INSTRUCTION_SECTION_NAME ".text"

namespace assembler
{
    struct CompiledFile
    {
        FileSet files;
        VariableSet variables;
        InstructionSet instructions;
        SubtexturesSet subtextures;
    };

    class Assembler
    {
    private:
        static void write_reg_x_in_buffer(uint8_t reg_x, const RegCount &reg_count, const UsedRegistries &registries, ByteBuffer &buffer);
        static void write_datas_flag_in_buffer(const DataCount &data_count, const DataValues &data_parsing_result, ByteBuffer &buffer);
        static ByteBuffer compiled_file_to_bytebuffer(CompiledFile &compiledFile);
        static CompiledFile compile_file(const std::string &path);
    public:
        static ByteBuffer assemble(const std::string &path);
    };
}