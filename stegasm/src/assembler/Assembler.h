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
#include "Linter.h"
#include "utils/LabelSet.h"

#define SECTION_KEYWORD "section"
#define VARIABLE_SECTION_NAME ".data"
#define FILES_SECTION_NAME ".files"
#define IMPORT_SECTION_NAME ".import"
#define SUBTEXTURES_SECTION_NAME ".subtextures"
#define INSTRUCTION_SECTION_NAME ".text"

namespace assembler
{
    struct CompiledFile
    {
        FileSet files{};
        VariableSet variables{};
        SubtexturesSet subtextures{};
        LabelSet labels{};

        SymbolSet symbols{};
        InstructionSet instructions{};

        void merge(const CompiledFile &other, Linter &linter);
    };

    class Assembler
    {
    private:
        static void write_reg_x_in_buffer(uint8_t reg_x, const RegCount &reg_count, const UsedRegistries &registries, ByteBuffer &buffer);
        // static void write_datas_flag_in_buffer(const DataCount &data_count, const DataValues &data_parsing_result, ByteBuffer &buffer);
        static ByteBuffer compiled_file_to_bytebuffer(CompiledFile &compiledFile, Linter &linter);
        static void compile_symbols(CompiledFile &compiled_file, const std::vector<ParsedLine> &lines, Linter &linter);

        static void collect_labels(CompiledFile& compiled_file, const std::vector<ParsedLine>& lines, Linter& linter, uint64_t& instruction_counter);
        static void emit_instructions(CompiledFile &compiled_file, const std::vector<ParsedLine> &lines, Linter &linter);

        static void compile_instructions(CompiledFile &compiled_file, const std::vector<ParsedLine> &lines, Linter &linter);

        static void compile_file(CompiledFile &compiled_file, TextParser &parser, Linter &linter);
    public:
        static ByteBuffer assemble(const std::string &path, Linter &linter, bool throw_if_error = true);
        static ByteBuffer assemble_from_text(const std::string &text_data, Linter &linter, bool throw_if_error = true);
    };
}