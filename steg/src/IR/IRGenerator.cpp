//
// Created by Roumite on 19/03/2026.
//

#include "IRGenerator.h"

#include "semantic_analysis/utils/SymbolTable.h"

using namespace compiler;

int rank_of(IrValueType t)
{
    switch (t)
    {
    case IrValueType::BOOL:
    case IrValueType::UINT8: return 8;
    case IrValueType::UINT16: return 16;
    case IrValueType::UINT32:
    case IrValueType::FLOAT:
    case IrValueType::INT: return 32;
    default: return 0;
    }
}

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
    case ASTBinaryExpressionNode::FLOAT_ADDITION: return IrOpCode::FADD;
    case ASTBinaryExpressionNode::FLOAT_SUBTRACTION: return IrOpCode::FSUB;
    case ASTBinaryExpressionNode::FLOAT_MULTIPLICATION: return IrOpCode::FMUL;
    case ASTBinaryExpressionNode::FLOAT_DIVISION: return IrOpCode::FDIV;
    case ASTBinaryExpressionNode::FLOAT_MODULO: return IrOpCode::FMOD;

    case ASTBinaryExpressionNode::SIGNED_DIVISION: return IrOpCode::SDIV;
    case ASTBinaryExpressionNode::SIGNED_MULTIPLICATION: return IrOpCode::SMUL;

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
    case ASTUnaryExpressionNode::unaryOperationType::NEGATION: return IrOpCode::NEG;
    case ASTUnaryExpressionNode::unaryOperationType::FLOAT_NEGATION: return IrOpCode::FNEG;
    case ASTUnaryExpressionNode::unaryOperationType::NOT: return IrOpCode::NOT;
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
    case ASTAssignExpressionStatement::ADD_ASSIGN_FLOAT: return IrOpCode::FADD;
    case ASTAssignExpressionStatement::SUB_ASSIGN_FLOAT: return IrOpCode::FSUB;
    case ASTAssignExpressionStatement::MUL_ASSIGN_FLOAT: return IrOpCode::FMUL;
    case ASTAssignExpressionStatement::DIV_ASSIGN_FLOAT: return IrOpCode::FDIV;
    case ASTAssignExpressionStatement::DIV_ASSIGN_SIGNED: return IrOpCode::SDIV;
    case ASTAssignExpressionStatement::MUL_ASSIGN_SIGNED: return IrOpCode::SMUL;

    default: return IrOpCode::ADD;
    }
}

IrValueType IRGenerator::type_to_ptr_type(const ResolvedType& t)
{
    switch (t.base)
    {
    case ASTTypeNode::BOOL:
    case ASTTypeNode::UINT8: return IrValueType::PTR8;

    case ASTTypeNode::UINT16: return IrValueType::PTR16;

    case ASTTypeNode::FILE:
    case ASTTypeNode::CLOCK:
    case ASTTypeNode::UINT32:
    case ASTTypeNode::INT: return IrValueType::PTR32;
    default: return IrValueType::UNKNOWN;
    }
}

IrValueType IRGenerator::resolved_to_ir_type(const ResolvedType& t)
{
    if (t.pointer_depth > 0) return type_to_ptr_type(t);
    switch (t.base)
    {
    case ASTTypeNode::BOOL: return IrValueType::BOOL;
    case ASTTypeNode::UINT8: return IrValueType::UINT8;
    case ASTTypeNode::UINT16: return IrValueType::UINT16;
    case ASTTypeNode::FLOAT:  return IrValueType::FLOAT;
    case ASTTypeNode::FILE:
    case ASTTypeNode::CLOCK:
    case ASTTypeNode::UINT32: return IrValueType::UINT32;
    case ASTTypeNode::INT: return IrValueType::INT;
    default: return IrValueType::UNKNOWN;
    }
}

