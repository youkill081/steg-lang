//
// Created by Roumite on 23/02/2026.
//

#include "InstructionSet.h"
#include "assembler/assembler_exception.h"
#include "utils/utils.hpp"
#include "assembler/TextParser.h"
#include "utils.h"
#include "assembler/Assembler.h"

#include <iostream>

using namespace assembler;

const InstructionDesc& InstructionSet::get_instruction_desc_from_name(const std::string& name)
{
    for (auto &instruction : instructionSet)
        if (isEqual(name, instruction.name))
            return instruction;
    Linter::error("Unknown instruction \"" + name + "\"");
}

RegNames InstructionSet::string_to_reg_name(const std::string &reg_name)
{
    if (not stringToRegistry.contains(reg_name))
        Linter::error("Unknown register \"" + reg_name + "\"");
    return stringToRegistry.at(reg_name);
}

UsedRegistries InstructionSet::get_used_registries_from_parsed_line(const InstructionDesc &desc, const ParsedLine &line)
{
    UsedRegistries registries;
    for (uint32_t i = 1; i < static_cast<uint32_t>(desc.regCount) + 1; i++)
        registries[i - 1] = string_to_reg_name(line.tokens[i]);
    return registries;
}

std::string InstructionSet::remove_brackets(const std::string &token)
{
    if (not token_is_in_brackets(token))
        Linter::error("Invalid variable address \"" + token + "\"");
    return token.substr(1, token.size() - 2);
}

bool InstructionSet::token_is_in_brackets(const std::string &token)
{
    return token[0] == '[' && token[token.size() - 1] == ']';
}

DataValueParsingResult InstructionSet::parse_data_value(std::string token, const SymbolSet &symbols)
{
    bool is_in_bracket = token_is_in_brackets(token);
    if (is_in_bracket)
        token = remove_brackets(token);

    if (token_is_uint16_value(token))
        return { .is_address = is_in_bracket, .value = token_to_uint16(token) };

    if (!symbols.contains(token))
        Linter::error("Unknown symbol \"" + token + "\"");

    return { .is_address = is_in_bracket, .value = symbols.at(token).value };
}

DataValues InstructionSet::get_data_values_from_parsed_line(
    const InstructionDesc &desc,
    const ParsedLine &line,
    const SymbolSet &symbols)
{
    DataValues data{};
    switch (desc.dataCount)
    {
    case ONE_DATA:
        data[0] = parse_data_value(line.tokens[desc.regCount + 1], symbols);
    default:
        return data;
    }
}

Instruction InstructionSet::parsed_line_to_instruction(const ParsedLine &line, const SymbolSet &symbols)
{
    const InstructionDesc &desc = get_instruction_desc_from_name(line.tokens[0]);
    uint32_t token_needed = static_cast<uint32_t>(desc.dataCount) + static_cast<uint32_t>(desc.regCount) + 1;

    if (not desc.user_usable)
        Linter::error("Try to use assemblers instruction in code !");
    if (line.tokens.size() != token_needed)
        Linter::error("Number of token missmatch for instruction");

    return {
        .desc = desc,
        .registries = get_used_registries_from_parsed_line(desc, line),
        .datas = get_data_values_from_parsed_line(desc, line, symbols)
    };
}

Instruction InstructionSet::get_eof_instruction()
{
    return Instruction{
        .desc = get_instruction_desc_from_name("EOF"),
        .registries = {},
        .datas = {}
    };
}

InstructionSet InstructionSet::from_parsed_lines(
    const std::vector<ParsedLine> &lines,
    const SymbolSet &symbols,
    Linter &linter
) {
    InstructionSet instructions;
    auto instructions_lines = get_section_lines(lines, INSTRUCTION_SECTION_NAME);

    linter.foreach_lines(instructions_lines, [&](const ParsedLine &line)
    {
        if (LabelSet::is_label(line))
            return;
        instructions.push_back(parsed_line_to_instruction(line, symbols));
    });
    instructions.push_back(get_eof_instruction()); // Add EOF instruction as last instructions of file
    return instructions;
}

uint64_t InstructionSet::count_text_lines(const std::vector<ParsedLine>& lines)
{
    uint64_t instruction_count = 0;
    auto instructions_lines = get_section_lines(lines, INSTRUCTION_SECTION_NAME);

    for (const auto &instruction_line: instructions_lines)
        instruction_count += not LabelSet::is_label(instruction_line);
    instruction_count += 1; // Count added EOF
    return instruction_count;
}

void InstructionSet::merge(const InstructionSet& other, Linter& linter)
{
    linter.foreach(other, [&](const Instruction &instruction){
        this->push_back(instruction);
    });
}


void InstructionSet::display() const
{
    for (const auto &instruction : *this)
    {
        std::cout << instruction.desc.name << " registrie(s) -> ";
        for (int i = 0; i < instruction.desc.regCount; i++)
            std::cout << registryToString.at(instruction.registries[i]) << " ";
        std::cout << "data(s) -> ";
        for (int i = 0; i < instruction.desc.dataCount; i++)
            std::cout << "\"" << instruction.datas[i].value << "\"" << (instruction.datas[i].is_address ? " as address " : " as constant ");
        std::cout << std::endl;
    }
}
