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

const InstructionDesc &InstructionSet::get_instruction_desc_from_parsed_line(const ParsedLine &line)
{
    std::string instruction_name = line.tokens[0];
    for (auto &instruction : instructionSet)
        if (isEqual(instruction_name, instruction.name))
            return instruction;
    throw AssemblerError("Unknown instruction \"" + instruction_name + "\"");
}

RegNames InstructionSet::string_to_reg_name(const std::string &reg_name)
{
    if (not stringToRegistry.contains(reg_name))
        throw AssemblerError("Unknown register \"" + reg_name + "\"");
    return stringToRegistry.at(reg_name);
}

UsedRegistries InstructionSet::get_used_registries_from_parsed_line(const InstructionDesc &desc, const ParsedLine &line)
{
    UsedRegistries registries;
    for (uint32_t i = 1; i < static_cast<uint32_t>(desc.regCount) + 1; i++)
        registries[i - 1] = string_to_reg_name(line.tokens[i]);
    return registries;
}

std::string InstructionSet::user_variable_write_as_address_to_string(const std::string &token)
{
    if (not user_write_value_in_bracket(token))
        throw AssemblerError("[userVariableWriteAsAddressToString] Invalid variable address \"" + token + "\"");
    return token.substr(1, token.size() - 2);
}

bool InstructionSet::user_write_value_in_bracket(const std::string &token)
{
    return token[0] == '[' && token[token.size() - 1] == ']';
}

uint16_t InstructionSet::token_to_uint16(const std::string &token)
{
    if (token_is_valid_value(token))
    {
        uint16_t result;
        std::from_chars(token.data(), token.data() + token.size(), result);
        return result;
    }
    throw AssemblerError("Invalid uint16 value \"" + token + "\"");
}

bool InstructionSet::token_is_valid_value(const std::string &token)
{
    uint16_t result;
    auto [ptr, ec] = std::from_chars(token.data(), token.data() + token.size(), result);
    return ec == std::errc();
}

DataValueParsingResult InstructionSet::parse_data_value(std::string token, const VariableSet &variables, const LabelMap &labels)
{
    bool is_in_bracket = user_write_value_in_bracket(token);
    if (is_in_bracket)
        token = user_variable_write_as_address_to_string(token);

    if (token_is_valid_value(token))
        return { .is_address = is_in_bracket, .value = token_to_uint16(token) };

    if (labels.contains(token))
        return { .is_address = false, .value = labels.at(token) };

    return {
        .is_address = is_in_bracket,
        .value = variables.get_variable_address(variables.get_variable_by_name(token))
    };
}

DataValues InstructionSet::get_data_values_from_parsed_line(
    const InstructionDesc &desc,
    const ParsedLine &line,
    const LabelMap &labels,
    const VariableSet &variables)
{
    DataValues data{};
    switch (desc.dataCount)
    {
    case TWO_DATA:
        data[1] = parse_data_value(line.tokens[desc.regCount + 2], variables, labels);
    case ONE_DATA:
        data[0] = parse_data_value(line.tokens[desc.regCount + 1], variables, labels);
    default:
        return data;
    }
}

Instruction InstructionSet::parsed_line_to_instruction(const ParsedLine &line, const VariableSet &variables, const LabelMap &labels)
{
    const InstructionDesc &desc = get_instruction_desc_from_parsed_line(line);
    uint32_t token_needed = static_cast<uint32_t>(desc.dataCount) + static_cast<uint32_t>(desc.regCount) + 1;

    if (line.tokens.size() != token_needed)
        throw AssemblerError("Number of token missmatch for instruction \"" + line.original_line + "\"");

    return {
        .desc = desc,
        .registries = get_used_registries_from_parsed_line(desc, line),
        .datas = get_data_values_from_parsed_line(desc, line, labels, variables)
    };
}

bool InstructionSet::is_label(const ParsedLine &line)
{
    return line.tokens.size() == 1 && line.tokens[0].ends_with(':');
}

LabelMap InstructionSet::parse_labels(const std::span<const ParsedLine> &lines, const VariableSet &variables)
{
    LabelMap labels{};
    uint64_t current_instruction_idx = 0;

    for (auto &line : lines)
    {
        if (!is_label(line))
        {
            line.line_number_in_section = current_instruction_idx++;
            line.is_instruction = true;
        }
        else
        {
            line.is_instruction = false;
            std::string label_name = line.tokens[0].substr(0, line.tokens[0].size() - 1);
            if (variables.contains_variable_by_name(label_name))
                throw AssemblerError("Label \"" + label_name + "\" is already used as variable name");
            if (labels.contains(label_name))
                throw AssemblerError("Label \"" + label_name + "\" is already used");
            labels[label_name] = current_instruction_idx;
        }
    }
    return labels;
}

InstructionSet InstructionSet::from_parsed_lines(
    const std::vector<ParsedLine> &lines,
    const VariableSet &variables,
    const FileSet &files)
{
    auto instructions_lines = get_section_lines(lines, INSTRUCTION_SECTION_NAME, true);
    if (instructions_lines.empty())
        throw AssemblerError("No instructions in .text section !");

    LabelMap labels = parse_labels(instructions_lines, variables);
    for (const auto &file : files) // Handle files like labels
    {
        if (labels.contains(file.user_name))
            throw AssemblerError("File name collide with label \"" + file.user_name + "\"");
        labels[file.user_name] = file.descriptor;
    }

    InstructionSet instructions;
    for (const auto &line : instructions_lines)
    {
        if (not line.is_instruction)
            continue;
        instructions.push_back(parsed_line_to_instruction(line, variables, labels));
    }
    return instructions;
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