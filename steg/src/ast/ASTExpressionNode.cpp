//
// Created by Roumite on 14/03/2026.
//

#include "ASTExpressionNode.h"

void compiler::ASTBinaryExpressionNode::display(std::size_t left_padding)
{
    display_name("Binary", left_padding);

    display_indent(left_padding + 1);
    std::cout << "Type: " << ASTBinaryExpressionNode_type_to_string[op_type] << std::endl;
    left->display(left_padding + 1);
    right->display(left_padding + 1);
}

void compiler::ASTUnaryExpressionNode::display(std::size_t left_padding)
{
    display_name("Unary", left_padding);

    display_indent(left_padding + 1);
    std::cout << "Type: " << ASTUnaryExpressionNode_type_to_string[op_type] << std::endl;
    expression->display(left_padding + 1);
}

void compiler::ASTTypeNode::display(std::size_t left_padding)
{
    display_name("Type", left_padding);

    display_indent(left_padding + 1);
    std::cout << "Type: " << ASTTypeNode_type_to_string[type] << std::endl;
}

void compiler::ASTLiteralExpressionNode::display(std::size_t left_padding)
{
    display_name("Literal", left_padding);

    type->display(left_padding + 1);
}

void compiler::ASTIdentifierExpressionNode::display(std::size_t left_padding)
{
    display_name("Identifier", left_padding);

    display_indent(left_padding + 1);
    std::cout << "Name: " << name << std::endl;
}

void compiler::ASTCallExpressionNode::display(std::size_t left_padding)
{
    display_name("Call", left_padding);

    callee->display(left_padding + 1);

    display_indent(left_padding);
    std::cout << "arguments: " << std::endl;
    for (const auto &argument : args)
        argument->display(left_padding + 1);
}

void compiler::ASTIndexExpressionNode::display(std::size_t left_padding)
{
    display_name("Index", left_padding);

    array->display(left_padding + 1);
    index->display(left_padding + 1);
}

void compiler::ASTAssignExpressionStatement::display(std::size_t left_padding)
{
    display_name("Assign", left_padding);

    display_indent(left_padding);
    std::cout << "Target" << std::endl;
    target->display(left_padding + 1);

    display_indent(left_padding);
    std::cout << "operator -> " << ASTAssignExpressionStatement_type_to_string[op] << std::endl;

    display_indent(left_padding);
    std::cout << "value" << std::endl;
    value->display(left_padding + 1);
}

void compiler::ASTAddressOfExpressionNode::display(std::size_t left_padding)
{
    display_name("Address of", left_padding);

    expression->display(left_padding + 1);
}

void compiler::ASTDereferenceExpressionNode::display(std::size_t left_padding)
{
    display_name("Dereference", left_padding);

    expression->display(left_padding + 1);
}
