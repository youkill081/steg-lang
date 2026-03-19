//
// Created by Roumite on 23/02/2026.
//

#include "VariableSet.h"
#include "utils.h"
#include <charconv>
#include <iostream>
#include <utils.h>

#include "assembler/Assembler.h"
#include "assembler/TextParser.h"

using namespace assembler;

template <typename T>
std::vector<VariableType> token_to_data_generic(
    const std::string &token,
    const std::string &variableName
) {
    T result;
    auto [ptr, ec] = std::from_chars(token.data(), token.data() + token.size(), result);

    if (ec == std::errc())
        return { result };
    if (ec == std::errc::invalid_argument) // For strings
    {
        std::vector<VariableType> end_vector;
        for (const auto &c : token)
            end_vector.push_back(static_cast<VariableType>(static_cast<T>(c)));
        return end_vector;
    }
    if (ec == std::errc::result_out_of_range)
        Linter::error("Value \"" + token + "\" for variable \"" + variableName + "\" is out of range");
    Linter::error("Failed to parse token \"" + token + "\" for variable \"" + variableName + "\"");
}

std::vector<VariableType> token_to_data(
    const std::string &token,
    const std::string &variableName,
    VariableTypeFlag type)
{
    switch (type) {
    case VARIABLE_UINT8:  return token_to_data_generic<uint8_t>(token, variableName);
    case VARIABLE_UINT16: return token_to_data_generic<uint16_t>(token, variableName);
    case VARIABLE_UINT32: return token_to_data_generic<uint32_t>(token, variableName);
    default: Linter::error("Type inconnu");
    }
}

VariableType get_end_of_variable(VariableTypeFlag type)
{
    switch (type)
    {
    case VARIABLE_UINT8:
        return static_cast<uint8_t>(0);
    case VARIABLE_UINT16:
        return static_cast<uint16_t>(0);
    case VARIABLE_UINT32:
        return static_cast<uint32_t>(0);
    default:
        return static_cast<uint32_t>(0);
    }
}

VariableTypeFlag VariableSet::get_variable_type(const ParsedLine &line)
{
    if (string_equal(line.tokens[1], "DB"))
        return VARIABLE_UINT8;
    if (string_equal(line.tokens[1], "DW"))
        return VARIABLE_UINT16;
    if (string_equal(line.tokens[1], "DD"))
        return VARIABLE_UINT32;
    Linter::error("Unknown variable type \"" + line.tokens[1] + "\"");
}

Variable VariableSet::parsed_line_to_variable(const ParsedLine &line)
{
    if (line.tokens.size() < 3)
        Linter::error("Number of token missmatch for variable declaration");

    VariableTypeFlag variable_type_flag = get_variable_type(line);

    std::vector<VariableType> data;
    const std::string variable_name = line.tokens[0];
    for (int i = 2; i < line.tokens.size(); i++)
    {
        auto token_data = token_to_data(line.tokens[i], variable_name, variable_type_flag);
        data.insert(data.end(), token_data.begin(), token_data.end());
    }
    data.emplace_back(get_end_of_variable(variable_type_flag));

    return {
        .name = variable_name,
        .type = variable_type_flag,
        .value = data,
        .flags = static_cast<uint8_t>(variable_type_flag)
    };
}

VariableSet VariableSet::from_parsed_lines(const std::vector<ParsedLine> &lines, Linter &linter)
{
    const auto variables_lines = get_section_lines(lines, VARIABLE_SECTION_NAME);
    if (variables_lines.empty())
        return {};

    VariableSet variables;
    linter.foreach_lines(variables_lines, [&](const ParsedLine &line)
    {
        variables.push_variable(parsed_line_to_variable(line));
    });
    return variables;
}

SymbolSet VariableSet::get_symbols() const
{
    SymbolSet symbols;
    for (const auto &var : *this)
        symbols.insert_symbol(var.name, get_variable_address(var), SymbolType::Variable);
    return symbols;
}

uint8_t get_item_size_in_bytes(VariableTypeFlag type) {
    switch (type) {
    case VARIABLE_UINT8:  return 1;
    case VARIABLE_UINT16: return 2;
    case VARIABLE_UINT32: return 4;
    default: return 1;
    }
}

void VariableSet::merge(const VariableSet& other, Linter& linter)
{
    linter.foreach(other, [&](const Variable& var)
    {
        if (this->contains(var))
            Linter::error("Duplicate variable \"" + var.name + "\" !");

        this->insert(var);
        this->variables_address[var] = current_address;

        current_address += var.value.size() * get_item_size_in_bytes(var.type);
    });
}

void VariableSet::push_variable(
    const std::string &name,
    const std::vector<VariableType> &value,
    VariableTypeFlag type,
    uint8_t flags)
{
    Variable new_variable = {
        .name=name,
        .type = type,
        .value=value,
        .flags = flags
    };
    this->push_variable(new_variable);
}

void VariableSet::push_variable(const Variable &new_variable)
{
    if (this->contains(new_variable))
        Linter::error("Duplicate variable \"" + new_variable.name + "\" !");

    this->insert(new_variable);
    this->variables_address[new_variable] = current_address;

    uint32_t size_in_bytes = new_variable.value.size() * get_item_size_in_bytes(new_variable.type);
    current_address += size_in_bytes;
}

uint32_t VariableSet::get_variable_address(const Variable &variable) const
{
    if (not this->variables_address.contains(variable))
        Linter::error("Variable \"" + variable.name + "\" not found !");
    return this->variables_address.at(variable);
}

const Variable &VariableSet::get_variable_by_name(const std::string &name) const
{
    for (const auto &variable : *this)
        if (variable.name == name)
            return variable;
    Linter::error("Variable \"" + name + "\" not found !");

}

bool VariableSet::contains_variable_by_name(const std::string &name) const
{
    for (const auto &variable : *this)
        if (string_equal(variable.name, name))
            return true;
    return false;
}

void VariableSet::display() const
{
    for (const auto &variable : *this)
    {
        std::cout << variable.name << " -> ";
        for (const auto &data : variable.value)
        {
            std::visit([](auto&& arg) {
                std::cout << +arg << " ";
            }, data);
        }
        std::cout << std::endl;
    }
}