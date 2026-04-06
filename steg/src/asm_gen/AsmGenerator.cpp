//
// Created by Roumite on 21/03/2026.
//

#include "AsmGenerator.h"

using namespace compiler;

AsmGenerator::AsmGenerator(
    const std::vector<std::shared_ptr<IrBasicBlock>>& blocks,
    const std::vector<IrGlobal>& globals,
    const std::vector<IrFile>& files,
    const RegisterAllocation& alloc)
    : _blocks(blocks), _globals(globals), _files(files), _alloc(alloc)
{
    for (const auto &g : _globals)
        _global_types[g.name] = g.type;
    for (const auto &[name, path] : _files)
        _file_names.insert(name);
}

std::string AsmGenerator::generate()
{
    emit_files_section();
    emit_data_section();
    emit_text_section();

    std::ostringstream out;
    out << _data.str() << '\n' << _code.str();
    return out.str();
}

// .data section

std::string AsmGenerator::data_directive(IrValueType t)
{
    switch (t)
    {
    case IrValueType::BOOL:
    case IrValueType::UINT8:
    case IrValueType::PTR8: return "DB";
    case IrValueType::UINT16:
    case IrValueType::PTR16: return "DW";
    default: return "DD";
    }
}

std::string AsmGenerator::format_init(const IrGlobal& g)
{
    if (g.initial_value.empty()) return "0";

    const std::string& v = g.initial_value.value;
    if (!v.empty() && v.front() == '"')
        return v + ", 0";
    return v;
}

void AsmGenerator::emit_files_section()
{
    d("section .files");
    for (const auto& f : _files)
        d("    " + f.name + " \"" + f.path + '"');
}

void AsmGenerator::emit_data_section()
{
    d("section .data");
    for (const auto& g : _globals)
    {
        if (g.default_ast_type == ASTTypeNode::STRING)
            d("    " + g.name + " " + data_directive(g.type) + " \"" + format_init(g) + "\"");
        else
            d("    " + g.name + " " + data_directive(g.type) + " " + format_init(g));
    }
}



// .text section

void AsmGenerator::emit_text_section()
{
    c("section .text");

    // Find main function name
    for (const auto& blk : _blocks)
    {
        if (!blk->is_function_entry) continue;
        const std::string& fn = blk->function_name;
        if (fn == "main" ||
            (fn.size() >= 6 && fn.rfind("__main") == fn.size() - 6))
        {
            _main_function_name = fn;
            break;
        }
    }

    auto emit_blocks_for = [&](const std::string& fn_filter, bool include)
    {
        std::string cur_fn;
        for (const auto& blk : _blocks)
        {
            if (blk->is_function_entry)
                cur_fn = blk->function_name;

            if ((cur_fn == fn_filter) == include)
                emit_block(*blk);
        }
    };

    // Write main and other after
    if (!_main_function_name.empty())
        emit_blocks_for(_main_function_name, true);
    emit_blocks_for(_main_function_name, false);
}

void AsmGenerator::emit_block(const IrBasicBlock& block)
{
    if (block.is_function_entry)
        _current_function = block.function_name;

    c(block.label + ":");
    for (const auto& instr : block.instructions)
        emit_instruction(instr);
    emit_terminator(block);
}

// operand

bool AsmGenerator::is_mem(const IrOperand& op) const
{
    return op.type == IrOperandType::Temporary && _global_types.count(op.value) > 0;
}

std::string AsmGenerator::phys_reg(const IrOperand& op) const
{
    if (op.type != IrOperandType::Temporary) return "";
    return _alloc.lookup(_current_function, op.value);
}

IrValueType AsmGenerator::type_of(const std::string& name) const
{
    const auto it = _global_types.find(name);
    return it != _global_types.end() ? it->second : IrValueType::UINT32;
}

uint8_t AsmGenerator::bits_for(IrValueType t)
{
    switch (t)
    {
    case IrValueType::BOOL:
    case IrValueType::UINT8: return 8;
    case IrValueType::UINT16: return 16;
    case IrValueType::PTR8:
    case IrValueType::PTR16:
    case IrValueType::PTR32:
    case IrValueType::UINT32:
    case IrValueType::INT:
    default: return 32;
    }
}

