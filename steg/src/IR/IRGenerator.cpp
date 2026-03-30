//
// Created by Roumite on 19/03/2026.
//

#include "IRGenerator.h"

#include "semantic_analysis/utils/SymbolTable.h"

using namespace compiler;

/* Block utils */
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

/* Terminator */
void IRGenerator::terminate_jump(const std::shared_ptr<IrBasicBlock>& target) const
{
    if (is_terminated()) return;
    _current_block->terminator = IrBlockTerminator::JUMP;
    _current_block->successor = target;
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
    if (val.value.empty())
    {
        _current_block->terminator = IrBlockTerminator::RETURN_VOID;
    }
    else
    {
        _current_block->terminator = IrBlockTerminator::RETURN;
        _current_block->return_operand = std::move(val);
    }
}

IrOpCode IRGenerator::binary_opcode(const ASTBinaryExpressionNode::binaryOperationType op)
{
    switch (op)
    {
    case ASTBinaryExpressionNode::ADDITION: return IrOpCode::ADD;
    case ASTBinaryExpressionNode::SUBTRACTION: return IrOpCode::SUB;
    case ASTBinaryExpressionNode::MULTIPLICATION: return IrOpCode::MUL;
    case ASTBinaryExpressionNode::DIVISION: return IrOpCode::DIV;
    case ASTBinaryExpressionNode::MODULO: return IrOpCode::MOD;
    case ASTBinaryExpressionNode::COMPARISON_EQUAL: return IrOpCode::EQ;
    case ASTBinaryExpressionNode::COMPARISON_NOT_EQUAL: return IrOpCode::NEQ;
    case ASTBinaryExpressionNode::COMPARISON_LESS: return IrOpCode::LT;
    case ASTBinaryExpressionNode::COMPARISON_GREATER: return IrOpCode::GT;
    case ASTBinaryExpressionNode::COMPARISON_LESS_OR_EQUAL: return IrOpCode::LEQ;
    case ASTBinaryExpressionNode::COMPARISON_GREATER_OR_EQUAL: return IrOpCode::GEQ;
    case ASTBinaryExpressionNode::COMPARISON_AND: return IrOpCode::AND;
    case ASTBinaryExpressionNode::COMPARISON_OR: return IrOpCode::OR;
    }
    return IrOpCode::ADD;
}

IrOpCode IRGenerator::binary_opcode(const ASTBinaryExpressionNode::binaryOperationType op, bool is_signed)
{
    switch (op)
    {
    case ASTBinaryExpressionNode::COMPARISON_LESS: return is_signed ? IrOpCode::SLT : IrOpCode::LT;
    case ASTBinaryExpressionNode::COMPARISON_GREATER: return is_signed ? IrOpCode::SGT : IrOpCode::GT;
    case ASTBinaryExpressionNode::COMPARISON_LESS_OR_EQUAL: return is_signed ? IrOpCode::SLEQ : IrOpCode::LEQ;
    case ASTBinaryExpressionNode::COMPARISON_GREATER_OR_EQUAL: return is_signed ? IrOpCode::SGEQ : IrOpCode::GEQ;
    default: return binary_opcode(op);
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
    case ASTAssignExpressionStatement::ADD_ASSIGN: return IrOpCode::ADD;
    case ASTAssignExpressionStatement::SUB_ASSIGN: return IrOpCode::SUB;
    case ASTAssignExpressionStatement::MUL_ASSIGN: return IrOpCode::MUL;
    case ASTAssignExpressionStatement::DIV_ASSIGN: return IrOpCode::DIV;
    default: return IrOpCode::ADD;
    }
}

IrValueType IRGenerator::resolved_to_ir_type(const ResolvedType& t)
{
    if (t.pointer_depth > 0) return IrValueType::PTR;
    switch (t.base)
    {
    case ASTTypeNode::BOOL: return IrValueType::BOOL;
    case ASTTypeNode::UINT8: return IrValueType::UINT8;
    case ASTTypeNode::UINT16: return IrValueType::UINT16;
    case ASTTypeNode::UINT32: return IrValueType::UINT32;
    case ASTTypeNode::INT8: return IrValueType::INT8;
    case ASTTypeNode::INT16: return IrValueType::INT16;
    case ASTTypeNode::INT32: return IrValueType::INT32;
    case ASTTypeNode::FILE: return IrValueType::FILE;
    case ASTTypeNode::CLOCK: return IrValueType::CLOCK;
    default: return IrValueType::UNKNOWN;
    }
}

