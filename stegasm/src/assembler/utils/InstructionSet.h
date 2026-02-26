//
// Created by Roumite on 23/02/2026.
//

#pragma once

#include <vector>
#include <array>
#include <map>
#include <span>

#include "FileSet.h"
#include "instructions.h"
#include "VariableSet.h"

namespace assembler
{
    struct ParsedLine;

    using LabelMap = std::map<std::string, uint16_t>;
    using UsedRegistries = std::array<RegNames, 8>;

    struct DataValueParsingResult
    {
        bool is_address;
        uint16_t value;
    };
    using DataValues = std::array<DataValueParsingResult, 2>;

    struct Instruction
    {
        const InstructionDesc &desc;
        UsedRegistries registries;
        DataValues datas;
    };

    class InstructionSet : public std::vector<Instruction>
    {
    private:
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
        static bool is_label(const ParsedLine &line);
        static LabelMap parse_labels(const std::span<const ParsedLine> &lines, const VariableSet &variables);
    public:
        void display() const;
        static InstructionSet from_parsed_lines(const std::vector<ParsedLine> &lines, const VariableSet &variables, const FileSet &files);
    };
}