std::string AsmGenerator::emit_mem_load(const std::string& name, IrValueType type, const char* scratch)
{
    if (ir_value_type_is_ptr(type))
        c("    LOAD_" + std::to_string(bits_for(type_of(name))) + " " + scratch + ", " + name );
    else
        c("    LOAD_" + std::to_string(bits_for(type_of(name))) + " " + scratch + ", [" + name + "]");
    return scratch;
}

void AsmGenerator::emit_mem_store(const std::string& reg, const std::string& name)
{
    c("    STORE_" + std::to_string(bits_for(type_of(name))) + " " + reg + ", " + name);
}

std::string AsmGenerator::resolve_src(
    const IrOperand& op, const char* scratch, bool& out_is_imm)
{
    out_is_imm = false;

    switch (op.type)
    {
    case IrOperandType::Constant:
    case IrOperandType::Label:
        out_is_imm = true;
        return op.value;

    case IrOperandType::Temporary:
        if (is_mem(op))
            return emit_mem_load(op.value, op.value_type, scratch);

        if (_file_names.contains(op.value)) {
            c("    LOAD_32 " + std::string(scratch) + ", " + op.value);
            return scratch;
        }

        {
            const std::string r = phys_reg(op);
            return r.empty() ? "0" : r;
        }
    }
    return "0";
}

std::string AsmGenerator::resolve_dst(const IrOperand& op)
{
    if (op.empty() || is_mem(op))
        return k_sdst;

    const std::string r = phys_reg(op);
    return r.empty() ? k_sdst : r;
}

void AsmGenerator::finalize_dst(const IrOperand& op, const std::string& dst_reg)
{
    if (!op.empty() && is_mem(op))
        emit_mem_store(dst_reg, op.value);
}

void AsmGenerator::emit_binop(const char* mnemonic, const IrInstruction& instr)
{
    bool imm1, imm2;
    const std::string s1 = resolve_src(instr.arg1, k_ssrc1, imm1);
    const std::string s2 = resolve_src(instr.arg2, k_ssrc2, imm2);
    const std::string dst = resolve_dst(instr.result);

    const std::string rs1 = [&]() -> std::string
    {
        if (!imm1) return s1;
        c("    LOAD_32 " + std::string(k_ssrc1) + ", " + s1);
        return k_ssrc1;
    }();

    c("    " + std::string(mnemonic) + " " + dst + ", " + rs1 + ", " + s2);
    finalize_dst(instr.result, dst);
}

void AsmGenerator::emit_unop(const char* mnemonic, const IrInstruction& instr)
{
    bool imm;
    const std::string src = resolve_src(instr.arg1, k_ssrc1, imm);
    const std::string dst = resolve_dst(instr.result);
    const uint8_t bits = bits_for(instr.result.value_type);

    const std::string rs = [&]() -> std::string
    {
        if (!imm) return src;
        c("    LOAD_" + std::to_string(bits) + " " + std::string(k_ssrc1) + ", " + src);
        return k_ssrc1;
    }();

    c("    " + std::string(mnemonic) + " " + dst + ", " + rs);
    finalize_dst(instr.result, dst);
}

void AsmGenerator::emit_cmp_bool(const IrInstruction& instr, const char* jmp_true, bool is_float)
{
    bool imm1, imm2;
    const std::string a = resolve_src(instr.arg1, k_ssrc1, imm1);
    const std::string b = resolve_src(instr.arg2, k_ssrc2, imm2);
    const std::string dst = resolve_dst(instr.result);

    const std::string ra = [&]() -> std::string
    {
        if (!imm1) return a;
        c("    LOAD_32 " + std::string(k_ssrc1) + ", " + a);
        return k_ssrc1;
    }();

    c("    " + std::string(is_float ? "FCMP" : "CMP") + " " + ra + ", " + b);

    const std::string lbl_true = fresh_label();
    const std::string lbl_end = fresh_label();
    c("    " + std::string(jmp_true) + " " + lbl_true);
    c("    LOAD_32 " + dst + ", 0");
    c("    JMP " + lbl_end);
    c(lbl_true + ":");
    c("    LOAD_32 " + dst + ", 1");
    c(lbl_end + ":");

    finalize_dst(instr.result, dst);
}

