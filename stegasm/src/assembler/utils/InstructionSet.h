//
// Created by Roumite on 23/02/2026.
//

#pragma once

#include <vector>
#include <array>
#include <span>

#include "instructions.h"
#include "assembler/Linter.h"
#include "assembler/Symbol.h"

namespace assembler
{
    struct ParsedLine;

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

    class InstructionSet : private std::vector<Instruction>
    {
        using Base = std::vector<Instruction>;
    private:
        static const InstructionDesc &get_instruction_desc_from_name(const std::string& name);
        static RegNames string_to_reg_name(const std::string &reg_name);
        static UsedRegistries get_used_registries_from_parsed_line(const InstructionDesc &desc, const ParsedLine &line);
        static std::string remove_brackets(const std::string &token);
        static bool token_is_in_brackets(const std::string &token);
        static DataValueParsingResult parse_data_value(std::string token, const SymbolSet &symbols);
        static DataValues get_data_values_from_parsed_line(const InstructionDesc &desc, const ParsedLine &line, const SymbolSet &symbols);
        static Instruction parsed_line_to_instruction(const ParsedLine &line, const SymbolSet &symbols);
        static Instruction get_eof_instruction();
    public:
        void display() const;

        static InstructionSet from_parsed_lines(const std::vector<ParsedLine> &lines, const SymbolSet &symbols, Linter &);

        using Base::begin;
        using Base::end;
        using Base::size;
        using Base::empty;
    };
}
