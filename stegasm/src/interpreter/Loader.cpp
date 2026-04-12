//
// Created by Roumite on 21/02/2026.
//

#include "Loader.h"

#include <iostream>

#include "exceptions.h"
#include "assembler/utils/VariableSet.h"
#include "runtime/files/Subtexture.h"

uint8_t get_variable_items_size_in_byte(assembler::VariableTypeFlag variable_type)
{
    switch (variable_type)
    {
        case assembler::VARIABLE_UINT8:
            return 1;
        case assembler::VARIABLE_UINT16:
            return 2;
        case assembler::VARIABLE_UINT32:
            return 4;
    }
    throw LoaderError("Unknown variable type");
}

void Loader::init_variables(ByteBuffer& buffer, Runtime& runtime)
{
    const uint32_t number_of_variable = buffer.read_uint32();

    for (int i = 0; i < number_of_variable; i++)
    {
        const uint8_t flag = buffer.read_uint8();
        const uint32_t address = buffer.read_uint32();
        const uint16_t size = buffer.read_uint16();

        uint8_t variable_type = (flag & 0b11000000);
        uint8_t item_size = get_variable_items_size_in_byte(static_cast<assembler::VariableTypeFlag>(variable_type));

        runtime.memory.allocate_at(address, size * item_size);

        for (int y = 0; y < size; y++)
        {
            uint32_t target_addr = address + (y * item_size);

            if (item_size == 1) {
                runtime.memory.write_uint8(target_addr, buffer.read_uint8());
            }
            else if (item_size == 2) {
                runtime.memory.write_uint16(target_addr, buffer.read_uint16());
            }
            else if (item_size == 4) {
                runtime.memory.write_uint32(target_addr, buffer.read_uint32());
            }
        }
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
            data.append_uint8(buffer.read_uint8());
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

        runtime.files.push_file(descriptor, std::make_shared<SubTexture>(
            runtime.files.get_file(origin_file_descriptor),
            x, y, width, height
        ));
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

        if (current_instruction.data_type() != NO_DATA)
        {
            current_instruction.raw_data |= static_cast<uint64_t>(buffer.read_uint32());
        }
        runtime.instructions.push_back({
            .view = current_instruction,
            .desc = current_instruction_desc,
            .handler = current_instruction_desc.handlers[current_instruction.handler_number()]
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
