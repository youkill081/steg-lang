//
// Created by Roumite on 21/02/2026.
//

#include "Loader.h"

#include <iostream>

#include "exceptions.h"

void Loader::init_variables(ByteBuffer& buffer, Runtime& runtime)
{
    const uint32_t number_of_variable = buffer.read_uint32();

    for (int i = 0; i < number_of_variable; i++)
    {
        const uint16_t address = buffer.read_uint16();
        const uint16_t size = buffer.read_uint16();

        runtime.memory.allocate_at(address, size); // Allocate memory

        for (int y = 0; y < size; y++)
            runtime.memory.write(address + y, buffer.read_uint16());
    }
}

void Loader::init_files(ByteBuffer& buffer, Runtime& runtime)
{
    uint32_t number_of_files = buffer.read_uint32();
    for (int i = 0; i < number_of_files; i++)
    {
        uint16_t descriptor = buffer.read_uint16();

        uint8_t extension_size = buffer.read_uint8();
        std::string extension;
        for (int y = 0; y < extension_size; y++)
            extension += static_cast<char>(buffer.read_uint8());

        uint32_t size = buffer.read_uint32();
        ByteBuffer data;
        for (int y = 0; y < size; y++)
            data.write_uint8(buffer.read_uint8());
        data.reset_cursor();

        runtime.files.push_file(descriptor, std::make_shared<File>(data, extension));
    }
}

void Loader::init_subtextures(ByteBuffer& buffer, Runtime& runtime)
{
    uint32_t number_of_subtextures = buffer.read_uint32();
    for (int i = 0; i < number_of_subtextures; i++)
    {
        uint16_t origin_file_descriptor = buffer.read_uint16();
        uint16_t descriptor = buffer.read_uint16();
        uint16_t x = buffer.read_uint16();
        uint16_t y = buffer.read_uint16();
        uint16_t width = buffer.read_uint16();
        uint16_t height = buffer.read_uint16();
    }
}

const InstructionDesc &get_instruction_desc(uint8_t opcode)
{
    for (const auto &instruction : instructionSet)
    {
        if (instruction.opcode == opcode)
            return instruction;
    }
    throw LoaderError("Opcode not found !");
}

void Loader::init_instructions(ByteBuffer& buffer, Runtime& runtime)
{
    while (buffer.remaining_uint32() != 0)
    {
        const uint32_t instruction_beg = buffer.read_uint32();

        auto current_instruction = InstructionView(static_cast<uint64_t>(instruction_beg) << 32);
        const auto &current_instruction_desc = get_instruction_desc(current_instruction.opcode());

        if (current_instruction_desc.regCount > 2 || current_instruction_desc.dataCount > 1)
        {
            current_instruction.raw_data |= static_cast<uint64_t>(buffer.read_uint32());
        }
        runtime.instructions.push_back({
            .view = current_instruction,
            .desc = current_instruction_desc
        });
    }
}

Runtime Loader::load(ByteBuffer& buffer)
{
    Runtime runtime;

    buffer.reset_cursor();
    init_variables(buffer, runtime);
    init_files(buffer, runtime);
    init_subtextures(buffer, runtime);
    init_instructions(buffer, runtime);
    return runtime;
}
