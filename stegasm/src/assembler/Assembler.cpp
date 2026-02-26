//
// Created by Roumite on 20/02/2026.
//

#include "Assembler.h"

#include <charconv>
#include <iostream>

#include "assembler_exception.h"
#include "TextParser.h"
#include "utils/utils.h"

using namespace assembler;

CompiledFile Assembler::compile_file(const std::string &path)
{
    TextParser parser(path);
    try
    {
        const auto lines = parser.parse();
        auto files = FileSet::from_parsed_lines(lines);
        auto variables = VariableSet::from_parsed_lines(lines);
        auto instructions = InstructionSet::from_parsed_lines(lines, variables, files);
        return {
            .files = std::move(files),
            .variables = std::move(variables),
            .instructions = std::move(instructions)
        };
    }
    catch (const TextParserError &error)
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

        buffer.write_uint8(file.extension.size());
        for (const auto &c : file.extension)
            buffer.write_uint8(c);

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
