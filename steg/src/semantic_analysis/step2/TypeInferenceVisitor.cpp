//
// Created by Roumite on 18/03/2026.
//

#include "TypeInferenceVisitor.h"

using namespace compiler;

ResolvedType TypeInferenceVisitor::check_assignable(
    const ResolvedType& from,
    const ResolvedType& to,
    const LexerToken& token
    ) {
    if (from == to) return to;

    if (from.is_numeric() && to.is_numeric()) {
        const ResolvedType promoted = promote(from, to);
        if (!promoted.is_void()) {
            if (from.bit_width() > to.bit_width() || from.is_signed() && !to.is_signed())
                type_hint(
                    "Implicit conversion from " + from.to_string() + " to " + to.to_string(),
                    token
                );
            return to;
        }
    }

    type_error(
        "Type mismatch: expected " + to.to_string() + ", got " + from.to_string(),
        token
    );
    return ResolvedType::from(ASTTypeNode::VOID);
}

void TypeInferenceVisitor::visit(ASTMainProgramNode* node)
{
    _checking_globals = true;
    for (const auto &variable  : node->global_variables)
        variable->accept(this);
    _checking_globals = false;
    for (const auto &function : node->functions)
        function->accept(this);
}

void TypeInferenceVisitor::visit(ASTFunctionProgramNode* node)
{
    if (is_invalid_pointer(ResolvedType::from(node->return_type)))
        type_error("Return type cannot be a pointer to void", node->return_type->token);

    const ResolvedType previous = _current_return_type;
    _current_return_type = ResolvedType::from(node->return_type);

    for (const auto &parameter : node->parameters)
        parameter->accept(this);

    node->statement->accept(this);
    _current_return_type = previous;
}

void TypeInferenceVisitor::visit(ASTParameterProgramNode *node)
{
    const ResolvedType t = ResolvedType::from(node->type);

    if (t.is_void())
        type_error("Parameter '" + node->name + "' cannot be of type void", node->token);
    else if (is_invalid_pointer(t))
        type_error("Parameter '" + node->name + "' cannot be a pointer to void", node->token);
}


void TypeInferenceVisitor::visit(ASTBlockStatementNode* node)
{
    for (const auto &statement : node->statements)
        statement->accept(this);
}

void TypeInferenceVisitor::visit(ASTVariableStatement* node)
{
    const ResolvedType declared = ResolvedType::from(node->type);

    if (declared.is_void())
    {
        type_error("Cannot declare a variable of type void", node->token);
        return;
    }
    if (is_invalid_pointer(declared))
    {
        type_error("Cannot declare a pointer to void", node->token);
        return;
    }

    if (!node->expression)
        return;

    if (_checking_globals)
    {
        if (not dynamic_cast<ASTLiteralExpressionNode *>(node->expression.get()))
        {
            type_error("Global variable must be initialized with literal", node->token);
        }
    }
    else
    {
        node->expression->accept(this);
        check_assignable(node->expression->resolved_type, declared, node->expression->token);
    }
}

void TypeInferenceVisitor::visit(ASTIfStatementNode* node)
{
    node->condition->accept(this);

    const ResolvedType t = node->condition->resolved_type;
    if (!t.is_bool() && !t.is_numeric() && !t.is_pointer())
        type_error("Condition must be bool, numeric or pointer, got " + t.to_string(), node->condition->token);

    node->then_statement->accept(this);
    if (node->false_statement)
        node->false_statement->accept(this);
}

void TypeInferenceVisitor::visit(ASTWhileStatementNode* node)
{
    node->condition->accept(this);

    const ResolvedType t = node->condition->resolved_type;
    if (!t.is_bool() && !t.is_numeric() && !t.is_pointer())
        type_error("Condition must be bool, numeric or pointer, got " + t.to_string(), node->condition->token);

    node->body_statement->accept(this);
}

void TypeInferenceVisitor::visit(ASTForStatementNode* node)
{
    if (node->init_expression)
        node->init_expression->accept(this);
    if (node->condition_expression)
        node->condition_expression->accept(this);
    if (node->post_expression)
        node->post_expression->accept(this);
    node->body_statement->accept(this);
}