void AsmGenerator::emit_cmp_bool_inv(const IrInstruction& instr, const char* jmp_false, bool is_float)
{
    bool imm1, imm2;
    const std::string a = resolve_src(instr.arg1, k_ssrc1, imm1);
    const std::string b = resolve_src(instr.arg2, k_ssrc2, imm2);
    const std::string dst = resolve_dst(instr.result);

    const std::string ra = [&]() -> std::string
    {
        if (!imm1) return a;
        c("    LOAD_32 " + std::string(k_ssrc1) + ", " + a);
        return k_ssrc1;
    }();

    c("    " + std::string(is_float ? "FCMP" : "CMP") + " " + ra + ", " + b);

    const std::string lbl_false = fresh_label();
    const std::string lbl_end = fresh_label();
    c("    " + std::string(jmp_false) + " " + lbl_false);
    c("    LOAD_32 " + dst + ", 1");
    c("    JMP " + lbl_end);
    c(lbl_false + ":");
    c("    LOAD_32 " + dst + ", 0");
    c(lbl_end + ":");

    finalize_dst(instr.result, dst);
}

void AsmGenerator::emit_load_ir(const IrInstruction& instr, uint8_t bits)
{
    const std::string dst = resolve_dst(instr.result);
    const std::string prefix = "LOAD_" + std::to_string(bits) + " ";
    const IrOperand& a = instr.arg1;

    if (is_mem(a))
    {
        if (ir_value_type_is_ptr(type_of(a.value)))
            c("    " + prefix + dst + ", " + a.value);
        else
            c("    " + prefix + dst + ", [" + a.value + "]");
    }
    else
    {
        bool imm;
        const std::string addr = resolve_src(a, k_ssrc1, imm);
        c("    " + prefix + dst + ", [" + addr + "]");
    }

    finalize_dst(instr.result, dst);
}
void AsmGenerator::emit_store_ir(const IrInstruction& instr, uint8_t bits)
{
    bool imm_v;
    const std::string val = resolve_src(instr.arg2, k_ssrc2, imm_v);
    const std::string vr = [&]() -> std::string
    {
        if (!imm_v) return val;
        c("    LOAD_32 " + std::string(k_ssrc2) + ", " + val);
        return k_ssrc2;
    }();

    const std::string prefix = "STORE_" + std::to_string(bits) + " ";
    const IrOperand& a = instr.arg1;

    if (is_mem(a))
    {
        c("    " + prefix + vr + ", " + a.value);
    }
    else
    {
        bool imm_a;
        const std::string addr = resolve_src(a, k_ssrc1, imm_a);
        c("    " + prefix + vr + ", " + addr);
    }
}

