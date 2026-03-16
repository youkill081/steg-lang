//
// Created by Roumite on 14/03/2026.
//

#include <iostream>
#include "ASTProgramNode.h"

using namespace compilator;

void ASTParameterProgramNode::display(std::size_t left_padding)
{
    display_name("Parameter", left_padding);

    display_indent(left_padding + 1);
    std::cout << "name: " << name << std::endl;

    type->display(left_padding + 1);
}

void ASTFunctionProgramNode::display(std::size_t left_padding)
{
    display_name("Function", left_padding);

    display_indent(left_padding);
    std::cout << "name: " << name << std::endl;

    display_indent(left_padding);
    std::cout << "is exported: " << is_exported << std::endl;

    display_indent(left_padding);
    std::cout << "parameters: " << std::endl;
    for (const auto &parameter : parameters)
        parameter->display(left_padding + 1);

    display_indent(left_padding);
    std::cout << "return type: " << std::endl;
    return_type->display(left_padding + 1);

    display_indent(left_padding);
    std::cout << "statement: " << std::endl;
    statement->display(left_padding + 1);
}

void ASTVariableProgramNode::display(std::size_t left_padding)
{
    display_name("Variable", left_padding);

    display_indent(left_padding);
    std::cout << "name: " << name << std::endl;

    type->display(left_padding + 1);
    if (expression)
        expression->display(left_padding + 1);
    else
        std::cout << "No expression" << std::endl;
}

void ASTMainProgramNode::display(std::size_t left_padding)
{
    display_name("Main Program", left_padding);

    display_indent(left_padding);
    std::cout << "Functions : " << std::endl;
    for (const auto &function : functions)
    {
        function->display(left_padding + 1);
    }

    display_indent(left_padding);
    std::cout << "Globals : " << std::endl;
    for (const auto &global : global_variables)
    {
        global->display(left_padding + 1);
    }
}
