//
// Created by Roumite on 18/03/2026.
//

#include "ControlFlowVisitor.h"

using namespace compiler;

/* Check for a block if all path return */
bool ControlFlowVisitor::always_returns(ASTNode* node)
{
    if (dynamic_cast<ASTReturnStatement*>(node))
        return true;

    if (auto* block = dynamic_cast<ASTBlockStatementNode*>(node))
    {
        for (const auto& statement : block->statements)
            if (always_returns(statement.get()))
                return true;
        return false;
    }

    if (auto* if_node = dynamic_cast<ASTIfStatementNode*>(node))
    {
        if (!if_node->false_statement) // not sure to enter in if
            return false;
        return always_returns(if_node->then_statement.get()) && // Two paths needs to return
            always_returns(if_node->false_statement.get());
    }

    return false;
}

void ControlFlowVisitor::visit(ASTMainProgramNode* node)
{
    for (const auto& func : node->functions)
        func->accept(this);
}

void ControlFlowVisitor::visit(ASTFunctionProgramNode* node)
{
    _current_is_void = node->return_type->type ==
        ASTTypeNode::VOID && node->return_type->pointer_depth == 0;

    if (!_current_is_void && !always_returns(node->statement.get()))
        flow_error(
            "Function '" + node->name + "' does not return a value on all paths",
            node->token
        );

    _loop_depth = 0;
    node->statement->accept(this);
}

void ControlFlowVisitor::visit(ASTBlockStatementNode* node)
{
    for (const auto& stmt : node->statements)
        stmt->accept(this);
}

void ControlFlowVisitor::visit(ASTIfStatementNode* node)
{
    node->then_statement->accept(this);
    if (node->false_statement)
        node->false_statement->accept(this);
}

void ControlFlowVisitor::visit(ASTWhileStatementNode* node)
{
    _loop_depth++;
    node->body_statement->accept(this);
    _loop_depth--;
}

void ControlFlowVisitor::visit(ASTForStatementNode* node)
{
    _loop_depth++;
    node->body_statement->accept(this);
    _loop_depth--;
}

void ControlFlowVisitor::visit(ASTReturnStatement*) {}

void ControlFlowVisitor::visit(ASTExpressionStatement* node)
{
    node->expression->accept(this);
}

void ControlFlowVisitor::visit(ASTVariableStatement*)
{
}

void ControlFlowVisitor::visit(ASTBreakStatement* node)
{
    if (_loop_depth == 0)
        flow_error("'break' outside of a loop", node->token);
}

void ControlFlowVisitor::visit(ASTContinueStatement* node)
{
    if (_loop_depth == 0)
        flow_error("'continue' outside of a loop", node->token);
}

void ControlFlowVisitor::visit(ASTAssignExpressionStatement* node)
{
    const bool valid_target =
        dynamic_cast<ASTIdentifierExpressionNode*>(node->target.get()) ||
        dynamic_cast<ASTDereferenceExpressionNode*>(node->target.get()) ||
        dynamic_cast<ASTIndexExpressionNode*>(node->target.get());

    if (!valid_target)
        flow_error("Left side of assignment is not assignable", node->token);
}
