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
#include <utils.h>

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

uint32_t InstructionSet::parse_data_value(std::string token, const SymbolSet &symbols)
{
    if (token_is_numeric_value(token))
        return token_to_numeric(token);

    if (!symbols.contains(token))
        Linter::error("Unknown symbol \"" + token + "\"");

    return symbols.at(token).value;
}

bool InstructionSet::is_registry(const std::string& reg_name)
{
    return stringToRegistry.contains(reg_name);
}

InstructionParameters InstructionSet::parse_data_and_registries_from_line(const ParsedLine &line, const SymbolSet &symbols)
{
    InstructionParameters datas_registries{};

    if (line.tokens.size() > 4)
        Linter::error("Too much token for instruction");
    for (int i = 1; i <= line.tokens.size() - 1; i++)
    {
        std::string token = line.tokens[i];
        const bool is_address = token_is_in_brackets(token);
        if (is_address)
            token = remove_brackets(token);

        if (is_registry(token))
        {
            RegTypes reg_type = is_address ? REG_ADDRESS : REG;
            datas_registries.registries[i - 1] = {.type = reg_type, .registry = string_to_reg_name(token)};
        } else
        {
            if (i != line.tokens.size() - 1)
                Linter::error("Misplaced data !");
            datas_registries.data_value = {
                .data_count = ONE_DATA,
                .value = parse_data_value(token, symbols),
                .is_address = is_address
            };
        }
    }

    return datas_registries;
}

HandlerNumber InstructionSet::get_handler_number(const InstructionDesc &desc, const InstructionParameters &data_registries)
{
    auto reg_matches = [](RegTypes handler_type, RegTypes actual_type) -> bool
    {
        if (handler_type == REG_BOTH)
            return actual_type == REG || actual_type == REG_ADDRESS;
        return handler_type == actual_type;
    };

    for (std::size_t i = 0; i < desc.handler_count; i++)
    {
        if (reg_matches(desc.handlers[i].first_reg_type,  data_registries.registries[0].type) &&
            reg_matches(desc.handlers[i].second_reg_type, data_registries.registries[1].type) &&
            reg_matches(desc.handlers[i].third_reg_type,  data_registries.registries[2].type) &&
            desc.handlers[i].data_type == data_registries.data_value.data_count
        )
        {
            return static_cast<HandlerNumber>(i);
        }
    }
    Linter::error("No compatible handler found for given instruction");
}

Instruction InstructionSet::parsed_line_to_instruction(const ParsedLine &line, const SymbolSet &symbols)
{
    const InstructionDesc &desc = get_instruction_desc_from_name(line.tokens[0]);
    if (not desc.user_usable)
        Linter::error("Try to use assemblers instruction in code !");

    InstructionParameters data_registries = parse_data_and_registries_from_line(line, symbols);
    HandlerNumber handler_number = get_handler_number(desc, data_registries);
    return {
        .desc = desc,
        .handler_number = handler_number,
        .instruction_parameters = data_registries,
    };
}

Instruction InstructionSet::get_eof_instruction()
{
    return Instruction{
        .desc = get_instruction_desc_from_name("EOF"),
        .handler_number = HANDLER_0,
        .instruction_parameters = {},
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