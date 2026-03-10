//
// Created by Roumite on 20/02/2026.
//

#include "Assembler.h"

#include "assembler_exception.h"
#include "TextParser.h"
#include "utils/LabelSet.h"
#include "utils/SubtexturesSet.h"
#include "utils/utils.h"

using namespace assembler;

void CompiledFile::merge(const CompiledFile& other, Linter& linter)
{
    this->files.merge(other.files, linter);
    this->subtextures.merge(other.subtextures, linter);
    this->variables.merge(other.variables, linter);
}

void Assembler::compile_symbols(CompiledFile &compiled_file, const std::vector<ParsedLine> &lines, Linter &linter)
{
    auto section_line = get_section_lines(lines, IMPORT_SECTION_NAME);
    linter.foreach_lines(section_line, [&](const ParsedLine &line)
    {
        if (line.tokens.size() != 1)
            Linter::error("Number of token mismatch for import");
        TextParser parser(line.tokens[0]);
        try
        {
            const auto imported_lines = parser.parse();
            compile_symbols(compiled_file, imported_lines, linter);
        }
        catch (const TextParserError &error) { Linter::error(error.what()); }
    });

    auto files = FileSet::from_parsed_lines(lines, linter);
    compiled_file.files.merge(files, linter);

    auto subtextures = SubtexturesSet::from_parsed_lines(lines, compiled_file.files, linter);
    compiled_file.subtextures.merge(subtextures, linter);

    auto variables = VariableSet::from_parsed_lines(lines, linter);
    compiled_file.variables.merge(variables, linter);
}

void Assembler::collect_labels(
    CompiledFile& compiled_file,
    const std::vector<ParsedLine>& lines,
    Linter& linter,
    uint64_t &instruction_counter)
{
    auto labels = LabelSet::from_parsed_lines(lines, linter);
    labels.add_padding(instruction_counter);
    compiled_file.labels.merge(labels, linter);

    instruction_counter += InstructionSet::count_text_lines(lines);

    auto section_lines = get_section_lines(lines, IMPORT_SECTION_NAME);
    linter.foreach_lines(section_lines, [&](const ParsedLine &line)
    {
        if (line.tokens.size() != 1)
            Linter::error("Number of token mismatch for import");
        TextParser parser(line.tokens[0]);
        try
        {
            const auto imported_lines = parser.parse();
            collect_labels(compiled_file, imported_lines, linter, instruction_counter);
        }
        catch (const TextParserError &error) { Linter::error(error.what()); }
    });
}

void Assembler::emit_instructions(CompiledFile& compiled_file, const std::vector<ParsedLine>& lines, Linter& linter)
{
    auto new_instructions = InstructionSet::from_parsed_lines(lines, compiled_file.symbols, linter);
    compiled_file.instructions.merge(new_instructions, linter);

    auto section_lines = get_section_lines(lines, IMPORT_SECTION_NAME);
    linter.foreach_lines(section_lines, [&](const ParsedLine &line)
    {
        if (line.tokens.size() != 1)
            Linter::error("Number of token mismatch for import");
        TextParser parser(line.tokens[0]);
        try
        {
            const auto imported_lines = parser.parse();
            emit_instructions(compiled_file, imported_lines, linter); // ← emit, pas compile
        }
        catch (const TextParserError &error) { throw AssemblerError(error.what()); }
    });
}

void Assembler::compile_instructions(CompiledFile &compiled_file, const std::vector<ParsedLine> &lines, Linter &linter)
{
    uint64_t counter = 0;
    collect_labels(compiled_file, lines, linter, counter);

    compiled_file.symbols =
        compiled_file.files.get_symbols() +
        compiled_file.subtextures.get_symbols() +
        compiled_file.variables.get_symbols() +
        compiled_file.labels.get_symbols();

    emit_instructions(compiled_file, lines, linter);
}

void Assembler::compile_file(CompiledFile &compiled_file, TextParser &parser, Linter &linter)
{
    try
    {
        const auto lines = parser.parse();
        compile_symbols(compiled_file, lines, linter);
        compile_instructions(compiled_file, lines, linter);
    }
    catch (const TextParserError &error)
    {
        throw AssemblerError(error.what());
    }
}

