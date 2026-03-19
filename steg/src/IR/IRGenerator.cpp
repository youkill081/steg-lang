//
// Created by Roumite on 19/03/2026.
//

#include "IRGenerator.h"

#include "semantic_analysis/utils/SymbolTable.h"

using namespace compiler;

std::shared_ptr<IrBasicBlock> IRGenerator::new_block(const std::string& label)
{
    auto block = std::make_shared<IrBasicBlock>();
    block->label = label.empty() ? ("Label_" + std::to_string(_label_count++)) : label;
    all_blocks.push_back(block);
    return block;
}

std::string IRGenerator::gen_function_label(std::filesystem::path path, const std::string& name)
{
    std::string result;

    for (const auto& part : path)
    {
        std::string s = part.stem().string();
        for (auto& c : s)
            if (!std::isalnum(c)) c = '_';
        if (!s.empty())
            result += s + "__";
    }

    result += name;
    return result;
}

void IRGenerator::terminate_jump(const std::shared_ptr<IrBasicBlock>& target) const
{
    if (is_terminated())
        return;
    _current_block->terminator = IrBlockTerminator::JUMP;
    _current_block->successor  = target;
}

void IRGenerator::terminate_branch(IrOperand cond, const std::shared_ptr<IrBasicBlock>& true_block,
    const std::shared_ptr<IrBasicBlock>& false_block) const
{
    if (is_terminated())
        return;
    _current_block->terminator = IrBlockTerminator::BRANCH;
    _current_block->condition_operand = std::move(cond);
    _current_block->successor = true_block;
    _current_block->false_successor = false_block;
}

void IRGenerator::terminate_return(IrOperand val) const
{
    if (is_terminated())
        return;
    if (val.value.empty()) {
        _current_block->terminator = IrBlockTerminator::RETURN_VOID;
    } else {
        _current_block->terminator = IrBlockTerminator::RETURN;
        _current_block->return_operand = std::move(val);
    }
}

IrOpCode IRGenerator::binary_opcode(const ASTBinaryExpressionNode::binaryOperationType op)
{
    switch (op)
    {
    case ASTBinaryExpressionNode::binaryOperationType::ADDITION:
        return IrOpCode::ADD;
    case ASTBinaryExpressionNode::binaryOperationType::SUBTRACTION:
        return IrOpCode::SUB;
    case ASTBinaryExpressionNode::binaryOperationType::MULTIPLICATION:
        return IrOpCode::MUL;
    case ASTBinaryExpressionNode::binaryOperationType::DIVISION:
        return IrOpCode::DIV;
    case ASTBinaryExpressionNode::binaryOperationType::MODULO:
        return IrOpCode::MOD;
    case ASTBinaryExpressionNode::binaryOperationType::COMPARISON_EQUAL:
        return IrOpCode::EQ;
    case ASTBinaryExpressionNode::binaryOperationType::COMPARISON_NOT_EQUAL:
        return IrOpCode::NEQ;
    case ASTBinaryExpressionNode::binaryOperationType::COMPARISON_LESS:
        return IrOpCode::LT;
    case ASTBinaryExpressionNode::binaryOperationType::COMPARISON_GREATER:
        return IrOpCode::GT;
    case ASTBinaryExpressionNode::binaryOperationType::COMPARISON_LESS_OR_EQUAL:
        return IrOpCode::LEQ;
    case ASTBinaryExpressionNode::binaryOperationType::COMPARISON_GREATER_OR_EQUAL:
        return IrOpCode::GEQ;
    case ASTBinaryExpressionNode::binaryOperationType::COMPARISON_AND:
        return IrOpCode::AND;
    case ASTBinaryExpressionNode::binaryOperationType::COMPARISON_OR: return
            IrOpCode::OR;
    }
    return IrOpCode::ADD;
}