void AsmGenerator::emit_instruction(const IrInstruction& instr)
{
    switch (instr.op)
    {
    case IrOpCode::ADD: emit_binop("ADD", instr);
        break;
    case IrOpCode::SUB: emit_binop("SUB", instr);
        break;
    case IrOpCode::MUL: emit_binop("MUL", instr);
        break;
    case IrOpCode::DIV: emit_binop("DIV", instr);
        break;
    case IrOpCode::MOD: emit_binop("MOD", instr);
        break;
    case IrOpCode::FADD: emit_binop("FADD", instr);
        break;
    case IrOpCode::FSUB: emit_binop("FSUB", instr);
        break;
    case IrOpCode::FMUL: emit_binop("FMUL", instr);
        break;
    case IrOpCode::FDIV: emit_binop("FDIV", instr);
        break;
    case IrOpCode::FMOD: emit_binop("FMOD", instr);
        break;
    case IrOpCode::NEG:
        {
            bool imm;
            const std::string src = resolve_src(instr.arg1, k_ssrc1, imm);
            const std::string dst = resolve_dst(instr.result);
            const uint8_t bits = bits_for(instr.result.value_type);

            const std::string rs = [&]() -> std::string
            {
                if (!imm) return src;
                c("    LOAD_" + std::to_string(bits) + " " + std::string(k_ssrc1) + ", " + src);
                return k_ssrc1;
            }();

            c("    LOAD_" + std::to_string(bits) + " " + std::string(k_ssrc2) + ", 0");
            c("    SUB " + dst + ", " + std::string(k_ssrc2) + ", " + rs);
            finalize_dst(instr.result, dst);
            break;
        }
    case IrOpCode::FNEG:
        {
            bool imm;
            const std::string src = resolve_src(instr.arg1, k_ssrc1, imm);
            const std::string dst = resolve_dst(instr.result);

            const std::string rs = [&]() -> std::string
            {
                if (!imm) return src;
                c("    LOAD_32 " + std::string(k_ssrc1) + ", " + src);
                return k_ssrc1;
            }();

            c("    LOAD_32 " + std::string(k_ssrc2) + ", 0.0");
            c("    FSUB " + dst + ", " + std::string(k_ssrc2) + ", " + rs);
            finalize_dst(instr.result, dst);
            break;
        }

    case IrOpCode::NOT: emit_unop("NOT", instr);
        break;

    // Unsigned comparison
    case IrOpCode::EQ: emit_cmp_bool(instr, "JE", false);
        break;
    case IrOpCode::NEQ: emit_cmp_bool(instr, "JNE", false);
        break;
    case IrOpCode::LT: emit_cmp_bool(instr, "JB", false);
        break;
    case IrOpCode::GT: emit_cmp_bool(instr, "JA", false);
        break;
    case IrOpCode::LEQ: emit_cmp_bool_inv(instr, "JA", false);
        break;
    case IrOpCode::GEQ: emit_cmp_bool_inv(instr, "JB", false);
        break;

    // Float comparison
    case IrOpCode::FEQ: emit_cmp_bool(instr, "JE", true);
        break;
    case IrOpCode::FNEQ: emit_cmp_bool(instr, "JNE", true);
        break;
    case IrOpCode::FLT: emit_cmp_bool(instr, "JB", true);
        break;
    case IrOpCode::FGT: emit_cmp_bool(instr, "JA", true);
        break;
    case IrOpCode::FLEQ:emit_cmp_bool_inv(instr, "JA", true);
        break;
    case IrOpCode::FGEQ:emit_cmp_bool_inv(instr, "JB", true);
        break;

    // Signed comparison
    case IrOpCode::SLT: emit_cmp_bool(instr, "JSB", false);
        break;
    case IrOpCode::SGT: emit_cmp_bool(instr, "JSA", false);
        break;
    case IrOpCode::SLEQ: emit_cmp_bool_inv(instr, "JSA", false);
        break;
    case IrOpCode::SGEQ: emit_cmp_bool_inv(instr, "JSB", false);
        break;

    case IrOpCode::AND: emit_binop("MUL", instr);
        break;
    case IrOpCode::OR: emit_binop("MAX", instr);
        break;

    case IrOpCode::ZEXTEND:
        {
            const IrValueType st = instr.arg1.value_type;
            if (st == IrValueType::UINT8)
                emit_unop("EXTEND_8", instr);
            else
                emit_unop("EXTEND_16", instr);
            break;
        }
    case IrOpCode::TRUNC:
        {
            const IrValueType st = instr.arg1.value_type;
            if (st == IrValueType::UINT8 || st == IrValueType::BOOL)
                emit_unop("TRUNC_8", instr);
            else if (st == IrValueType::UINT16)
                emit_unop("TRUNC_16", instr);
            else
                emit_unop("LOAD_32", instr);
            break;
        }

    case IrOpCode::COPY:
        {
            bool imm;
            const std::string src = resolve_src(instr.arg1, k_ssrc1, imm);
            const std::string dst = resolve_dst(instr.result);
            c("    LOAD_32 " + dst + ", " + src);
            finalize_dst(instr.result, dst);
            break;
        }

    case IrOpCode::ADDR_OF:
        {
            const std::string dst = resolve_dst(instr.result);
            c("    LOAD_32 " + dst + ", " + instr.arg1.value);
            finalize_dst(instr.result, dst);
            break;
        }

    case IrOpCode::LOAD_8: emit_load_ir(instr, 8);
        break;
    case IrOpCode::LOAD_16: emit_load_ir(instr, 16);
        break;
    case IrOpCode::LOAD_32: emit_load_ir(instr, 32);
        break;

    case IrOpCode::STORE_8: emit_store_ir(instr, 8);
        break;
    case IrOpCode::STORE_16: emit_store_ir(instr, 16);
        break;
    case IrOpCode::STORE_32: emit_store_ir(instr, 32);
        break;

    case IrOpCode::SPILL_SAVE: c("    PUSH " + resolve_dst(instr.arg1));
        break;
    case IrOpCode::SPILL_RESTORE: c("    POP " + resolve_dst(instr.arg1));
        break;

    case IrOpCode::LOAD_ARR:
        {
            const uint8_t bits = bits_for(instr.result.value_type);
            bool imm_b, imm_i;
            const std::string base = resolve_src(instr.arg1, k_ssrc1, imm_b);
            const std::string idx = resolve_src(instr.arg2, k_ssrc2, imm_i);
            const std::string dst = resolve_dst(instr.result);

            const std::string rb = [&]() -> std::string
            {
                if (!imm_b) return base;
                c("    LOAD_32 " + std::string(k_ssrc1) + ", " + base);
                return k_ssrc1;
            }();

            c("    ADD " + std::string(k_ssrc1) + ", " + rb + ", " + idx);
            c("    LOAD_" + std::to_string(bits) + " " + dst + ", [" + std::string(k_ssrc1) + "]");
            finalize_dst(instr.result, dst);
            break;
        }

    case IrOpCode::STORE_ARR:
        {
            const uint8_t bits = bits_for(instr.arg2.value_type);
            bool imm_b, imm_i, imm_v;
            const std::string base = resolve_src(instr.result, k_sdst, imm_b);
            const std::string idx = resolve_src(instr.arg1, k_ssrc1, imm_i);
            const std::string val = resolve_src(instr.arg2, k_ssrc2, imm_v);

            const std::string rb = [&]() -> std::string
            {
                if (!imm_b) return base;
                c("    LOAD_32 " + std::string(k_sdst) + ", " + base);
                return k_sdst;
            }();

            const std::string rv = [&]() -> std::string
            {
                if (!imm_v) return val;
                c("    LOAD_32 " + std::string(k_ssrc2) + ", " + val);
                return k_ssrc2;
            }();

            c("    ADD " + std::string(k_ssrc1) + ", " + rb + ", " + idx);
            c("    STORE_" + std::to_string(bits) + " " + rv + ", " + std::string(k_ssrc1));
            break;
        }

    case IrOpCode::DEREF:
        {
            const uint8_t bits = bits_for(instr.result.value_type);
            bool imm;
            const std::string ptr = resolve_src(instr.arg1, k_ssrc1, imm);
            const std::string dst = resolve_dst(instr.result);

            const std::string rp = [&]() -> std::string
            {
                if (!imm) return ptr;
                c("    LOAD_32 " + std::string(k_ssrc1) + ", " + ptr);
                return k_ssrc1;
            }();

            c("    LOAD_" + std::to_string(bits) + " " + dst + ", [" + rp + "]");
            finalize_dst(instr.result, dst);
            break;
        }

    case IrOpCode::DEREF_STORE:
        {
            bool imm_v;
            const std::string val = resolve_src(instr.arg1, k_ssrc2, imm_v);
            const uint8_t bits = bits_for(instr.arg1.value_type);
            const std::string rv = [&]() -> std::string
            {
                if (!imm_v) return val;
                c("    LOAD_32 " + std::string(k_ssrc2) + ", " + val);
                return k_ssrc2;
            }();

            const std::string addr = [&]() -> std::string
            {
                if (is_mem(instr.result))
                    return emit_mem_load(instr.result.value, instr.result.value_type, k_ssrc1);
                const std::string r = phys_reg(instr.result);
                return r.empty() ? k_ssrc1 : r;
            }();

            c("    STORE_" + std::to_string(bits) + " " + rv + ", " + addr);
            break;
        }

    case IrOpCode::CALL:
        {
            for (std::size_t i = 0; i < instr.call_args.size() && i < 6; ++i)
            {
                bool imm;
                const std::string arg = resolve_src(instr.call_args[i], k_ssrc1, imm);
                const std::string param = "R" + std::to_string(static_cast<int>(i) + 1);

                if (arg != param)
                {
                    uint8_t bits = bits_for(instr.call_args[i].value_type);
                    c("    LOAD_" + std::to_string(bits) + " " + param + ", " + arg);
                }
            }

            c("    CALL " + instr.arg1.value);

            if (!instr.result.empty())
            {
                const std::string dst = resolve_dst(instr.result);
                if (dst != "R0")
                {
                    uint8_t bits = bits_for(instr.result.value_type);
                    c("    LOAD_" + std::to_string(bits) + " " + dst + ", R0");
                }
                finalize_dst(instr.result, dst);
            }
            break;
        }

    case IrOpCode::BUILTIN_CALL:
        {
            std::vector<std::string> used_params;

            for (std::size_t i = 0; i < instr.call_args.size() && i < 6; ++i)
            {
                bool imm;
                const std::string arg = resolve_src(instr.call_args[i], k_ssrc1, imm);
                const std::string param = "R" + std::to_string(static_cast<int>(i) + 1);

                if (arg != param)
                {
                    uint8_t bits = bits_for(instr.call_args[i].value_type);
                    c("    LOAD_" + std::to_string(bits) + " " + param + ", " + arg);
                }

                used_params.push_back(param);
            }

            std::string line = "    " + instr.arg1.value;
            bool has_return = !instr.result.empty();
            if (has_return)
            {
                line += " R0";
            }

            for (std::size_t i = 0; i < used_params.size(); ++i)
            {
                line += " " + used_params[i];
            }

            c(line);

            if (has_return)
            {
                const std::string dst = resolve_dst(instr.result);
                if (dst != "R0")
                {
                    uint8_t bits = bits_for(instr.result.value_type);
                    c("    LOAD_" + std::to_string(bits) + " " + dst + ", R0");
                }
                finalize_dst(instr.result, dst);
            }
            break;
        }



    case IrOpCode::ITOF: emit_unop("ITOF", instr);
        break;
    case IrOpCode::UTOF: emit_unop("UTOF", instr);
        break;
    case IrOpCode::FTOI: emit_unop("FTOI", instr);
        break;
    case IrOpCode::FTOU: emit_unop("FTOU", instr);
        break;
    }
}