// void Assembler::write_reg_x_in_buffer(
//     uint8_t reg_x,
//     const RegCount& reg_count,
//     const UsedRegistries& registries,
//     ByteBuffer& buffer
// ) {
//     if (reg_x <= static_cast<uint8_t>(reg_count))
//     {
//         const uint8_t reg = registries[reg_x - 1];
//         const uint8_t write_reg = static_cast<uint8_t>(reg & 0b111);
//
//         buffer.push_bit((write_reg >> 2) & 0x1);
//         buffer.push_bit((write_reg >> 1) & 0x1);
//         buffer.push_bit((write_reg >> 0) & 0x1);
//     } else {
//         buffer.push_bit(0);
//         buffer.push_bit(0);
//         buffer.push_bit(0);
//     }
// }

// void Assembler::write_datas_flag_in_buffer(
//     const DataCount& data_count,
//     const DataValues& data_parsing_result,
//     ByteBuffer& buffer)
// {
//     const bool first_bit_value = static_cast<uint8_t>(data_count) >= 1 && data_parsing_result[0].is_address;
//     const bool second_bit_value = static_cast<uint8_t>(data_count) >= 2 && data_parsing_result[1].is_address;
//
//     buffer.push_bit(first_bit_value);
//     buffer.push_bit(second_bit_value);
// }

ByteBuffer Assembler::compiled_file_to_bytebuffer(CompiledFile &compiledFile, Linter &linter)
{
    ByteBuffer buffer{};

    if (compiledFile.instructions.empty())
    {
        linter.inline_error("No instructions found !");
        return buffer;
    }

    linter.error_guard([&]
    {
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

        // Push subtextures
        buffer.write_uint32(compiledFile.subtextures.size());
        for (const auto &sub : compiledFile.subtextures)
        {
            buffer.write_uint16(sub.origin_file.descriptor);
            buffer.write_uint16(sub.descriptor);
            buffer.write_uint16(sub.x);
            buffer.write_uint16(sub.y);
            buffer.write_uint16(sub.width);
            buffer.write_uint16(sub.height);
        }

        for (const auto& instruction : compiledFile.instructions)
        {
            buffer.write_uint8(instruction.desc.opcode);

            buffer.push_bit((instruction.handler_number >> 1) & 1);
            buffer.push_bit(instruction.handler_number & 1);

            // 3 à 8. RegX(1), RegX(2) et RegX(3) (6 bits par registre : 1 bit d'état + 5 bits d'ID)
            for (int i = 0; i < 3; ++i)
            {
                auto reg_type = instruction.instruction_parameters.registries[i].type;
                auto reg_name = instruction.instruction_parameters.registries[i].registry;

                buffer.push_bit(reg_type == REG_ADDRESS);

                buffer.push_bit((reg_name >> 4) & 1);
                buffer.push_bit((reg_name >> 3) & 1);
                buffer.push_bit((reg_name >> 2) & 1);
                buffer.push_bit((reg_name >> 1) & 1);
                buffer.push_bit((reg_name >> 0) & 1);
            }

            uint8_t number_of_registry = 0;
            for (int i = 0; i < 3; ++i)
            {
                if (instruction.instruction_parameters.registries[i].type != REG_NO)
                {
                    number_of_registry++;
                }
            }
            buffer.push_bit((number_of_registry >> 1) & 1);
            buffer.push_bit(number_of_registry & 1);

            uint8_t data_type = 0b00;
            if (instruction.instruction_parameters.data_value.data_count == ONE_DATA)
            {
                if (instruction.instruction_parameters.data_value.is_address)
                {
                    data_type = 0b10; // 2 = Adresse
                }
                else
                {
                    data_type = 0b01; // 1 = Valeur brute
                }
            }

            buffer.push_bit((data_type >> 1) & 1);
            buffer.push_bit(data_type & 1);

            if (instruction.instruction_parameters.data_value.data_count != NO_DATA)
            {
                buffer.write_uint32(instruction.instruction_parameters.data_value.value);
            }
        }
    });

    return buffer;
}

ByteBuffer Assembler::assemble(const std::string& path, Linter &linter, bool throw_if_error)
{
    auto parser = TextParser(path);

    CompiledFile file;
    compile_file(file, parser, linter);

    ByteBuffer bytes = compiled_file_to_bytebuffer(file, linter);

    if (linter.has_errors())
    {
        if (throw_if_error)
            throw AssemblerError("Errors detected during assembly");
        return {};
    }
    return bytes;
}

ByteBuffer Assembler::assemble_from_text(const std::string& text_data, Linter& linter, bool throw_if_error)
{
    auto parser = TextParser::from_string(text_data);

    CompiledFile file;
    compile_file(file, parser, linter);

    ByteBuffer bytes = compiled_file_to_bytebuffer(file, linter);

    if (linter.has_errors())
    {
        if (throw_if_error)
            throw AssemblerError("Errors detected during assembly");
        return {};
    }
    return bytes;
}