IrOpCode IRGenerator::binary_opcode(const ASTBinaryExpressionNode::binaryOperationType op, bool is_signed)
{
    switch (op)
    {
    case ASTBinaryExpressionNode::COMPARISON_LESS:
        return is_signed ? IrOpCode::SLT : IrOpCode::LT;
    case ASTBinaryExpressionNode::COMPARISON_GREATER:
        return is_signed ? IrOpCode::SGT : IrOpCode::GT;
    case ASTBinaryExpressionNode::COMPARISON_LESS_OR_EQUAL:
        return is_signed ? IrOpCode::SLEQ : IrOpCode::LEQ;
    case ASTBinaryExpressionNode::COMPARISON_GREATER_OR_EQUAL:
        return is_signed ? IrOpCode::SGEQ : IrOpCode::GEQ;
    default:
        return binary_opcode(op);
    }
}

IrOpCode IRGenerator::unary_opcode(const ASTUnaryExpressionNode::unaryOperationType op)
{
    switch (op)
    {
    case ASTUnaryExpressionNode::unaryOperationType::NEGATION:
        return IrOpCode::NEG;
    }
    return IrOpCode::NEG;
}

IrOpCode IRGenerator::composed_opcode(const ASTAssignExpressionStatement::assignmentType op)
{
    switch (op)
    {
    case ASTAssignExpressionStatement::assignmentType::ADD_ASSIGN:
        return IrOpCode::ADD;
    case ASTAssignExpressionStatement::assignmentType::SUB_ASSIGN:
        return IrOpCode::SUB;
    case ASTAssignExpressionStatement::assignmentType::MUL_ASSIGN:
        return IrOpCode::MUL;
    case ASTAssignExpressionStatement::assignmentType::DIV_ASSIGN:
        return IrOpCode::DIV;
    default:
        return IrOpCode::ADD;
    }
}

IrValueType IRGenerator::resolved_to_ir_type(const ResolvedType& t)
{
    if (t.pointer_depth > 0) return IrValueType::PTR;
    switch (t.base)
    {
    case ASTTypeNode::Types::BOOL: return IrValueType::BOOL;
    case ASTTypeNode::Types::UINT8: return IrValueType::UINT8;
    case ASTTypeNode::Types::UINT16: return IrValueType::UINT16;
    case ASTTypeNode::Types::UINT32: return IrValueType::UINT32;
    case ASTTypeNode::Types::INT8: return IrValueType::INT8;
    case ASTTypeNode::Types::INT16: return IrValueType::INT16;
    case ASTTypeNode::Types::INT32: return IrValueType::INT32;
    default: return IrValueType::UNKNOWN;
    }
}

void IRGenerator::visit(ASTLiteralExpressionNode* node)
{
    const auto t = new_temp();
    const auto ir_type = resolved_to_ir_type(node->resolved_type);
    IrOperand dest = temp_op(t);
    dest.value_type = ir_type;

    IrOperand src = const_op(node->value);
    src.value_type = ir_type;

    add_instruction({IrOpCode::COPY, dest, src});
    _current_operand = dest;
}

void IRGenerator::visit(ASTIdentifierExpressionNode* node)
{
    IrOperand op = temp_op(node->name);
    op.value_type = resolved_to_ir_type(node->resolved_type);
    _current_operand = op;
}

IrValueType wider_type(IrValueType a, IrValueType b)
{
    auto rank = [](IrValueType t) -> int
    {
        switch (t)
        {
        case IrValueType::BOOL:
        case IrValueType::UINT8:
        case IrValueType::INT8: return 8;
        case IrValueType::UINT16:
        case IrValueType::INT16: return 16;
        case IrValueType::UINT32:
        case IrValueType::INT32: return 32;
        default: return 0;
        }
    };
    return rank(a) >= rank(b) ? a : b;
}

bool is_value_type_signed(IrValueType t)
{
    return t == IrValueType::INT8
        || t == IrValueType::INT16
        || t == IrValueType::INT32;
}