void AsmGenerator::emit_terminator(const IrBasicBlock& block)
{
    switch (block.terminator)
    {
    case IrBlockTerminator::JUMP:
        {
            const auto t = block.successor.lock();
            c("    JMP " + (t ? t->label : "???"));
            break;
        }
    case IrBlockTerminator::BRANCH:
        {
            const auto t = block.successor.lock();
            const auto f = block.false_successor.lock();

            bool imm;
            const std::string cond = resolve_src(block.condition_operand, k_ssrc1, imm);
            const std::string rc = [&]() -> std::string
            {
                if (!imm) return cond;
                c("    LOAD_32 " + std::string(k_ssrc1) + ", " + cond);
                return k_ssrc1;
            }();

            c("    CMP " + rc + ", 0");
            c("    JNE " + (t ? t->label : "???"));
            c("    JMP " + (f ? f->label : "???"));
            break;
        }
    case IrBlockTerminator::RETURN:
        {
            bool imm;
            const std::string val = resolve_src(block.return_operand, k_ssrc1, imm);
            if (_current_function == _main_function_name)
            {
                if (val != "R0")
                    c("    LOAD_32 R0, " + val);
                c("    HALT");
            }
            else
            {
                if (val != "R0")
                    c("    LOAD_32 R0, " + val);
                c("    RET");
            }
            break;

        }
    case IrBlockTerminator::RETURN_VOID:
        if (_current_function == _main_function_name)
            c("    HALT");
        else
            c("    RET");
        break;

    case IrBlockTerminator::NONE:
        c("    ; <bloc non terminé>");
        break;
    }
}

std::string AsmGenerator::fresh_label()
{
    return "__cg_" + std::to_string(_label_idx++);
}

void AsmGenerator::c(std::string line) { _code << std::move(line) << '\n'; }
void AsmGenerator::d(std::string line) { _data << std::move(line) << '\n'; }
