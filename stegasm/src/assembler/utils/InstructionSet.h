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

    struct RegistryParsingResult
    {
        RegTypes type = REG_NO;
        RegNames registry = R0;
    };

    using UsedRegistries = std::array<RegistryParsingResult, 3>;

    struct DataValueParsingResult
    {
        DataCount data_count = NO_DATA;
        uint16_t value = 0;
        bool is_address = false;
    };

    struct InstructionParameters
    {
        UsedRegistries registries{};
        DataValueParsingResult data_value{};
    };

    struct Instruction
    {
        const InstructionDesc &desc;
        HandlerNumber handler_number = HANDLER_0;

        InstructionParameters instruction_parameters{};
    };

    class InstructionSet : private std::vector<Instruction>
    {
        using Base = std::vector<Instruction>;
    private:
        static const InstructionDesc &get_instruction_desc_from_name(const std::string& name);
        static RegNames string_to_reg_name(const std::string &reg_name);
        static bool is_registry(const std::string &reg_name);
        static std::string remove_brackets(const std::string &token);
        static bool token_is_in_brackets(const std::string &token);
        static InstructionParameters parse_data_and_registries_from_line(const ParsedLine& line, const SymbolSet& symbols);
        static HandlerNumber get_handler_number(const InstructionDesc& desc, const InstructionParameters& data_registries);
        static uint16_t parse_data_value(std::string token, const SymbolSet &symbols);
        static Instruction parsed_line_to_instruction(const ParsedLine &line, const SymbolSet &symbols);
        static Instruction get_eof_instruction();
    public:
        void display() const;

        static InstructionSet from_parsed_lines(const std::vector<ParsedLine> &lines, const SymbolSet &symbols, Linter &);
        static uint64_t count_text_lines(const std::vector<ParsedLine> &lines);

        void merge(const InstructionSet& other, Linter &linter);

        using Base::begin;
        using Base::end;
        using Base::size;
        using Base::empty;
    };
}
