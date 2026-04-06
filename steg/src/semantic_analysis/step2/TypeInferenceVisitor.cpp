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

    if (from.is_pointer() && to.is_pointer())
    {
        if (from.base == ASTTypeNode::VOID || to.base == ASTTypeNode::VOID) {
            return to;
        }
    }

    // int -> float
    if (from.is_numeric() && !from.is_float() && to.is_float())
        return to;

    // float -> int
    if (from.is_float() && to.is_numeric() && !to.is_float()) {
        type_hint(
            "Implicit conversion from float to " + to.to_string() + ": possible precision loss",
            token
        );
        return to;
    }

    if (from.is_numeric() && to.is_numeric()) {
        const ResolvedType promoted = promote(from, to);
        if (!promoted.is_void()) {
            if (from.bit_width() > to.bit_width())
                type_hint(
                    "Narrowing conversion from " + from.to_string() + " to "
                    + to.to_string() + ": possible data loss",
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
        type_error("Return type cannot be a pointer to this type", node->return_type->token);

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
        type_error("Parameter '" + node->name + "' cannot be a pointer to this type", node->token);
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
        type_error("Cannot declare a pointer to this type", node->token);
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

    if (node->type->type == ASTTypeNode::FLOAT) {
        node->resolved_type = ResolvedType::from(ASTTypeNode::FLOAT);
        return;
    }

    if (node->type->type == ASTTypeNode::INT) {
        const int64_t val = std::stoll(node->value);

        if (val < 0) {
            node->resolved_type = ResolvedType::from(ASTTypeNode::INT);
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

    const bool any_float = L.is_float() || R.is_float();
    const bool any_signed = L.is_signed() || R.is_signed();

    switch (node->op_type)
    {
    case Op::COMPARISON_AND:
    case Op::COMPARISON_OR:
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
        if (any_float)
            node->is_float_cmp = true;
        if (L != R)
        {
            const ResolvedType promoted = promote(L, R);
            if (promoted.is_void())
                type_error("Cannot compare " + L.to_string() + " with " + R.to_string(), node->token);
        }
        node->resolved_type = ResolvedType::from(ASTTypeNode::BOOL);
        break;

    case Op::ADDITION:
    case Op::SUBTRACTION:
    case Op::MULTIPLICATION:
    case Op::DIVISION:
    case Op::MODULO:
        if (L.base == ASTTypeNode::VOID)
            type_error("Cannot perform arithmetic on void", node->token);
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

        if (any_float)
        {
            switch (node->op_type)
            {
            case Op::ADDITION: node->op_type = Op::FLOAT_ADDITION;
                break;
            case Op::SUBTRACTION: node->op_type = Op::FLOAT_SUBTRACTION;
                break;
            case Op::MULTIPLICATION: node->op_type = Op::FLOAT_MULTIPLICATION;
                break;
            case Op::DIVISION: node->op_type = Op::FLOAT_DIVISION;
                break;
            case Op::MODULO: node->op_type = Op::FLOAT_MODULO;
                break;
            default: type_error("No float operand for this binary " + L.to_string() + " and " + R.to_string(), node->token);
            }
            node->resolved_type = ResolvedType::from(ASTTypeNode::FLOAT);
            break;
        } else if (any_signed)
        {
            switch (node->op_type)
            {
            case Op::MULTIPLICATION: node->op_type = Op::SIGNED_MULTIPLICATION;
                break;
            case Op::DIVISION: node->op_type = Op::SIGNED_DIVISION;
                break;
            }
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
                node->resolved_type = promoted;
            }
        }
        break;
    case ASTBinaryExpressionNode::FLOAT_ADDITION:
    case ASTBinaryExpressionNode::FLOAT_SUBTRACTION:
    case ASTBinaryExpressionNode::FLOAT_MULTIPLICATION:
    case ASTBinaryExpressionNode::FLOAT_DIVISION:
    case ASTBinaryExpressionNode::FLOAT_MODULO:
        node->resolved_type = ResolvedType::from(ASTTypeNode::FLOAT);
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
        if (t.is_float()) {
            node->op_type = Op::FLOAT_NEGATION;
            node->resolved_type = ResolvedType::from(ASTTypeNode::FLOAT);
            return;
        }
        if (!t.is_signed()) {
            const uint8_t w = t.bit_width();
            const ResolvedType signed_t = ResolvedType::from(ASTTypeNode::INT);
            if (!dynamic_cast<ASTLiteralExpressionNode *>(node->expression.get())) // Don't hint if user type '-5'
                type_hint("Negation of unsigned " + t.to_string() + " promoted to " + signed_t.to_string(), node->token);
            node->resolved_type = signed_t;
        } else {
            node->resolved_type = t;
        }
    } else if (node->op_type == Op::NOT)
    {
        const ResolvedType t = node->expression->resolved_type;

        if (is_opaque(t)) {
            type_error("Negation not allowed on opaque type: " + t.to_string(), node->token);
            node->resolved_type = ResolvedType::from(ASTTypeNode::VOID);
            return;
        }
        if (!t.is_bool() || t.is_numeric()) {
            type_error("Negation applied to non-numeric/bool type: " + t.to_string(), node->token);
            node->resolved_type = ResolvedType::from(ASTTypeNode::VOID);
            return;
        }
        node->resolved_type = ResolvedType::from(ASTTypeNode::BOOL);
    }
}

void TypeInferenceVisitor::visit(ASTAssignExpressionStatement* node)
{
    node->target->accept(this);
    node->value->accept(this);

    const ResolvedType target_t = node->target->resolved_type;
    const ResolvedType value_t  = node->value->resolved_type;

    const bool any_float = target_t.is_float() || value_t.is_float();

    if (any_float)
    {
        switch (node->op)
        {
        case ASTAssignExpressionStatement::ADD_ASSIGN: node->op = ASTAssignExpressionStatement::ADD_ASSIGN_FLOAT;
            break;
        case ASTAssignExpressionStatement::SUB_ASSIGN: node->op = ASTAssignExpressionStatement::SUB_ASSIGN_FLOAT;
            break;
        case ASTAssignExpressionStatement::MUL_ASSIGN: node->op = ASTAssignExpressionStatement::MUL_ASSIGN_FLOAT;
            break;
        case ASTAssignExpressionStatement::DIV_ASSIGN: node->op = ASTAssignExpressionStatement::DIV_ASSIGN_FLOAT;
            break;
        default:;
        }
    }

    const bool any_signed = target_t.is_signed() || value_t.is_signed();
    if (any_signed)
    {
        switch (node->op)
        {
        case ASTAssignExpressionStatement::MUL_ASSIGN: node->op = ASTAssignExpressionStatement::MUL_ASSIGN_SIGNED;
            break;
        case ASTAssignExpressionStatement::DIV_ASSIGN: node->op = ASTAssignExpressionStatement::DIV_ASSIGN_SIGNED;
        default:;
        }
    }


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
    else if (t.base == ASTTypeNode::VOID)
        type_error("Cannot dereference a void pointer", node->token);
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