void TypeInferenceVisitor::visit(ASTReturnStatement* node)
{
    if (node->expression)
    {
        node->expression->accept(this);

        if (_current_return_type.is_void())
        {
            type_error("Cannot return a value from a void function", node->token);
            return;
        }
        check_assignable(node->expression->resolved_type, _current_return_type, node->expression->token);
    }
    else
    {
        if (not _current_return_type.is_void())
        {
            type_error("Missing return value, expected " + _current_return_type.to_string(), node->token);
        }
    }
}

void TypeInferenceVisitor::visit(ASTExpressionStatement* node)
{
    node->expression->accept(this);
}

/* Expressions */

void TypeInferenceVisitor::visit(ASTLiteralExpressionNode* node)
{
    // Compute the real literal type
    if (node->type->type == ASTTypeNode::STRING)
    {
        node->resolved_type = ResolvedType::string_literal();
        return;
    }


    if (node->type->type == ASTTypeNode::INT32) {
        const int64_t val = std::stoll(node->value);

        if (val < 0) {
            if (val >= -128) node->resolved_type = ResolvedType::from(ASTTypeNode::INT8);
            else if (val >= -32768) node->resolved_type = ResolvedType::from(ASTTypeNode::INT16);
            else node->resolved_type = ResolvedType::from(ASTTypeNode::INT32);
        } else {
            if (val <= 255)   node->resolved_type = ResolvedType::from(ASTTypeNode::UINT8);
            else if (val <= 65535) node->resolved_type = ResolvedType::from(ASTTypeNode::UINT16);
            else node->resolved_type = ResolvedType::from(ASTTypeNode::UINT32);
        }
        return;
    }

    node->resolved_type = ResolvedType::from(node->type);
}

void TypeInferenceVisitor::visit(ASTIdentifierExpressionNode* node)
{
    if (node->resolved_symbol)
        node->resolved_type = node->resolved_symbol->type;
}

void TypeInferenceVisitor::visit(ASTCallExpressionNode* node)
{
    for (auto &arg : node->args)
        arg->accept(this);

    if (!node->resolved_symbol) return;

    node->resolved_type = node->resolved_symbol->type;

    const auto& expected = node->resolved_symbol->param_types;
    if (node->args.size() != expected.size()) {
        type_error(
            "Wrong number of arguments: expected " + std::to_string(expected.size()) +
            ", got " + std::to_string(node->args.size()),
            node->token
        );
        return;
    }

    for (std::size_t i = 0; i < expected.size(); ++i)
        check_assignable(node->args[i]->resolved_type, expected[i], node->args[i]->token);
}

void TypeInferenceVisitor::visit(ASTBinaryExpressionNode* node)
{
    node->left->accept(this);
    node->right->accept(this);

    using Op = ASTBinaryExpressionNode::binaryOperationType;
    const ResolvedType L = node->left->resolved_type;
    const ResolvedType R = node->right->resolved_type;

    switch (node->op_type)
    {
    case Op::COMPARISON_AND:
    case Op::COMPARISON_OR:
        // Opérandes doivent être bool ou numeric, pas FILE/CLOCK/opaque
        if (is_opaque(L) || is_opaque(R))
            type_error("Logical operator not allowed on opaque types", node->token);
        node->resolved_type = ResolvedType::from(ASTTypeNode::BOOL);
        break;

    case Op::COMPARISON_EQUAL:
    case Op::COMPARISON_NOT_EQUAL:
    case Op::COMPARISON_LESS:
    case Op::COMPARISON_GREATER:
    case Op::COMPARISON_LESS_OR_EQUAL:
    case Op::COMPARISON_GREATER_OR_EQUAL:
        if (is_opaque(L) || is_opaque(R))
        {
            type_error("Comparison not allowed on opaque types (file, clock)", node->token);
            node->resolved_type = ResolvedType::from(ASTTypeNode::BOOL);
            break;
        }
        if (L != R)
        {
            const ResolvedType promoted = promote(L, R);
            if (promoted.is_void())
                type_error("Cannot compare " + L.to_string() + " with " + R.to_string(), node->token);
            else
                type_hint("Implicit promotion in comparison: " +
                          L.to_string() + " vs " + R.to_string() + " -> " + promoted.to_string(),
                          node->token);
        }
        node->resolved_type = ResolvedType::from(ASTTypeNode::BOOL);
        break;

    case Op::ADDITION:
    case Op::SUBTRACTION:
    case Op::MULTIPLICATION:
    case Op::DIVISION:
    case Op::MODULO:
        if (is_opaque(L) || is_opaque(R))
        {
            type_error("Arithmetic not allowed on opaque types (file, clock)", node->token);
            node->resolved_type = ResolvedType::from(ASTTypeNode::VOID);
            break;
        }
        if (L.is_pointer() && R.is_numeric())
        {
            node->resolved_type = L;
            break;
        }
        if (R.is_pointer() && L.is_numeric())
        {
            node->resolved_type = R;
            break;
        }

        {
            const ResolvedType promoted = promote(L, R);
            if (promoted.is_void())
            {
                type_error("Invalid operands for arithmetic: " + L.to_string() + " and " + R.to_string(), node->token);
                node->resolved_type = ResolvedType::from(ASTTypeNode::VOID);
            }
            else
            {
                if (L != R)
                    type_hint("Implicit promotion: " + L.to_string() + " and " +
                              R.to_string() + " -> " + promoted.to_string(), node->token);
                node->resolved_type = promoted;
            }
        }
        break;
    }
}