void IRGenerator::visit(ASTBinaryExpressionNode* node)
{
    auto left  = eval(node->left.get());
    auto right = eval(node->right.get());

    const IrValueType result_type   = resolved_to_ir_type(node->resolved_type);
    const IrValueType left_type     = left.value_type;
    const IrValueType right_type    = right.value_type;

    const bool is_comparison = [&] {
        using T = ASTBinaryExpressionNode::binaryOperationType;
        switch (node->op_type) {
            case T::COMPARISON_EQUAL:
            case T::COMPARISON_NOT_EQUAL:
            case T::COMPARISON_LESS:
            case T::COMPARISON_GREATER:
            case T::COMPARISON_LESS_OR_EQUAL:
            case T::COMPARISON_GREATER_OR_EQUAL:
                return true;
            default: return false;
        }
    }();

    const IrValueType operand_target = is_comparison
        ? wider_type(left_type, right_type)
        : result_type;

    auto sext_if_needed = [&](IrOperand& op, IrValueType target) {
        if (op.value_type == target || op.value_type == IrValueType::UNKNOWN)
            return;
        const auto t = new_temp();
        IrOperand dest = temp_op(t);
        dest.value_type = target;
        add_instruction({op.is_signed() ? IrOpCode::SEXT : IrOpCode::ZEXT, dest, op});
        op = dest;
    };

    sext_if_needed(left, operand_target);
    sext_if_needed(right, operand_target);

    const auto t = new_temp();
    IrOperand dest = temp_op(t);
    dest.value_type = result_type;

    const bool signed_op = is_value_type_signed(left_type) || is_value_type_signed(right_type);
    add_instruction({binary_opcode(node->op_type, signed_op), dest, left, right});
    _current_operand = dest;
}

void IRGenerator::visit(ASTUnaryExpressionNode* node)
{
    const auto operand = eval(node->expression.get()); // t0 = !t1
    const auto t = new_temp();
    add_instruction({unary_opcode(node->op_type), temp_op(t), operand});
    _current_operand = temp_op(t);
}

void IRGenerator::visit(ASTIndexExpressionNode* node)
{
    const auto index = eval(node->index.get());
    const auto t = new_temp();
    add_instruction({IrOpCode::LOAD_ARR, temp_op(t), temp_op(node->array->name), index});
    _current_operand = temp_op(t);
}

void IRGenerator::visit(ASTCallExpressionNode* node)
{
    std::vector<IrOperand> args;
    for (auto& arg : node->args)
        args.push_back(eval(arg.get()));

    IrInstruction instruction;
    instruction.op = IrOpCode::CALL;
    const std::string call_label = (node->resolved_symbol && !node->resolved_symbol->source_file.empty())
                                       ? gen_function_label(node->resolved_symbol->source_file, node->callee->name)
                                       : node->callee->name;
    instruction.arg1 = label_op(call_label);

    instruction.call_args = std::move(args);

    const bool is_void = node->resolved_type.base == ASTTypeNode::Types::VOID
        && node->resolved_type.pointer_depth == 0;

    if (is_void)
    {
        instruction.result = {};
        _current_operand = {};
    }
    else
    {
        const auto t = new_temp();
        instruction.result = temp_op(t);
        _current_operand = temp_op(t);
    }

    add_instruction(std::move(instruction));
}

void IRGenerator::visit(ASTAssignExpressionStatement* node)
{
    auto val = eval(node->value.get());

    if (node->op != ASTAssignExpressionStatement::ASSIGN) {
        auto to = eval(node->target.get());
        const auto t = new_temp();
        // If it's composed operation (+=, -=, *= of /=) make the calcul before
        add_instruction({composed_opcode(node->op), temp_op(t), to, val});
        val = temp_op(t);
    }

    if (const auto *idx = dynamic_cast<ASTIndexExpressionNode*>(node->target.get())) {
        // Place data at the right index
        const auto index = eval(idx->index.get());
        add_instruction({IrOpCode::STORE_ARR, temp_op(idx->array->name), index, val});
    } else if (const auto *deref = dynamic_cast<ASTDereferenceExpressionNode*>(node->target.get())) {
        // Place data at the pointer position
        const auto ptr = eval(deref->expression.get());
        add_instruction({IrOpCode::DEREF_STORE, ptr, val});
    } else {
        // Place data directly
        const auto target = eval(node->target.get());
        add_instruction({IrOpCode::COPY, target, val});
    }

    _current_operand = val;
}

void IRGenerator::visit(ASTAddressOfExpressionNode* node)
{
    const auto operand = eval(node->expression.get());
    const auto t = new_temp();
    add_instruction({IrOpCode::ADDR_OF, temp_op(t), operand});
    _current_operand = temp_op(t);
}