uint8_t IRGenerator::type_size_in_byte(const ResolvedType& t)
{
    switch (t.base)
    {
    case ASTTypeNode::BOOL:
    case ASTTypeNode::UINT8: return 1;
    case ASTTypeNode::UINT16: return 2;
    case ASTTypeNode::FILE:
    case ASTTypeNode::CLOCK:
    case ASTTypeNode::UINT32:
    case ASTTypeNode::INT:
    case ASTTypeNode::STRING:
    case ASTTypeNode::VOID:
    default: return 4;
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

static bool is_value_type_signed(IrValueType t)
{
    return t == IrValueType::INT;
}

IrOperand IRGenerator::ensure_type(IrOperand op, IrValueType target_type) {
    if (op.value_type == target_type || target_type == IrValueType::UNKNOWN) {
        return op;
    }

    if (op.type == IrOperandType::Constant) {
        op.value_type = target_type;
        return op;
    }

    IrOperand dest = temp_op(new_temp());
    dest.value_type = target_type;

    const bool src_float = op.value_type == IrValueType::FLOAT;
    const bool dst_float = target_type  == IrValueType::FLOAT;

    if (!src_float && dst_float) {
        // int/uint → float
        const IrOpCode cast = (op.value_type == IrValueType::INT) ? IrOpCode::ITOF : IrOpCode::UTOF;
        add_instruction({cast, dest, op});
        return dest;
    }

    if (src_float && !dst_float) {
        // float → int/uint
        const IrOpCode cast = (target_type == IrValueType::INT) ? IrOpCode::FTOI : IrOpCode::FTOU;
        add_instruction({cast, dest, op});
        return dest;
    }

    const int src_rank = rank_of(op.value_type);
    const int dst_rank = rank_of(target_type);

    if (src_rank > dst_rank)
        add_instruction({IrOpCode::TRUNC, dest, op});
    else
        add_instruction({IrOpCode::COPY,  dest, op});

    return dest;
}

// Pick the larger value of two IR value types
static IrValueType wider_type(IrValueType a, IrValueType b)
{
    if (is_value_type_signed(a) || is_value_type_signed(b))
        return IrValueType::INT;
    return rank_of(a) >= rank_of(b) ? a : b;
}

void IRGenerator::visit(ASTBinaryExpressionNode* node)
{
    auto left  = eval(node->left.get());
    auto right = eval(node->right.get());

    const IrValueType result_type = resolved_to_ir_type(node->resolved_type);
    const IrValueType left_type   = left.value_type;
    const IrValueType right_type  = right.value_type;

    const bool any_float = left_type == IrValueType::FLOAT || right_type == IrValueType::FLOAT;

    const bool is_comparison = [&] {
        using T = ASTBinaryExpressionNode::binaryOperationType;
        switch (node->op_type) {
        case T::COMPARISON_EQUAL:
        case T::COMPARISON_NOT_EQUAL:
        case T::COMPARISON_LESS:
        case T::COMPARISON_GREATER:
        case T::COMPARISON_LESS_OR_EQUAL:
        case T::COMPARISON_GREATER_OR_EQUAL: return true;
        default: return false;
        }
    }();

    if (any_float)
    {
        left = ensure_type(left, IrValueType::FLOAT);
        right = ensure_type(right, IrValueType::FLOAT);
    }
    else if (node->op_type == node->SIGNED_DIVISION || node->op_type == node->SIGNED_MULTIPLICATION)
    {
        left = ensure_type(left, IrValueType::INT);
        right = ensure_type(right, IrValueType::INT);
    } else
    {
        const IrValueType operand_target = is_comparison
            ? wider_type(left_type, right_type)
            : result_type;

        auto widen_if_needed = [&](IrOperand& op, IrValueType target)
        {
            if (op.value_type == target || op.value_type == IrValueType::UNKNOWN)
                return;

            IrOperand dest = temp_op(new_temp());
            dest.value_type = target;

            add_instruction({IrOpCode::COPY, dest, op});

            op = dest;
        };

        widen_if_needed(left,  operand_target);
        widen_if_needed(right, operand_target);
    }

    IrOperand dest = temp_op(new_temp());
    dest.value_type = result_type;

    IrOpCode opcode = IrOpCode::ADD;
    if (any_float && is_comparison)
    {
        // If is float -> use float comparisons
        using T = ASTBinaryExpressionNode::binaryOperationType;
        switch (node->op_type)
        {
        case T::COMPARISON_EQUAL: opcode = IrOpCode::FEQ;
            break;
        case T::COMPARISON_NOT_EQUAL: opcode = IrOpCode::FNEQ;
            break;
        case T::COMPARISON_LESS: opcode = IrOpCode::FLT;
            break;
        case T::COMPARISON_GREATER: opcode = IrOpCode::FGT;
            break;
        case T::COMPARISON_LESS_OR_EQUAL: opcode = IrOpCode::FLEQ;
            break;
        case T::COMPARISON_GREATER_OR_EQUAL: opcode = IrOpCode::FGEQ;
            break;
        default: opcode = IrOpCode::FEQ;
            break;
        }
    }
    else if (any_float) {
        opcode = binary_opcode(node->op_type, false);
    } else {
        const bool signed_op = is_value_type_signed(left_type) || is_value_type_signed(right_type);
        opcode = binary_opcode(node->op_type, signed_op);
    }

    add_instruction({opcode, dest, left, right});
    _current_operand = dest;
}

void IRGenerator::visit(ASTUnaryExpressionNode* node)
{
    const auto operand = eval(node->expression.get());
    IrOperand dest = temp_op(new_temp());
    dest.value_type = resolved_to_ir_type(node->resolved_type);
    add_instruction({unary_opcode(node->op_type), dest, operand});
    _current_operand = dest;
}

void IRGenerator::visit(ASTIndexExpressionNode* node)
{
    auto index = eval(node->index.get());
    uint8_t stride = type_size_in_byte(node->resolved_type);

    if (stride > 1) {
        IrOperand scaled_index = temp_op(new_temp());
        scaled_index.value_type = index.value_type;

        IrOperand stride_op = const_op(std::to_string(stride));
        stride_op.value_type = index.value_type;

        add_instruction({IrOpCode::MUL, scaled_index, index, stride_op});
        index = scaled_index;
    }

    IrOperand dest = temp_op(new_temp());
    dest.value_type = resolved_to_ir_type(node->resolved_type);

    add_instruction({IrOpCode::LOAD_ARR, dest, temp_op(node->array->name), index});

    _current_operand = dest;
}

void IRGenerator::visit(ASTCallExpressionNode* node)
{
    std::vector<IrOperand> args;
    for (std::size_t i = 0; i < node->args.size(); ++i)
    {
        auto arg = eval(node->args[i].get());
        if (node->resolved_symbol && i < node->resolved_symbol->param_types.size())
            arg = ensure_type(arg, resolved_to_ir_type(node->resolved_symbol->param_types[i]));
        args.push_back(arg);
    }

    IrInstruction instruction;
    instruction.call_args = std::move(args);

    const bool is_void = node->resolved_type.base == ASTTypeNode::Types::VOID
        && node->resolved_type.pointer_depth == 0;

    if (!is_void)
    {
        IrOperand res = temp_op(new_temp());
        res.value_type = resolved_to_ir_type(node->resolved_type);
        instruction.result = res;
        _current_operand = res;
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
    }
    add_instruction(std::move(instruction));
}

void IRGenerator::visit(ASTAssignExpressionStatement* node)
{
    auto val = eval(node->value.get());
    const IrValueType dest_type = resolved_to_ir_type(node->target->resolved_type);

    if (node->op == ASTAssignExpressionStatement::ASSIGN)
    {
        val = ensure_type(val, dest_type);
    } else
    {
        auto to = eval(node->target.get());
        val = ensure_type(val, to.value_type);
        IrOperand dest = temp_op(new_temp());
        dest.value_type = dest_type;
        add_instruction({composed_opcode(node->op), dest, to, val});
        val = dest;
    }

    if (const auto* idx = dynamic_cast<ASTIndexExpressionNode*>(node->target.get()))
    {
        auto index = eval(idx->index.get());

        int stride = type_size_in_byte(idx->array->resolved_type.deref());
        if (stride > 1) {
            IrOperand scaled = temp_op(new_temp());
            scaled.value_type = index.value_type;
            add_instruction({IrOpCode::MUL, scaled, index, const_op(std::to_string(stride))});
            index = scaled;
        }

        val = ensure_type(val, dest_type);

        add_instruction({IrOpCode::STORE_ARR, temp_op(idx->array->name), index, val});
    }
    else if (const auto* deref = dynamic_cast<ASTDereferenceExpressionNode*>(node->target.get()))
    {
        const auto ptr = eval(deref->expression.get());
        add_instruction({IrOpCode::DEREF_STORE, ptr, val});
    }
    else
    {
        const auto target = eval(node->target.get());
        add_instruction({IrOpCode::COPY, target, val});
    }

    _current_operand = val;
}

void IRGenerator::visit(ASTAddressOfExpressionNode* node)
{
    const auto operand = eval(node->expression.get());
    IrOperand dest = temp_op(new_temp());
    dest.value_type = resolved_to_ir_type(node->resolved_type);
    add_instruction({IrOpCode::ADDR_OF, dest, operand});
    _current_operand = dest;
}

void IRGenerator::visit(ASTDereferenceExpressionNode* node)
{
    const auto operand = eval(node->expression.get());
    IrOperand dest = temp_op(new_temp());
    dest.value_type = resolved_to_ir_type(node->resolved_type);
    add_instruction({IrOpCode::DEREF, dest, operand});
    _current_operand = dest;
}

/* Statements visitors */
void IRGenerator::visit(ASTVariableStatement* node)
{
    if (!_current_block) // It's a global variable
    {
        IrGlobal g;
        g.name = node->name;
        g.type = resolved_to_ir_type(ResolvedType::from(node->type));
        g.default_ast_type = node->type->type;

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
        auto val = eval(node->expression.get());
        const IrValueType dest_type = resolved_to_ir_type(ResolvedType::from(node->type));
        val = ensure_type(val, dest_type);
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

void IRGenerator::visit(ASTFileProgramNode *file)
{
    this->files.push_back({
        .name = file->name,
        .path = file->path,
        .absolute_path = file->absolute_path
    });
}