void TypeInferenceVisitor::visit(ASTUnaryExpressionNode* node)
{
    node->expression->accept(this);
    using Op = ASTUnaryExpressionNode::unaryOperationType;

    if (node->op_type == Op::NEGATION) {
        const ResolvedType t = node->expression->resolved_type;

        if (is_opaque(t)) {
            type_error("Negation not allowed on opaque type: " + t.to_string(), node->token);
            node->resolved_type = ResolvedType::from(ASTTypeNode::VOID);
            return;
        }
        if (!t.is_numeric()) {
            type_error("Negation applied to non-numeric type: " + t.to_string(), node->token);
            node->resolved_type = ResolvedType::from(ASTTypeNode::VOID);
            return;
        }
        if (!t.is_signed()) {
            const uint8_t w = t.bit_width();
            const ResolvedType signed_t = ResolvedType::from(
                w == 8  ? ASTTypeNode::INT8  :
                w == 16 ? ASTTypeNode::INT16 : ASTTypeNode::INT32
            );
            type_hint("Negation of unsigned " + t.to_string() + " promoted to " + signed_t.to_string(), node->token);
            node->resolved_type = signed_t;
        } else {
            node->resolved_type = t;
        }
    }
}

void TypeInferenceVisitor::visit(ASTAssignExpressionStatement* node)
{
    node->target->accept(this);
    node->value->accept(this);

    const ResolvedType target_t = node->target->resolved_type;
    const ResolvedType value_t  = node->value->resolved_type;

    check_assignable(value_t, target_t, node->value->token);
    node->resolved_type = target_t;
}

void TypeInferenceVisitor::visit(ASTAddressOfExpressionNode* node)
{
    node->expression->accept(this);
    const ResolvedType t = node->expression->resolved_type;

    if (t.is_void() || t.base == ASTTypeNode::FILE || t.base == ASTTypeNode::CLOCK) {
        type_error("Cannot take address of type " + t.to_string(), node->token);
        return;
    }
    node->resolved_type = t.addr_of();
}

void TypeInferenceVisitor::visit(ASTDereferenceExpressionNode* node)
{
    node->expression->accept(this);
    const ResolvedType t = node->expression->resolved_type;
    if (!t.is_pointer())
        type_error("Dereference of non-pointer type: " + t.to_string(), node->token);
    else
        node->resolved_type = t.deref();
}

void TypeInferenceVisitor::visit(ASTIndexExpressionNode* node)
{
    node->array->accept(this);
    node->index->accept(this);

    const ResolvedType arr_t = node->array->resolved_type;
    if (!arr_t.is_pointer())
        type_error("Index operator on non-pointer type: " + arr_t.to_string(), node->token);
    else
        node->resolved_type = arr_t.deref();

    if (!node->index->resolved_type.is_numeric())
        type_error("Array index must be numeric", node->index->token);
}