/* Visitor */
void IRGenerator::visit(ASTLiteralExpressionNode* node)
{
    const auto ir_type = resolved_to_ir_type(node->resolved_type);

    IrOperand dest = temp_op(new_temp());
    dest.value_type = ir_type;

    std::string val = node->value;
    if (ir_type == IrValueType::BOOL)
    {
        if (val == "true")
            val = "1";
        if (val == "false")
            val = "0";
    }


    IrOperand src = const_op(val);
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

// Pick the larger value of two IR value types
static IrValueType wider_type(IrValueType a, IrValueType b)
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

static bool is_value_type_signed(IrValueType t)
{
    return t == IrValueType::INT8
        || t == IrValueType::INT16
        || t == IrValueType::INT32;
}

void IRGenerator::visit(ASTBinaryExpressionNode* node)
{
    auto left = eval(node->left.get());
    auto right = eval(node->right.get());

    const IrValueType result_type = resolved_to_ir_type(node->resolved_type);
    const IrValueType left_type = left.value_type;
    const IrValueType right_type = right.value_type;

    const bool is_comparison = [&]
    {
        using T = ASTBinaryExpressionNode::binaryOperationType;
        switch (node->op_type)
        {
        case T::COMPARISON_EQUAL:
        case T::COMPARISON_NOT_EQUAL:
        case T::COMPARISON_LESS:
        case T::COMPARISON_GREATER:
        case T::COMPARISON_LESS_OR_EQUAL:
        case T::COMPARISON_GREATER_OR_EQUAL: return true;
        default: return false;
        }
    }();

    const IrValueType operand_target = is_comparison
                                           ? wider_type(left_type, right_type)
                                           : result_type;

    auto sext_if_needed = [&](IrOperand& op, IrValueType target)
    {
        if (op.value_type == target || op.value_type == IrValueType::UNKNOWN)
            return;
        IrOperand dest = temp_op(new_temp());
        dest.value_type = target;
        add_instruction({op.is_signed() ? IrOpCode::SEXT : IrOpCode::ZEXT, dest, op});
        op = dest;
    };

    sext_if_needed(left, operand_target);
    sext_if_needed(right, operand_target);

    IrOperand dest = temp_op(new_temp());
    dest.value_type = result_type;

    const bool signed_op = is_value_type_signed(left_type) || is_value_type_signed(right_type);
    add_instruction({binary_opcode(node->op_type, signed_op), dest, left, right});
    _current_operand = dest;
}

void IRGenerator::visit(ASTUnaryExpressionNode* node)
{
    const auto operand = eval(node->expression.get());
    const IrOperand dest = temp_op(new_temp());
    add_instruction({unary_opcode(node->op_type), dest, operand});
    _current_operand = dest;
}

void IRGenerator::visit(ASTIndexExpressionNode* node)
{
    const auto index = eval(node->index.get());
    const IrOperand dest = temp_op(new_temp());
    add_instruction({IrOpCode::LOAD_ARR, dest, temp_op(node->array->name), index});
    _current_operand = dest;
}

void IRGenerator::visit(ASTCallExpressionNode* node)
{
    std::vector<IrOperand> args;
    for (auto& arg : node->args)
        args.push_back(eval(arg.get()));

    IrInstruction instruction;
    instruction.call_args = std::move(args);

    const bool is_void = node->resolved_type.base == ASTTypeNode::Types::VOID
        && node->resolved_type.pointer_depth == 0;

    if (!is_void)
    {
        const auto t = new_temp();
        instruction.result = temp_op(t);
        _current_operand = temp_op(t);
    }
    else
    {
        instruction.result = {};
        _current_operand = {};
    }

    if (node->resolved_symbol && node->resolved_symbol->is_builtin)
    {
        if (!node->resolved_symbol->builtin_instruction.has_value())
            throw std::runtime_error(
                "built_in_not_implemented: '" + node->callee->name + "'");

        instruction.op = IrOpCode::BUILTIN_CALL;
        instruction.arg1 = label_op(*node->resolved_symbol->builtin_instruction);
        add_instruction(std::move(instruction));
    }
    else
    {
        instruction.op = IrOpCode::CALL;

        std::string final_label;
        if (node->resolved_symbol) {
            final_label = gen_function_label(node->resolved_symbol->source_file, node->callee->name);
        } else {
            final_label = node->callee->name;
        }

        instruction.arg1 = label_op(final_label);
        add_instruction(std::move(instruction));
    }
}

void IRGenerator::visit(ASTAssignExpressionStatement* node)
{
    auto val = eval(node->value.get());

    if (node->op != ASTAssignExpressionStatement::ASSIGN)
    {
        auto to = eval(node->target.get());
        IrOperand dest = temp_op(new_temp());
        add_instruction({composed_opcode(node->op), dest, to, val});
        val = dest;
    }

    if (const auto* idx = dynamic_cast<ASTIndexExpressionNode*>(node->target.get()))
    {
        // Place data at the right index
        const auto index = eval(idx->index.get());
        add_instruction({IrOpCode::STORE_ARR, temp_op(idx->array->name), index, val});
    }
    else if (const auto* deref = dynamic_cast<ASTDereferenceExpressionNode*>(node->target.get()))
    {
        // Place data at the pointer position
        const auto ptr = eval(deref->expression.get());
        add_instruction({IrOpCode::DEREF_STORE, ptr, val});
    }
    else
    {
        // Place data directly
        const auto target = eval(node->target.get());
        add_instruction({IrOpCode::COPY, target, val});
    }

    _current_operand = val;
}

void IRGenerator::visit(ASTAddressOfExpressionNode* node)
{
    const auto operand = eval(node->expression.get());
    IrOperand dest = temp_op(new_temp());
    add_instruction({IrOpCode::ADDR_OF, dest, operand});
    _current_operand = dest;
}

void IRGenerator::visit(ASTDereferenceExpressionNode* node)
{
    const auto operand = eval(node->expression.get());
    IrOperand dest = temp_op(new_temp());
    add_instruction({IrOpCode::DEREF, dest, operand});
    _current_operand = dest;
}

/* Statements visitors */
void IRGenerator::visit(ASTVariableStatement* node)
{
    if (!_current_block)
    {
        IrGlobal g;
        g.name = node->name;
        g.type = resolved_to_ir_type(ResolvedType::from(node->type));

        if (node->expression)
        {
            if (const auto* lit = dynamic_cast<ASTLiteralExpressionNode*>(node->expression.get()))
            {
                g.initial_value = const_op(lit->value);
                g.initial_value.value_type = g.type;
            }
        }
        globals.push_back(std::move(g));
        return;
    }

    if (node->expression)
    {
        const auto val = eval(node->expression.get());
        add_instruction({IrOpCode::COPY, temp_op(node->name), val});
    }
}

void IRGenerator::visit(ASTBlockStatementNode* node)
{
    for (const auto& statement : node->statements)
        statement->accept(this);
}

void IRGenerator::visit(ASTIfStatementNode* node)
{
    const auto cond = eval(node->condition.get());
    const auto then_block = new_block();
    const auto end_block = new_block();

    const bool has_else = (node->false_statement != nullptr);
    const auto else_block = has_else ? new_block() : end_block;

    terminate_branch(cond, then_block, else_block);

    _current_block = then_block;
    node->then_statement->accept(this);
    terminate_jump(end_block);

    if (has_else)
    {
        _current_block = else_block;
        node->false_statement->accept(this);
        terminate_jump(end_block);
    }

    _current_block = end_block;
}

void IRGenerator::visit(ASTWhileStatementNode* node)
{
    auto cond_block = new_block();
    auto body_block = new_block();
    auto end_block = new_block();

    loop_start_blocks_.push_back(cond_block);
    loop_end_blocks_.push_back(end_block);

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
    eval(node->init_expression.get());

    auto cond_block = new_block();
    auto body_block = new_block();
    auto post_block = new_block();
    auto end_block = new_block();

    loop_start_blocks_.push_back(post_block);
    loop_end_blocks_.push_back(end_block);

    terminate_jump(cond_block);

    _current_block = cond_block;
    const auto cond = eval(node->condition_expression.get());
    terminate_branch(cond, body_block, end_block);

    _current_block = body_block;
    node->body_statement->accept(this);
    terminate_jump(post_block);

    _current_block = post_block;
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

/* Program visitors */
void IRGenerator::visit(ASTFunctionProgramNode* node)
{
    _temp_count = 0;

    const std::string label = node->path.empty()
                                  ? node->name
                                  : gen_function_label(node->path, node->name);

    _current_block = new_block(label);

    _current_block->is_function_entry = true;
    _current_block->function_name = label;

    for (const auto& param : node->parameters)
        _current_block->parameters.push_back(param->name);

    node->statement->accept(this);
    terminate_return();
}
