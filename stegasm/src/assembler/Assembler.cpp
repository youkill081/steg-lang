//
// Created by Roumite on 20/02/2026.
//

#include "Assembler.h"

#include <charconv>

#include "assembler_exception.h"
#include "utils/utils.hpp"

using namespace assembler;

std::span<const ParsedLine> Assembler::get_section_lines(
    const std::vector<ParsedLine>& lines,
    const std::string &sectionName,
    bool throwIfNotFound
) {
    bool is_in_section = false;
    uint32_t section_start_index = 0, number_line_section = 0, current_index = 0;

    for (const auto &line : lines)
    {
        if (not is_in_section &&
            line.tokens[0] == SECTION_KEYWORD &&
            line.tokens.size() >= 2 &&
            line.tokens[1] == sectionName)
        {
            if (current_index == lines.size() - 1)
                return {};
            is_in_section = true;
            section_start_index = current_index;
        } else if (is_in_section)
        {
            if (line.tokens[0] == SECTION_KEYWORD)
            {
                return {lines.data() + section_start_index + 1, number_line_section};
            }
            line.line_number_in_section = 1;
            number_line_section += 1;
        }

        current_index += 1;
    }

    if (number_line_section != 0)
        return {lines.data() + section_start_index + 1, number_line_section};

    if (throwIfNotFound)
        throw AssemblerError("Mandatory section \"" + sectionName + "\" not found");
    return {};
}

void Assembler::parsed_line_to_file(const ParsedLine& line, FileSet& files)
{
    if (line.tokens.size() != 2)
        throw AssemblerError("Invalid file declaration: " + line.original_line);
    files.push_file(line.tokens[0], line.tokens[1]);
}

FileSet Assembler::parse_files(const std::vector<ParsedLine>& lines)
{
    const auto files_lines = get_section_lines(lines, FILES_SECTION_NAME);
    if (files_lines.empty())
        return {};

    FileSet files{};
    for (const auto &line : files_lines)
        parsed_line_to_file(line, files);
    return files;
}

std::vector<uint16_t> Assembler::token_to_data(const std::string& token, const std::string &variableName)
{
    uint16_t result;
    auto [ptr, ec] = std::from_chars(token.data(), token.data() + token.size(), result);

    if (ec == std::errc())
        return {result};
    if (ec == std::errc::invalid_argument)
    {
        std::vector<uint16_t> end_vector;
        for (const auto &c : token)
            end_vector.push_back(static_cast<uint16_t>(c));
        return end_vector;
    }

    if (ec == std::errc::result_out_of_range)
    {
        throw AssemblerError("Value \"" + token + "\" for variable \"" + variableName + "\" is out of range");
    }
    throw AssemblerError("Failed to parse token \"" + token + "\" for variable \"" + variableName + "\"");
}

Variable Assembler::parsed_line_to_variable(const ParsedLine& line)
{
    if (line.tokens.size() < 2)
        throw AssemblerError("Invalid variable declaration: " + line.original_line);

    std::vector<uint16_t> data;
    std::string variable_name = line.tokens[0];
    for (int i = 1; i < line.tokens.size(); i++)
    {
        auto token_data = token_to_data(line.tokens[i], variable_name);
        data.insert(data.end(), token_data.begin(), token_data.end());
    }

    data.push_back('\0'); // Add null terminaison to all variables

    return {
        .name = variable_name,
        .value = data
    };
}

VariableSet Assembler::parse_variables(const std::vector<ParsedLine> &lines)
{
    const auto variables_lines = get_section_lines(lines, VARIABLE_SECTION_NAME);
    if (variables_lines.empty())
    {
        return {};
    }

    VariableSet variables;
    for (const auto &line : variables_lines)
        variables.push_variable(parsed_line_to_variable(line));
    return variables;
}

const InstructionDesc &Assembler::get_instruction_desc_from_parsed_line(const ParsedLine& line)
{
    std::string instruction_name = line.tokens[0];
    for (auto &instruction : instructionSet)
    {
        if (isEqual(instruction_name, instruction.name))
            return instruction;
    }

    throw AssemblerError("Unknown instruction \"" + instruction_name + "\"");
}

