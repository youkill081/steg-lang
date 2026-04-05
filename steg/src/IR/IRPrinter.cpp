//
// Created by Roumite on 19/03/2026.
//

#include "IRPrinter.h"

#include <sstream>

using namespace compiler;

std::string IRPrinter::format_value_type(const IrValueType t)
{
    switch (t)
    {
    case IrValueType::BOOL: return "bool";
    case IrValueType::UINT8: return "u8";
    case IrValueType::UINT16: return "u16";
    case IrValueType::UINT32: return "u32";
    case IrValueType::INT: return "i32";
    case IrValueType::PTR8: return "ptr8";
    case IrValueType::PTR16: return "ptr16";
    case IrValueType::PTR32: return "ptr32";
    case IrValueType::FILE: return "file";
    case IrValueType::CLOCK: return "clock";
    case IrValueType::UNKNOWN: return "?";
    }
    return "?";
}

std::string IRPrinter::format_operand(const IrOperand& op)
{
    if (op.value.empty()) return "_";

    const std::string type_suffix = (op.value_type != IrValueType::UNKNOWN)
                                        ? (":" + format_value_type(op.value_type))
                                        : "";

    switch (op.type)
    {
    case IrOperandType::Constant: return op.value + type_suffix;
    case IrOperandType::Label: return "@" + op.value;
    case IrOperandType::Temporary: return op.value + type_suffix;
    }
    return "?";
}

std::string IRPrinter::format_instruction(const IrInstruction& i)
{
    std::ostringstream out;
    const auto res = format_operand(i.result);
    const auto a1 = format_operand(i.arg1);
    const auto a2 = format_operand(i.arg2);

    switch (i.op)
    {
    case IrOpCode::ADD: out << res << " = " << a1 << " + " << a2;
        break;
    case IrOpCode::SUB: out << res << " = " << a1 << " - " << a2;
        break;
    case IrOpCode::MUL: out << res << " = " << a1 << " * " << a2;
        break;
    case IrOpCode::DIV: out << res << " = " << a1 << " / " << a2;
        break;
    case IrOpCode::MOD: out << res << " = " << a1 << " % " << a2;
        break;
    case IrOpCode::NEG: out << res << " = -" << a1;
        break;
    case IrOpCode::NOT: out << res << " = !" << a1;
        break;

    case IrOpCode::EQ: out << res << " = " << a1 << " == " << a2;
        break;
    case IrOpCode::NEQ: out << res << " = " << a1 << " != " << a2;
        break;
    case IrOpCode::LT: out << res << " = " << a1 << " <u " << a2;
        break;
    case IrOpCode::GT: out << res << " = " << a1 << " >u " << a2;
        break;
    case IrOpCode::LEQ: out << res << " = " << a1 << " <=u " << a2;
        break;
    case IrOpCode::GEQ: out << res << " = " << a1 << " >=u " << a2;
        break;

    case IrOpCode::SLT: out << res << " = " << a1 << " <s " << a2;
        break;
    case IrOpCode::SGT: out << res << " = " << a1 << " >s " << a2;
        break;
    case IrOpCode::SLEQ: out << res << " = " << a1 << " <=s " << a2;
        break;
    case IrOpCode::SGEQ: out << res << " = " << a1 << " >=s " << a2;
        break;

    case IrOpCode::AND: out << res << " = " << a1 << " && " << a2;
        break;
    case IrOpCode::OR: out << res << " = " << a1 << " || " << a2;
        break;

    case IrOpCode::TRUNC: out << res << " = trunc " << a1;
        break;
    case IrOpCode::ZEXTEND: out << res << " = zextend " << a1;
        break;

    case IrOpCode::COPY: out << res << " = " << a1;
        break;
    case IrOpCode::ADDR_OF: out << res << " = &" << a1;
        break;

    case IrOpCode::LOAD_ARR: out << res << " = " << a1 << "[" << a2 << "]";
        break;
    case IrOpCode::STORE_ARR: out << res << "[" << a1 << "] = " << a2;
        break;

    case IrOpCode::LOAD_8: out << res << " = LOAD.8  " << a1;
        break;
    case IrOpCode::LOAD_16: out << res << " = LOAD.16 " << a1;
        break;
    case IrOpCode::LOAD_32: out << res << " = LOAD.32 " << a1;
        break;

    case IrOpCode::STORE_8: out << "STORE.8  " << a1 << ", " << a2;
        break;
    case IrOpCode::STORE_16: out << "STORE.16 " << a1 << ", " << a2;
        break;
    case IrOpCode::STORE_32: out << "STORE.32 " << a1 << ", " << a2;
        break;

    case IrOpCode::SPILL_SAVE: out << "SPILL_SAVE " << a1;
        break;
    case IrOpCode::SPILL_RESTORE: out << "SPILL_RESTORE " << a1;
        break;

    case IrOpCode::DEREF: out << res << " = *" << a1;
        break;
    case IrOpCode::DEREF_STORE: out << "*" << res << " = " << a1;
        break;

    case IrOpCode::CALL:
    case IrOpCode::BUILTIN_CALL:
        {
            if (!i.result.empty())
                out << res << " = ";
            out << "call " << a1 << "(";
            for (std::size_t k = 0; k < i.call_args.size(); ++k)
            {
                if (k > 0) out << ", ";
                out << format_operand(i.call_args[k]);
            }
            out << ")";
            break;
        }
    }

    out << " [instr -> " << i.instr_nbr << "]";

    return out.str();
}

std::string IRPrinter::format_terminator(const IrBasicBlock& block)
{
    switch (block.terminator)
    {
    case IrBlockTerminator::JUMP:
        {
            const auto target = block.successor.lock();
            return "jump -> " + (target ? target->label : "???");
        }
    case IrBlockTerminator::BRANCH:
        {
            const auto t = block.successor.lock();
            const auto f = block.false_successor.lock();
            return "branch " + format_operand(block.condition_operand)
                + " -> " + (t ? t->label : "???")
                + " | " + (f ? f->label : "???");
        }
    case IrBlockTerminator::RETURN:
        return "return " + format_operand(block.return_operand);
    case IrBlockTerminator::RETURN_VOID:
        return "return";
    case IrBlockTerminator::NONE:
        return "<unterminated>";
    }
    return "?";
}

std::string IRPrinter::print() const
{
    std::ostringstream out;

    if (!_globals.empty())
    {
        out << "Globals\n";
        for (const auto &[name, type, ast_type, initial_value] : _globals)
        {
        out << "  " << name;
            if (!initial_value.value.empty())
                out << " = " << format_operand(initial_value);
            out << "  (" << format_value_type(type) << ")\n";
        }
        out << "\n";
    }

    if (!_files.empty())
    {
        out << "Files\n";
        for (const auto& file : _files)
            out << "  " << file.name << " -> " << file.path << "\n";
        out << "\n";
    }

    for (const auto& block : _blocks)
    {
        out << block->label << ":\n";

        for (const auto& instr : block->instructions)
            out << "  " << format_instruction(instr) << "\n";

        out << "  " << format_terminator(*block) << "\n\n";
    }

    return out.str();
}