//
// Created by Roumite on 23/02/2026.
//

#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <map>

namespace assembler
{
    struct ParsedLine;

    struct Variable
    {
        std::string name;
        std::vector<uint16_t> value;
        bool operator<(const Variable &other) const {
            if (name != other.name)
                return name < other.name;
            return value < other.value;
        }
    };

    class VariableSet : public std::vector<Variable>
    {
    private:
        std::map<Variable, uint16_t> variables_address;
        uint16_t current_address = 0;

        static std::vector<uint16_t> token_to_data(const std::string &token, const std::string &variableName);
        static Variable parsed_line_to_variable(const ParsedLine &line);
    public:
        void push_variable(const std::string &name, const std::vector<uint16_t> &value);
        void push_variable(const Variable &new_variable);
        [[nodiscard]] uint16_t get_variable_address(const Variable &variable) const;
        [[nodiscard]] const Variable &get_variable_by_name(const std::string &name) const;
        [[nodiscard]] bool contains_variable_by_name(const std::string &name) const;
        void display() const;

        static VariableSet from_parsed_lines(const std::vector<ParsedLine> &lines);
    };
}