RegNames Assembler::string_to_reg_name(const std::string& reg_name)
{
    if (not stringToRegistry.contains(reg_name))
        throw AssemblerError("Unknown register \"" + reg_name + "\"");
    return stringToRegistry.at(reg_name);
}

UsedRegistries Assembler::get_used_registries_from_parsed_line(const InstructionDesc &desc, const ParsedLine& line)
{
    UsedRegistries registries;
    for (uint32_t i = 1; i < static_cast<uint32_t>(desc.regCount) + 1; i++)
        registries[i - 1] = string_to_reg_name(line.tokens[i]);
    return registries;
}

/*
 * Basically this function remove brackets from strings ([variable] -> variable)
 */
std::string Assembler::user_variable_write_as_address_to_string(const std::string& token)
{
    if (not user_write_value_in_bracket(token))
        throw AssemblerError("[userVariableWriteAsAddressToString] Invalid variable address \"" + token + "\"");
    return token.substr(1, token.size() - 2);
}

bool Assembler::user_write_value_in_bracket(const std::string& token)
{
    return token[0] == '[' && token[token.size() - 1] == ']';
}

uint16_t Assembler::token_to_uint16(const std::string& token)
{
    if (token_is_valid_value(token))
    {
        uint16_t result;
        std::from_chars(token.data(), token.data() + token.size(), result);
        return result;
    }
    throw AssemblerError("Invalid uint16 value \"" + token + "\"");
}

bool Assembler::token_is_valid_value(const std::string& token)
{
    uint16_t result;
    auto [ptr, ec] = std::from_chars(token.data(), token.data() + token.size(), result);
    return ec == std::errc();
}

DataValueParsingResult Assembler::parse_data_value(std::string token, const VariableSet &variables, const LabelMap &labels)
{
    bool is_in_bracket = user_write_value_in_bracket(token);
    if (is_in_bracket) // User hardcoded address ex : LOAD A [5]
        token = user_variable_write_as_address_to_string(token);

    if (token_is_valid_value(token)) // User hardcoded a constant ex : LOAD A 5
        return {
            .is_address = is_in_bracket,
            .value = token_to_uint16(token)
        };

    if (labels.contains(token)) // User used a label
    {
        return {
            .is_address = false, // Labels are transformed to constant
            .value = labels.at(token)
        };
    }
    return {
        .is_address = is_in_bracket,
        .value = variables.get_variable_address(variables.get_variable_by_name(token))
    };
}

DataValues Assembler::get_data_values_from_parsed_line(
    const InstructionDesc& desc,
    const ParsedLine& line,
    const LabelMap &labels,
    const VariableSet &variables)
{
    DataValues data{};
    switch (desc.dataCount)
    {
    case TWO_DATA:
        data[1] = parse_data_value(
            line.tokens[desc.regCount + 2],
            variables,
            labels
        );
    case ONE_DATA:
        data[0] = parse_data_value(
            line.tokens[desc.regCount + 1],
            variables,
            labels
        );
    default:
        return data;
    }
}

Instruction Assembler::parsed_line_to_instruction(const ParsedLine& line, const VariableSet& variables, const LabelMap &labels)
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

InstructionSet Assembler::parse_instructions(const std::vector<ParsedLine>& lines, const VariableSet& variables)
{
    auto instructions_lines = get_section_lines(lines, INSTRUCTION_SECTION_NAME, true);
    if (instructions_lines.empty())
        throw AssemblerError("No instructions in .text section !");

    LabelMap labels = parse_labels(instructions_lines, variables);
    InstructionSet instructions;
    for (const auto &line : instructions_lines)
    {
        if (not line.is_instruction)
            continue;
        instructions.push_back(parsed_line_to_instruction(line, variables, labels));
    }
    return instructions;
}

bool Assembler::is_label(const ParsedLine &line)
{
    return line.tokens.size() == 1 && line.tokens[0].ends_with(':');
}

