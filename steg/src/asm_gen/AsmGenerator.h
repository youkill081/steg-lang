//
// Created by Roumite on 21/03/2026.
//

#pragma once

#include "IR/ir_structure.h"
#include "asm_gen/Registerallocator.h"

#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <set>

namespace compiler
{
    class AsmGenerator
    {
    public:
        AsmGenerator(
            const std::vector<std::shared_ptr<IrBasicBlock>>& blocks,
            const std::vector<IrGlobal>& globals,
            const std::vector<IrFile>& files,
            const RegisterAllocation& alloc);

        [[nodiscard]] std::string generate();

    private:
        // R0 = Return Value
        // R1–R6 = Parameter or skratch
        // R7–R31 = general usage
        static constexpr auto k_sdst = "R4";
        static constexpr auto k_ssrc1 = "R5";
        static constexpr auto k_ssrc2 = "R6";

        const std::vector<std::shared_ptr<IrBasicBlock>>& _blocks;
        const std::vector<IrGlobal>& _globals;
        const std::vector<IrFile>& _files;
        const RegisterAllocation& _alloc;
        std::string _main_function_name;

        std::unordered_map<std::string, IrValueType> _global_types;
        std::set<std::string> _file_names;

        std::ostringstream _data;
        std::ostringstream _code;
        std::string _current_function;
        int _label_idx = 0;

        // .files section
        void emit_files_section();

        // .data section
        void emit_data_section();
        static std::string data_directive(IrValueType t);
        static std::string format_init(const IrGlobal& g);

        // .text section
        void emit_text_section();
        void emit_block(const IrBasicBlock& block);
        void emit_instruction(const IrInstruction& instr);
        void emit_terminator(const IrBasicBlock& block);

        // operand resolution
        [[nodiscard]] bool is_mem(const IrOperand& op) const;
        [[nodiscard]] std::string phys_reg(const IrOperand& op) const;
        [[nodiscard]] IrValueType type_of(const std::string& name) const;
        [[nodiscard]] static uint8_t bits_for(IrValueType t);

        // Load variable in scratch
        std::string emit_mem_load(const std::string& name, IrValueType type, const char* scratch);
        // Store reg in scratch
        void emit_mem_store(const std::string& reg, const std::string& name);

        std::string resolve_src(const IrOperand& op, const char* scratch, bool& out_is_imm);
        std::string resolve_dst(const IrOperand& op);
        void finalize_dst(const IrOperand& op, const std::string& dst_reg);

        void emit_binop(const char* mnemonic, const IrInstruction& instr);
        void emit_unop(const char* mnemonic, const IrInstruction& instr);
        void emit_cmp_bool(const IrInstruction& instr, const char* jmp_true, bool is_float);
        void emit_cmp_bool_inv(const IrInstruction& instr, const char* jmp_false, bool is_float);
        void emit_load_ir(const IrInstruction& instr, uint8_t bits);
        void emit_store_ir(const IrInstruction& instr, uint8_t bits);

        std::string fresh_label();
        void c(std::string line); // _code
        void d(std::string line); // _data
    };
}
