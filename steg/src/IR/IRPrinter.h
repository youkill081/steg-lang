//
// Created by Roumite on 19/03/2026.
//

#pragma once

#include "ir_structure.h"
#include "IRGenerator.h"

#include <string>
#include <vector>
#include <memory>

namespace compiler
{
    class IRPrinter
    {
        const std::vector<std::shared_ptr<IrBasicBlock>>& _blocks;
        const std::vector<IrGlobal>& _globals;

    public:
        explicit IRPrinter(
            const std::vector<std::shared_ptr<IrBasicBlock>>& blocks,
            const std::vector<IrGlobal>& globals
        ) : _blocks(blocks), _globals(globals)
        {
        }

        explicit IRPrinter(const IRGenerator& gen)
            : _blocks(gen.all_blocks), _globals(gen.globals)
        {
        }

        [[nodiscard]] std::string print() const;

    private:
        static std::string format_operand(const IrOperand& op);
        static std::string format_value_type(IrValueType t);
        static std::string format_instruction(const IrInstruction& i);
        static std::string format_terminator(const IrBasicBlock& block);
    };
}