LabelMap Assembler::parse_labels(const std::span<const ParsedLine> &lines, const VariableSet &variables)
{
    LabelMap labels{};
    uint64_t current_instruction_idx = 0;

    for (auto & line : lines)
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

CompiledFile Assembler::compile_file(const std::string& path)
{
    TextParser parser(path);

    try
    {
        const auto lines = parser.parse();

        FileSet files = parse_files(lines);
        VariableSet variables = parse_variables(lines);
        for (const auto &file : files)
            variables.push_variable({file.user_name, {file.descriptor}});

        InstructionSet instructions = parse_instructions(lines, variables);
        return {
            .files = files,
            .variables = variables,
            .instructions = instructions,
        };
    } catch (const TextParserError &error)
    {
        throw AssemblerError(error.what());
    }
}

void Assembler::write_reg_x_in_buffer(
    uint8_t reg_x,
    const RegCount& reg_count,
    const UsedRegistries& registries,
    ByteBuffer& buffer
) {
    if (reg_x <= static_cast<uint8_t>(reg_count))
    {
        const uint8_t reg = registries[reg_x - 1];
        const uint8_t write_reg = static_cast<uint8_t>(reg & 0b111);

        buffer.push_bit((write_reg >> 2) & 0x1);
        buffer.push_bit((write_reg >> 1) & 0x1);
        buffer.push_bit((write_reg >> 0) & 0x1);
    } else {
        buffer.push_bit(0);
        buffer.push_bit(0);
        buffer.push_bit(0);
    }
}

void Assembler::write_datas_flag_in_buffer(
    const DataCount& data_count,
    const DataValues& data_parsing_result,
    ByteBuffer& buffer)
{
    const bool first_bit_value = static_cast<uint8_t>(data_count) >= 1 && data_parsing_result[0].is_address;
    const bool second_bit_value = static_cast<uint8_t>(data_count) >= 2 && data_parsing_result[1].is_address;

    buffer.push_bit(first_bit_value);
    buffer.push_bit(second_bit_value);
}

ByteBuffer Assembler::compiled_file_to_bytebuffer(CompiledFile &compiledFile)
{
    ByteBuffer buffer;

    // Push Variables
    buffer.write_uint32(compiledFile.variables.size());
    for (const auto &variable : compiledFile.variables)
    {
        buffer.write_uint16(compiledFile.variables.get_variable_address(variable));
        buffer.write_uint16(variable.value.size());
        for (const auto &data : variable.value)
        {
            buffer.write_uint16(data);
        }
    }

    // Push Files
    buffer.write_uint32(compiledFile.files.size());
    for (auto &file : compiledFile.files)
    {
        file.file_data.reset_cursor();

        buffer.write_uint16(file.descriptor);
        buffer.write_uint32(file.file_data.remaining_uint8());
        while (file.file_data.remaining_uint8())
            buffer.write_uint8(file.file_data.read_uint8());
    }

    // Push Instructions
    for (const auto &instruction : compiledFile.instructions)
    {
        buffer.write_uint8(instruction.desc.opcode);

        // Registry And Flag
        write_reg_x_in_buffer(1, instruction.desc.regCount, instruction.registries, buffer); // RegX(1) | 3 bit
        write_datas_flag_in_buffer(instruction.desc.dataCount, instruction.datas, buffer); // Data Flag | 2 bit
        write_reg_x_in_buffer(2, instruction.desc.regCount, instruction.registries, buffer); // RegX(2) | 3 bit

        // 8 bit was wrote, ByteBuffer flushed

        buffer.write_uint16(instruction.datas[0].value);

        if (instruction.desc.dataCount == TWO_DATA || static_cast<uint8_t>(instruction.desc.regCount) > 2)
        {
            write_reg_x_in_buffer(3, instruction.desc.regCount, instruction.registries, buffer); // RegX(3) | 3 bit
            write_reg_x_in_buffer(4, instruction.desc.regCount, instruction.registries, buffer); // RegX(4) | 3 bit
            write_reg_x_in_buffer(5, instruction.desc.regCount, instruction.registries, buffer); // RegX(5) | 3 bit
            write_reg_x_in_buffer(6, instruction.desc.regCount, instruction.registries, buffer); // RegX(6) | 3 bit
            write_reg_x_in_buffer(7, instruction.desc.regCount, instruction.registries, buffer); // RegX(7) | 3 bit
            buffer.push_bit(0); // 1 bit

            // 16 bit was wrote, ByteBuffer flushed

            buffer.write_uint16(instruction.datas[1].value);
        }
    }

    return buffer;
}

ByteBuffer Assembler::assemble(const std::string& path)
{
    CompiledFile file = compile_file(path);
    return compiled_file_to_bytebuffer(file);
}
