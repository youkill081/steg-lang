//
// Created by Roumite on 20/02/2026.
//

#include "Compiler.h"

#include <charconv>
#include <iostream>

#include "Logger.h"
#include "utils/utils.hpp"

using namespace compiler;

std::span<const ParsedLine> Compiler::getSectionLines(
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
            number_line_section += 1;
        }

        current_index += 1;
    }

    if (number_line_section != 0)
        return {lines.data() + section_start_index + 1, number_line_section};

    if (throwIfNotFound)
        throw CompilerError("Mandatory section \"" + sectionName + "\" not found");
    return {};
}

std::vector<uint16_t> Compiler::tokenToData(const std::string& token, const std::string &variableName)
{
    uint16_t result;
    auto [ptr, ec] = std::from_chars(token.data(), token.data() + token.size(), result);

    if (ec == std::errc())
        return {result};
    if (ec == std::errc::invalid_argument)
    {
        std::vector<uint16_t> result;
        for (const auto &c : token)
            result.push_back(static_cast<uint16_t>(c));
        return result;
    }

    if (ec == std::errc::result_out_of_range)
    {
        throw CompilerError("Value \"" + token + "\" for variable \"" + variableName + "\" is out of range");
    }
    throw CompilerError("Failed to parse token \"" + token + "\" for variable \"" + variableName + "\"");
}

Variable Compiler::parsedLineToVariable(const ParsedLine& line)
{
    if (line.tokens.size() < 2)
        throw CompilerError("Invalid variable declaration: " + line.original_line);

    std::vector<uint16_t> data;
    std::string variable_name = line.tokens[0];
    for (int i = 1; i < line.tokens.size(); i++)
    {
        auto token_data = tokenToData(line.tokens[i], variable_name);
        data.insert(data.end(), token_data.begin(), token_data.end());
    }

    return {
        .name = variable_name,
        .value = data
    };
}

VariableSet Compiler::parseVariables(const std::vector<ParsedLine> &lines)
{
    Logger::log("Parsing variables", "Compiler");

    const auto variables_lines = getSectionLines(lines, VARIABLE_SECTION_NAME);
    if (variables_lines.empty())
    {
        Logger::log("No variables found", "Compiler");
        return {};
    }

    VariableSet variables;
    for (const auto &line : variables_lines)
        variables.push_variable(parsedLineToVariable(line));
    variables.display();
    return variables;
}

const InstructionDesc &Compiler::getInstructionDescFromParsedLine(const ParsedLine& line)
{
    std::string instruction_name = line.tokens[0];
    for (auto &instruction : instructionSet)
    {
        if (isEqual(instruction_name, instruction.name))
            return instruction;
    }

    throw CompilerError("Unknown instruction \"" + instruction_name + "\"");
}

RegNames Compiler::stringToRegName(const std::string& reg_name)
{
    if (not stringToRegistry.contains(reg_name))
        throw CompilerError("Unknown register \"" + reg_name + "\"");
    return stringToRegistry.at(reg_name);
}

UsedRegistries Compiler::getUsedRegistriesFromParsedLine(const InstructionDesc &desc, const ParsedLine& line)
{
    UsedRegistries registries;
    for (uint32_t i = 1; i < static_cast<uint32_t>(desc.regCount) + 1; i++)
        registries[i - 1] = stringToRegName(line.tokens[i]);
    return registries;
}

/*
 * Basically this function remove brackets from strings ([variable] -> variable)
 */
std::string Compiler::userVariableWriteAsAddressToString(const std::string& token)
{
    if (not userWriteVariableAsAddress(token))
        throw CompilerError("[userVariableWriteAsAddressToString] Invalid variable address \"" + token + "\"");
    return token.substr(1, token.size() - 2);
}

bool Compiler::userWriteVariableAsAddress(const std::string& token)
{
    return token[0] == '[' && token[token.size() - 1] == ']';
}

uint16_t Compiler::tokenToUint16(const std::string& token)
{
    if (tokenIsValidValue(token))
    {
        uint16_t result;
        std::from_chars(token.data(), token.data() + token.size(), result);
        return result;
    }
    throw CompilerError("Invalid uint16 value \"" + token + "\"");
}

bool Compiler::tokenIsValidValue(const std::string& token)
{
    uint16_t result;
    auto [ptr, ec] = std::from_chars(token.data(), token.data() + token.size(), result);
    return ec == std::errc();
}

DataValueParsingResult Compiler::parseDataValue(std::string token, const VariableSet &variables)
{
    bool is_address = userWriteVariableAsAddress(token);
    if (is_address)
        token = userVariableWriteAsAddressToString(token);

    if (tokenIsValidValue(token))
        return {
            .is_address = is_address,
            .value = tokenToUint16(token)
        };
    return {
        .is_address = true, // Variables are always managed as address'
        .value = variables.get_variable_address(variables.get_variable_by_name(token))
    };
}

