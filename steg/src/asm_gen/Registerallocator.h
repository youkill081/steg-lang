//
// Created by Roumite on 20/03/2026.
//

#pragma once

#include "IR/ir_structure.h"

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace compiler
{
    static constexpr int k_reg_return      = 0;
    static constexpr int k_reg_param_first = 1;
    static constexpr int k_reg_param_last  = 6;
    static constexpr int k_reg_free_first  = 10;
    static constexpr int k_reg_free_last   = 31;
    static constexpr int k_reg_free_count  = k_reg_free_last - k_reg_free_first + 1; // 25

    inline std::string reg_name(int index) { return "R" + std::to_string(index); }

    struct LifeDuration { uint64_t start; uint64_t end; };

    struct RegisterAllocation
    {
        std::unordered_map<std::string, std::string> reg_map;

        [[nodiscard]] std::string lookup(
            const std::string& function_label,
            const std::string& temp_name) const;
    };

    class RegisterAllocator
    {
    public:
        explicit RegisterAllocator(
            const std::vector<std::shared_ptr<IrBasicBlock>>& blocks,
            const std::unordered_set<std::string>& global_names);

        [[nodiscard]] RegisterAllocation allocate();

    private:
        const std::vector<std::shared_ptr<IrBasicBlock>>& _blocks;
        const std::unordered_set<std::string>& _global_names;
        std::unordered_set<std::string> _parameter_names;

        std::string _current_function;
        std::unordered_map<std::string, LifeDuration> _registry_life_duration;
        std::vector<int> _free_regs;
        RegisterAllocation _result;

        void compute_instruction_duration(const IrInstruction& instr);
        void compute_function_duration(const IrBasicBlock& entry, uint64_t start_offset);
        void extend_lifetimes_for_back_jumps(uint64_t start_offset);

        void assign_parameters(const IrBasicBlock& entry);
        void linear_scan_function();

        void insert_function_spills(uint64_t start_index);
        void insert_spills_for_block(IrBasicBlock& block);

        void reset_free_pool();
        [[nodiscard]] std::string make_key(const std::string& temp_name) const;
        [[nodiscard]] bool needs_allocation(const IrOperand& op) const;
        [[nodiscard]] bool is_live_across_call(const std::string& name, uint64_t call_instr) const;
    };
}