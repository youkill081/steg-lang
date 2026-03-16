//
// Created by Roumite on 14/03/2026.
//

#include "ASTStatementNode.h"

void compiler::ASTBlockStatementNode::display(std::size_t left_padding)
{
    display_name("Block", left_padding);

    for (auto &statement : statements)
    {
        statement->display(left_padding + 1);
    }
}

void compiler::ASTIfStatementNode::display(std::size_t left_padding)
{
    display_name("If", left_padding);

    display_indent(left_padding);
    std::cout << "Condition" << std::endl;
    condition->display(left_padding + 1);

    then_statement->display(left_padding + 1);

    if (false_statement) {
        display_indent(left_padding);
        std::cout << "Else" << std::endl;
        false_statement->display(left_padding + 2);
    }
}

void compiler::ASTVariableStatement::display(std::size_t left_padding)
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

void compiler::ASTWhileStatementNode::display(std::size_t left_padding)
{
    display_name("While", left_padding);

    display_indent(left_padding);
    std::cout << "Condition" << std::endl;
    condition->display(left_padding + 1);

    display_indent(left_padding);
    std::cout << "Body" << std::endl;
    body_statement->display(left_padding + 1);
}

void compiler::ASTForStatementNode::display(std::size_t left_padding)
{
    display_name("For", left_padding);

    display_indent(left_padding);
    std::cout << "Init" << std::endl;
    init_expression->display(left_padding + 1);

    display_indent(left_padding);
    std::cout << "Condition" << std::endl;
    condition_expression->display(left_padding + 1);

    display_indent(left_padding);
    std::cout << "Post" << std::endl;
    post_expression->display(left_padding + 1);

    display_indent(left_padding);
    std::cout << "Body" << std::endl;
    body_statement->display(left_padding + 1);
}

void compiler::ASTReturnStatement::display(std::size_t left_padding)
{
    display_name("Return", left_padding);

    if (expression)
    {
        expression->display(left_padding + 1);
    } else
    {
        display_indent(left_padding + 1);
        std::cout << "No return expression" << std::endl;
    }
}

void compiler::ASTBreakStatement::display(std::size_t left_padding)
{
    display_name("Break", left_padding);
}

void compiler::ASTContinueStatement::display(std::size_t left_padding)
{
    display_name("Continue", left_padding);
}

void compiler::ASTExpressionStatement::display(std::size_t left_padding)
{
    display_name("Expression", left_padding);
    expression->display(left_padding + 1);
}