DataValues Compiler::getDataValuesFromParsedLine(const InstructionDesc& desc, const ParsedLine& line, const VariableSet &variables)
{
    DataValues data{};
    switch (desc.dataCount)
    {
    case TWO_DATA:
        data[1] = parseDataValue(
            line.tokens[desc.regCount + 2],
            variables
        );
    case ONE_DATA:
        data[0] = parseDataValue(
            line.tokens[desc.regCount + 1],
            variables
        );
    default:
        return data;
    }
}

Instruction Compiler::parsedLineToInstruction(const ParsedLine& line, const VariableSet& variables)
{
    const InstructionDesc &desc = getInstructionDescFromParsedLine(line);
    uint32_t token_needed = static_cast<uint32_t>(desc.dataCount) + static_cast<uint32_t>(desc.regCount) + 1;

    if (line.tokens.size() != token_needed)
        throw CompilerError("Number of token missmatch for instruction \"" + line.original_line + "\"");

    return {
        .desc = desc,
        .registries = getUsedRegistriesFromParsedLine(desc, line),
        .datas = getDataValuesFromParsedLine(desc, line, variables)
    };
}

InstructionSet Compiler::parseInstructions(const std::vector<ParsedLine>& lines, const VariableSet& variables)
{
    Logger::log("Parsing instructions", "Compiler");

    const auto instructions_lines = getSectionLines(lines, INSTRUCTION_SECTION_NAME, true);
    if (instructions_lines.empty())
        throw CompilerError("No instructions in .text section !");

    InstructionSet instructions;
    for (const auto &line : instructions_lines)
        instructions.push_back(parsedLineToInstruction(line, variables));
    instructions.display();
    return instructions;
}

CompiledFile Compiler::compileFile(const std::string& path)
{
    TextParser parser(path);

    try
    {
        const auto lines = parser.parse();

        VariableSet variables = parseVariables(lines);
        InstructionSet instructions = parseInstructions(lines, variables);
        return {
            .variables = variables,
            .instructions = instructions
        };
    } catch (const TextParserError &error)
    {
        throw CompilerError(error.what());
    }
}

void Compiler::writeRegXInBuffer(
    uint8_t reg_x,
    const RegCount& reg_count,
    const UsedRegistries& registries,
    ByteBuffer& buffer
) {
    if (reg_x >= static_cast<uint8_t>(reg_count))
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

void Compiler::writeDatasFlagInBuffer(
    const DataCount& data_count,
    const DataValues& data_parsing_result,
    ByteBuffer& buffer)
{
    const bool first_bit_value = static_cast<uint8_t>(data_count) >= 1 && data_parsing_result[0].is_address;
    const bool second_bit_value = static_cast<uint8_t>(data_count) >= 2 && data_parsing_result[1].is_address;

    buffer.push_bit(first_bit_value);
    buffer.push_bit(second_bit_value);
}

ByteBuffer Compiler::compiledFileToBytebuffer(const CompiledFile& compiledFile)
{
    Logger::log("Convert file to ByteBuffer", "Compiler");

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

    // Push Instructions
    for (const auto &instruction : compiledFile.instructions)
    {
        buffer.write_uint8(instruction.desc.opcode);

        // Registry And Flag
        writeRegXInBuffer(1, instruction.desc.regCount, instruction.registries, buffer); // RegX(1) | 3 bit
        writeDatasFlagInBuffer(instruction.desc.dataCount, instruction.datas, buffer); // Data Flag | 2 bit
        writeRegXInBuffer(2, instruction.desc.regCount, instruction.registries, buffer); // RegX(2) | 3 bit

        // 8 bit was wrote, ByteBuffer flushed

        buffer.write_uint16(instruction.datas[0].value);

        if (instruction.desc.dataCount == TWO_DATA || static_cast<uint8_t>(instruction.desc.regCount) > 2)
        {
            writeRegXInBuffer(3, instruction.desc.regCount, instruction.registries, buffer); // RegX(3) | 3 bit
            writeRegXInBuffer(4, instruction.desc.regCount, instruction.registries, buffer); // RegX(4) | 3 bit
            writeRegXInBuffer(5, instruction.desc.regCount, instruction.registries, buffer); // RegX(5) | 3 bit
            writeRegXInBuffer(6, instruction.desc.regCount, instruction.registries, buffer); // RegX(6) | 3 bit
            writeRegXInBuffer(7, instruction.desc.regCount, instruction.registries, buffer); // RegX(7) | 3 bit
            buffer.push_bit(0); // 1 bit

            // 16 bit was wrote, ByteBuffer flushed

            buffer.write_uint16(instruction.datas[1].value);
        }
    }

    return buffer;
}

ByteBuffer Compiler::compile(const std::string& path)
{
    CompiledFile file = compileFile(path);
    return compiledFileToBytebuffer(file);
}
