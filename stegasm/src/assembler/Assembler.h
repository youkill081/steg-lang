//
// Created by Roumite on 20/02/2026.
//

#pragma once

#include <filesystem>
#include <vector>
#include <span>
#include <map>

#include "../instructions.h"
#include "ByteBuffer.h"
#include "TextParser.h"
#include "utils/InstructionSet.h"

#include "utils/VariableSet.h"

#define SECTION_KEYWORD "section"
#define VARIABLE_SECTION_NAME ".data"
#define INSTRUCTION_SECTION_NAME ".text"

namespace assembler
{
    using LabelMap = std::map<std::string, uint16_t>; // Map label's name with its associated code line
    struct CompiledFile
    {
        VariableSet variables;
        InstructionSet instructions;
    };

    class Assembler
    {
    private:
        static std::span<const ParsedLine> getSectionLines(
            const std::vector<ParsedLine> &lines,
            const std::string& sectionName,
            bool throwIfNotFound = false
        );

        static std::vector<uint16_t> tokenToData(const std::string &token, const std::string &variableName);
        static Variable parsedLineToVariable(const ParsedLine &line);
        static VariableSet parseVariables(const std::vector<ParsedLine> &line);

        static const InstructionDesc &getInstructionDescFromParsedLine(const ParsedLine &line);
        static RegNames stringToRegName(const std::string &reg_name);
        static UsedRegistries getUsedRegistriesFromParsedLine(const InstructionDesc &desc, const ParsedLine &line);
        static std::string userVariableWriteAsAddressToString(const std::string &token);
        static bool user_write_value_in_bracket(const std::string &token);
        static uint16_t tokenToUint16(const std::string &token);
        static bool tokenIsValidValue(const std::string &token);
        static DataValueParsingResult parseDataValue(std::string token, const VariableSet &variables, const LabelMap &labels);
        static DataValues getDataValuesFromParsedLine(const InstructionDesc &desc, const ParsedLine &line, const LabelMap &labels, const VariableSet &variables);
        static Instruction parsedLineToInstruction(const ParsedLine &line, const VariableSet &variables, const LabelMap &labels);
        static InstructionSet parseInstructions(const std::vector<ParsedLine> &lines, const VariableSet &variables);

        static bool is_label(const ParsedLine &line);
        static LabelMap parseLabels(const std::span<const ParsedLine> &lines, const VariableSet &variables);

        static CompiledFile compileFile(const std::string &path);

        static void writeRegXInBuffer(uint8_t reg_x, const RegCount &reg_count, const UsedRegistries &registries, ByteBuffer &buffer);
        static void writeDatasFlagInBuffer(const DataCount &data_count, const DataValues &data_parsing_result, ByteBuffer &buffer);
        static ByteBuffer compiledFileToBytebuffer(const CompiledFile &compiledFile);
    public:
        static ByteBuffer assemble(const std::string &path);
    };
}
