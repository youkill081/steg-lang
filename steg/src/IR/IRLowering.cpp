//
// Created by Roumite on 20/03/2026.
//

#include "IRLowering.h"

#include <cassert>
#include <stdexcept>

using namespace compiler;

IRLowering::IRLowering(
    const std::vector<std::shared_ptr<IrBasicBlock>>& src_blocks,
    const std::vector<IrGlobal>& src_globals
) : _src_blocks(src_blocks), _src_globals(src_globals)
{
    for (const auto& global : src_globals)
    {
        _global_names.insert(global.name);
        _global_types[global.name] = global.type;
    }
    _str_count = 0;
}

void IRLowering::lower()
{
    lowered_globals = _src_globals;

    std::unordered_map<const IrBasicBlock*, std::shared_ptr<IrBasicBlock>> remap;
    remap.reserve(_src_blocks.size());

    for (const auto& src : _src_blocks)
    {
        auto dst = std::make_shared<IrBasicBlock>();
        dst->label = src->label;
        dst->is_function_entry = src->is_function_entry;
        dst->function_name = src->function_name;
        dst->parameters = src->parameters;

        remap[src.get()] = dst;
        lowered_blocks.push_back(dst);
    }

    for (const auto& src_ptr : _src_blocks)
    {
        const IrBasicBlock& src = *src_ptr;
        IrBasicBlock& dst = *remap.at(src_ptr.get());

        for (const auto& instr : src.instructions)
            lower_instruction(instr, dst.instructions);

        dst.terminator = src.terminator;

        switch (src.terminator)
        {
        case IrBlockTerminator::BRANCH:
            dst.condition_operand = lower_src(src.condition_operand, dst.instructions);
            break;

        case IrBlockTerminator::RETURN:
            dst.return_operand = lower_src(src.return_operand, dst.instructions);
            break;

        default:
            break;
        }

        if (const auto succ = src.successor.lock())
            dst.successor = remap.at(succ.get());

        if (const auto false_succ = src.false_successor.lock())
            dst.false_successor = remap.at(false_succ.get());
    }
}

/* Helper */
std::string IRLowering::new_temp()
{
    return "_lt" + std::to_string(_temp_count++);
}

IrOpCode IRLowering::load_opcode(IrValueType t)
{
    switch (t)
    {
    case IrValueType::BOOL:
    case IrValueType::UINT8:
    case IrValueType::INT8: return IrOpCode::LOAD_8;
    case IrValueType::UINT16:
    case IrValueType::INT16: return IrOpCode::LOAD_16;
    default: return IrOpCode::LOAD_32;
    }
}

IrOpCode IRLowering::store_opcode(IrValueType t)
{
    switch (t)
    {
    case IrValueType::BOOL:
    case IrValueType::UINT8:
    case IrValueType::INT8: return IrOpCode::STORE_8;
    case IrValueType::UINT16:
    case IrValueType::INT16: return IrOpCode::STORE_16;
    default: return IrOpCode::STORE_32;
    }
}

/* Lowered operands */
IrOperand IRLowering::lower_src(const IrOperand& op, std::vector<IrInstruction>& out)
{
    if (op.type != IrOperandType::Temporary || op.value.empty())
        return op;

    if (_global_names.count(op.value))
    {
        const IrValueType vt = (op.value_type != IrValueType::UNKNOWN)
                                   ? op.value_type
                                   : _global_types.count(op.value)
                                   ? _global_types.at(op.value)
                                   : IrValueType::UINT32;

        IrOperand dest = {IrOperandType::Temporary, new_temp(), vt};
        IrOperand addr = {IrOperandType::Temporary, op.value, vt};
        out.push_back({load_opcode(vt), dest, addr});
        return dest;
    }

    return op;
}

IrValueType IRLowering::effective_type(IrValueType vt, const std::string& name) const
{
    if (vt != IrValueType::UNKNOWN) return vt;
    const auto it = _global_types.find(name);
    return it != _global_types.end() ? it->second : IrValueType::UINT32;
}

/* Lowered instructions */
void IRLowering::lower_instruction(
    const IrInstruction& instr, std::vector<IrInstruction>& out)
{
    if (instr.op == IrOpCode::STORE_ARR)
    {
        auto base = lower_src(instr.result, out);
        auto index = lower_src(instr.arg1, out);
        auto value = lower_src(instr.arg2, out);

        out.push_back({IrOpCode::STORE_ARR, base, index, value});
    }
    else if (instr.op == IrOpCode::COPY)
    {
        const std::string& dest_name = instr.result.value;

        if (_global_names.count(dest_name)) // If dest is global -> store in memory
        {
            const IrValueType vt = effective_type(instr.result.value_type, dest_name);
            IrOperand addr = {IrOperandType::Temporary, dest_name, vt};
            auto val = lower_src(instr.arg1, out);
            out.push_back({store_opcode(vt), {}, addr, val});
        }
        else if (instr.arg1.type == IrOperandType::Constant // If the value is a table (like string)
            && ir_value_type_is_ptr(instr.arg1.value_type))
        {
            const std::string& str_val = instr.arg1.value;

            std::string mangled = "_str_" + std::to_string(_str_count++);
            _string_constants[str_val] = mangled;
            _global_names.insert(mangled);
            _global_types[mangled] = instr.arg1.value_type;
            lowered_globals.push_back({
                mangled, instr.arg1.value_type, ASTTypeNode::STRING,
                {IrOperandType::Constant, str_val, instr.arg1.value_type}
            });

            IrOperand mangled_op = {IrOperandType::Temporary, mangled, instr.arg1.value_type};
            out.push_back({IrOpCode::COPY, instr.result, mangled_op});
        } else // Just a simple copy
        {
            auto val = lower_src(instr.arg1, out);
            out.push_back({IrOpCode::COPY, instr.result, val});
        }
        return;
    }


    if (instr.op == IrOpCode::DEREF)
    {
        auto ptr = lower_src(instr.arg1, out);
        out.push_back({load_opcode(instr.result.value_type), instr.result, ptr});
        return;
    }

    if (instr.op == IrOpCode::DEREF_STORE)
    {
        auto ptr = lower_src(instr.result, out);
        auto val = lower_src(instr.arg1, out);

        const IrValueType vt = (val.value_type != IrValueType::UNKNOWN)
                                   ? val.value_type
                                   : IrValueType::UINT32;

        out.push_back({store_opcode(vt), {}, ptr, val});
        return;
    }

    IrInstruction lowered = instr;

    if (!instr.arg1.empty())
        lowered.arg1 = lower_src(instr.arg1, out);

    if (!instr.arg2.empty())
        lowered.arg2 = lower_src(instr.arg2, out);

    if (instr.op == IrOpCode::CALL || instr.op == IrOpCode::BUILTIN_CALL)
    {
        lowered.call_args.clear();
        for (const auto& arg : instr.call_args)
            lowered.call_args.push_back(lower_src(arg, out));
    }

    out.push_back(std::move(lowered));
}
