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

    for (size_t i = 0; i < _src_blocks.size(); ++i)
    {
        if (!_src_blocks[i]->is_function_entry)
            continue;
        _current_function_name = _src_blocks[i]->function_name;
        collect_address_taken(*_src_blocks[i], i);
    }

    for (const auto& src_ptr : _src_blocks)
    {
        const IrBasicBlock& src = *src_ptr;
        IrBasicBlock& dst = *remap.at(src_ptr.get());

        if (src.is_function_entry)
            _current_function_name = src.function_name;

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

void IRLowering::collect_address_taken(const IrBasicBlock& entry, size_t start_index)
{
    auto scan_block = [&](const IrBasicBlock& block)
    {
        for (const auto& instr : block.instructions)
        {
            if (instr.op != IrOpCode::ADDR_OF) continue;

            const std::string& var = instr.arg1.value;
            if (var.empty() || _global_names.count(var))
                continue;

            const std::string key = _current_function_name + "::" + var;
            if (_address_taken.count(key))
                continue;

            const std::string mangled = mangle_local(_current_function_name, var);
            _address_taken[key] = mangled;

            IrValueType vt = instr.arg1.value_type != IrValueType::UNKNOWN
                ? instr.arg1.value_type
                : IrValueType::UINT32;

            /* set as global */
            _global_names.insert(mangled);
            _global_types[mangled] = vt;
            lowered_globals.push_back({
                mangled,
                vt,
                ASTTypeNode::UINT32,
                {}
            });
        }
    };

    scan_block(entry);
    for (size_t i = start_index + 1; i < _src_blocks.size(); ++i)
    {
        if (_src_blocks[i]->is_function_entry)
            break;
        scan_block(*_src_blocks[i]);
    }

}

std::string IRLowering::mangle_local(const std::string& fn, const std::string& var)
{
    return fn + "__local__" + var;
}

bool IRLowering::is_address_taken(const std::string& var) const
{
    return _address_taken.count(_current_function_name + "::" + var) > 0;
}

std::string IRLowering::remap_name(const std::string& var) const
{
    const auto key = _current_function_name + "::" + var;
    const auto it = _address_taken.find(key);
    return it != _address_taken.end() ? it->second : var;
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
    case IrValueType::PTR8: return IrOpCode::LOAD_8;
    case IrValueType::UINT16:
    case IrValueType::PTR16: return IrOpCode::LOAD_16;
    default: return IrOpCode::LOAD_32;
    }
}

IrOpCode IRLowering::store_opcode(IrValueType t)
{
    switch (t)
    {
    case IrValueType::BOOL:
    case IrValueType::UINT8:
    case IrValueType::PTR8: return IrOpCode::STORE_8;
    case IrValueType::UINT16:
    case IrValueType::PTR16: return IrOpCode::STORE_16;
    default: return IrOpCode::STORE_32;
    }
}

/* Lowered operands */
IrOperand IRLowering::lower_src(const IrOperand& op, std::vector<IrInstruction>& out)
{
    if (op.type != IrOperandType::Temporary || op.value.empty())
        return op;

    const std::string remapped = remap_name(op.value);
    if (remapped != op.value)
    {
        IrOperand remapped_op = op;
        remapped_op.value = remapped;
        return lower_src(remapped_op, out);
    }


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
    else if (instr.op == IrOpCode::ADDR_OF)
    {
        const std::string mangled = remap_name(instr.arg1.value);
        IrOperand mangled_op = instr.arg1;
        mangled_op.value = mangled;

        out.push_back({IrOpCode::ADDR_OF, instr.result, mangled_op});
        return;

    }
    else if (instr.op == IrOpCode::COPY)
    {
        const std::string dest_name = remap_name(instr.result.value);
        IrOperand remapped_result = instr.result;
        remapped_result.value = dest_name;

        if (_global_names.count(dest_name))
        {
            const IrValueType vt = effective_type(remapped_result.value_type, dest_name);
            IrOperand addr = {IrOperandType::Temporary, dest_name, vt};
            auto val = lower_src(instr.arg1, out);
            out.push_back({store_opcode(vt), {}, addr, val});
        }
        else if (instr.arg1.type == IrOperandType::Constant
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
            out.push_back({IrOpCode::COPY, remapped_result, mangled_op});
        }
        else
        {
            auto val = lower_src(instr.arg1, out);
            out.push_back({IrOpCode::COPY, remapped_result, val});
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

        IrValueType vt;
        switch (instr.result.value_type)
        {
        case IrValueType::PTR8: vt = IrValueType::UINT8;
            break;
        case IrValueType::PTR16: vt = IrValueType::UINT16;
            break;
        default: vt = IrValueType::UINT32;
            break;
        }

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
