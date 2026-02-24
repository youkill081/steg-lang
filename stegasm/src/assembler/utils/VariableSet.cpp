//
// Created by Roumite on 23/02/2026.
//

#include "VariableSet.h"

#include "utils.h"
#include "assembler/assembler_exception.h"

void assembler::VariableSet::push_variable(const std::string& name, const std::vector<uint16_t>& value)
{
    Variable new_variable = {name, value};
    this->push_variable(new_variable);
}

void assembler::VariableSet::push_variable(const Variable& new_variable)
{
    for (const auto &variable : *this)
    {
        if (string_equal(variable.name, new_variable.name))
            throw AssemblerError("Multiple variable with name " + new_variable.name + " !");
    }
    this->push_back(new_variable);

    this->variables_address[new_variable] = current_address;
    current_address += new_variable.value.size();
}

uint16_t assembler::VariableSet::get_variable_address(const Variable& variable) const
{
    if (not this->variables_address.contains(variable))
        throw AssemblerError("[get_variable_address] Variable \"" + variable.name + "\" not found !");
    return this->variables_address.at(variable);
}

const assembler::Variable& assembler::VariableSet::get_variable_by_name(const std::string& name) const
{
    for (const auto &variable : *this)
        if (variable.name == name)
            return variable;
    throw AssemblerError("[get_variable_by_name] Variable \"" + name + "\" not found !");
}

bool assembler::VariableSet::contains_variable_by_name(const std::string& name) const
{
    for (const auto &variable : *this)
    {
        if (string_equal(variable.name, name))
            return true;
    }
    return false;
}

void assembler::VariableSet::display() const
{
    for (const auto &variable : *this)
    {
        std::cout << variable.name << " -> ";
        for (const auto &data : variable.value)
        {
            std::cout << data << " ";
        }
        std::cout << std::endl;
    }
}