void IRGenerator::visit(ASTDereferenceExpressionNode* node)
{
    const auto operand = eval(node->expression.get());
    const auto t = new_temp();
    add_instruction({IrOpCode::DEREF, temp_op(t), operand});
    _current_operand = temp_op(t);
}

void IRGenerator::visit(ASTVariableStatement* node)
{
    if (!_current_block) {
        IrGlobal g;
        g.name = node->name;
        if (node->expression) {
            if (const auto *lit = dynamic_cast<ASTLiteralExpressionNode*>(node->expression.get()))
                g.initial_value = const_op(lit->value);
        }
        globals.push_back(std::move(g));
        return;
    }

    if (node->expression) { // Normal variable initialisation
        const auto val = eval(node->expression.get());
        add_instruction({IrOpCode::COPY, temp_op(node->name), val});
    }
}

void IRGenerator::visit(ASTBlockStatementNode* node)
{
    for (const auto &statement : node->statements)
        statement->accept(this);
}

void IRGenerator::visit(ASTIfStatementNode* node)
{
    const auto cond = eval(node->condition.get()); // Compute condition statement
    const auto then_block = new_block(); // If the statement is true
    const auto end_block = new_block(); // Block after the if

    const bool has_else = (node->false_statement != nullptr);
    const auto else_block = has_else ? new_block() : end_block; // Facultative else block

    terminate_branch(cond, then_block, else_block);

    _current_block = then_block;
    node->then_statement->accept(this); // File if block
    terminate_jump(end_block);

    if (has_else) {
        _current_block = else_block;
        node->false_statement->accept(this); // Fill else block
        terminate_jump(end_block);
    }

    _current_block = end_block;
}

void IRGenerator::visit(ASTWhileStatementNode* node)
{
    auto cond_block = new_block(); // Condition is a block computed it's loop iteration
    auto body_block = new_block();
    auto end_block = new_block(); // After the while block

    loop_start_blocks_.push_back(cond_block); // For continue
    loop_end_blocks_.push_back(end_block); // For break

    terminate_jump(cond_block);

    _current_block = cond_block;
    const auto cond = eval(node->condition.get());
    terminate_branch(cond, body_block, end_block);

    _current_block = body_block;
    node->body_statement->accept(this);
    terminate_jump(cond_block);

    _current_block = end_block;

    loop_start_blocks_.pop_back();
    loop_end_blocks_.pop_back();
}

void IRGenerator::visit(ASTForStatementNode* node)
{
    eval(node->init_expression.get()); // Add init condition in current block

    auto cond_block = new_block(); // Block for condition
    auto body_block = new_block(); // Block for body
    auto post_block = new_block(); // First for parameter
    auto end_block  = new_block(); // After the for loop

    loop_start_blocks_.push_back(post_block); // For continue statement
    loop_end_blocks_.push_back(end_block); // For break statement

    terminate_jump(cond_block);

    _current_block = cond_block; // Compute condition block
    const auto cond = eval(node->condition_expression.get());
    terminate_branch(cond, body_block, end_block);

    _current_block = body_block; // Compute body block
    node->body_statement->accept(this);
    terminate_jump(post_block);

    _current_block = post_block; // Compute post block
    eval(node->post_expression.get());
    terminate_jump(cond_block);

    _current_block = end_block;

    loop_start_blocks_.pop_back();
    loop_end_blocks_.pop_back();
}

void IRGenerator::visit(ASTReturnStatement* node)
{
    if (node->expression)
        terminate_return(eval(node->expression.get()));
    else
        terminate_return();
}

void IRGenerator::visit(ASTBreakStatement*)
{
    if (const auto target = loop_end_blocks_.back().lock())
        terminate_jump(target);
}

void IRGenerator::visit(ASTContinueStatement*)
{
    if (const auto target = loop_start_blocks_.back().lock())
        terminate_jump(target);
}

void IRGenerator::visit(ASTFunctionProgramNode* node)
{
    _temp_count = 0;
    const std::string label = node->path.empty()
                                  ? node->name
                                  : gen_function_label(node->path, node->name);
    _current_block = new_block(label);
    node->statement->accept(this);
    terminate_return();
}


