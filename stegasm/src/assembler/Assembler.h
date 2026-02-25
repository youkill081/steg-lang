//
// Created by Roumite on 20/02/2026.
//

#pragma once

#include <filesystem>
#include <vector>
#include <span>
#include <map>

#include "../instructions.h"
#include "ByteBuffer.h"
#include "TextParser.h"
#include "utils/FileSet.h"
#include "utils/InstructionSet.h"

#include "utils/VariableSet.h"

#define SECTION_KEYWORD "section"
#define VARIABLE_SECTION_NAME ".data"
#define FILES_SECTION_NAME ".files"
#define INSTRUCTION_SECTION_NAME ".text"

namespace assembler
{
    using LabelMap = std::map<std::string, uint16_t>; // Map label's name with its associated code line
    struct CompiledFile
    {
        FileSet files;
        VariableSet variables;
        InstructionSet instructions;
    };

    class Assembler
    {
    private:
        static std::span<const ParsedLine> get_section_lines(
            const std::vector<ParsedLine> &lines,
            const std::string& sectionName,
            bool throwIfNotFound = false
        );

        // Files
        static void parsed_line_to_file(const ParsedLine &line, FileSet &files);
        static FileSet parse_files(const std::vector<ParsedLine> &lines);

        // Variables
        static std::vector<uint16_t> token_to_data(const std::string &token, const std::string &variableName);
        static Variable parsed_line_to_variable(const ParsedLine &line);
        static VariableSet parse_variables(const std::vector<ParsedLine> &line);

        // Instructions
        static const InstructionDesc &get_instruction_desc_from_parsed_line(const ParsedLine &line);
        static RegNames string_to_reg_name(const std::string &reg_name);
        static UsedRegistries get_used_registries_from_parsed_line(const InstructionDesc &desc, const ParsedLine &line);
        static std::string user_variable_write_as_address_to_string(const std::string &token);
        static bool user_write_value_in_bracket(const std::string &token);
        static uint16_t token_to_uint16(const std::string &token);
        static bool token_is_valid_value(const std::string &token);
        static DataValueParsingResult parse_data_value(std::string token, const VariableSet &variables, const LabelMap &labels);
        static DataValues get_data_values_from_parsed_line(const InstructionDesc &desc, const ParsedLine &line, const LabelMap &labels, const VariableSet &variables);
        static Instruction parsed_line_to_instruction(const ParsedLine &line, const VariableSet &variables, const LabelMap &labels);
        static InstructionSet parse_instructions(const std::vector<ParsedLine> &lines, const VariableSet &variables);

        // Labels
        static bool is_label(const ParsedLine &line);
        static LabelMap parse_labels(const std::span<const ParsedLine> &lines, const VariableSet &variables);

        static CompiledFile compile_file(const std::string &path);

        static void write_reg_x_in_buffer(uint8_t reg_x, const RegCount &reg_count, const UsedRegistries &registries, ByteBuffer &buffer);
        static void write_datas_flag_in_buffer(const DataCount &data_count, const DataValues &data_parsing_result, ByteBuffer &buffer);
        static ByteBuffer compiled_file_to_bytebuffer(CompiledFile &compiledFile);
    public:
        static ByteBuffer assemble(const std::string &path);
    };
}
