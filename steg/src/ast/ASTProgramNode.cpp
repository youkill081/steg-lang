//
// Created by Roumite on 14/03/2026.
//

#include <iostream>
#include "ASTProgramNode.h"

#include "ASTVisitor.h"

using namespace compiler;

void ASTFileProgramNode::display(std::size_t left_padding)
{
    display_name("File", left_padding);

    display_indent(left_padding);
    std::cout << "name: \"" << name << "\" path: \"" << path << "\"" << std::endl;
}

void ASTFileProgramNode::accept(ASTVisitor* visitor)
{
    visitor->visit(this);
}

void ASTFilesProgramNode::accept(ASTVisitor* visitor)
{
    visitor->visit(this);
}

void ASTImportProgramNode::display(std::size_t left_padding)
{
    display_name("Import", left_padding);

    display_indent(left_padding);
    std::cout << "From \"" << path << "\"" << std::endl;
    display_indent(left_padding);
    std::cout << "Importing: " ;
    for (const auto &imported_name : functions_variables)
    {
        std::cout << imported_name.value << " ";
    }
    std::cout << std::endl;
}

void ASTImportProgramNode::accept(ASTVisitor* visitor)
{
    visitor->visit(this);
}

void ASTParameterProgramNode::display(std::size_t left_padding)
{
    display_name("Parameter", left_padding);

    display_indent(left_padding + 1);
    std::cout << "name: " << name << std::endl;

    type->display(left_padding + 1);
}

void ASTParameterProgramNode::accept(ASTVisitor* visitor)
{
    visitor->visit(this);
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

void ASTFunctionProgramNode::accept(ASTVisitor* visitor)
{
    visitor->visit(this);
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

    display_indent(left_padding);
    std::cout << "Imports : " << std::endl;
    for (const auto &import : imports)
    {
        import->display(left_padding + 1);
    }

    display_indent(left_padding);
    std::cout << "Files : " << std::endl;
    for (const auto &file : files)
    {
        file->display(left_padding + 1);
    }
}

void ASTMainProgramNode::accept(ASTVisitor* visitor)
{
    visitor->visit(this);
}
