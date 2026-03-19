//
// Created by Roumite on 19/03/2026.
//

#pragma once

#include <memory>
#include <string>
#include <vector>

namespace compiler
{
    enum class IrOperandType
    {
        Temporary, // t0, t1 etc...
        Constant,
        Label // A function label, for example
    };

    struct IrOperand {
        IrOperandType type;
        std::string value = "";

        [[nodiscard]] int32_t as_int() const { return std::stoi(value); }
    };

    enum class IrOpCode {
        ADD, SUB, MUL, DIV, MOD,
        NEG, NOT,
        EQ, NEQ, LT, GT, LEQ, GEQ,
        AND, OR,
        COPY,
        LOAD_ARR,
        STORE_ARR,
        ADDR_OF,
        DEREF,
        DEREF_STORE,
        CALL,
    };

    struct IrInstruction {
        IrOpCode op;
        IrOperand result;
        IrOperand arg1;
        IrOperand arg2;

        std::vector<IrOperand> call_args; // For function call

        static IrInstruction make_irp(const IrOpCode op, const std::string& res, const std::string& a1, const std::string& a2) {
            return {
                op,
                {IrOperandType::Temporary, res},
                {IrOperandType::Temporary, a1},
                {IrOperandType::Temporary, a2}
            };
        }
    };

    struct IrGlobal {
        std::string name;
        IrOperand initial_value;
    };

    enum class IrBlockTerminator {
        NONE,
        JUMP,
        BRANCH,
        RETURN,
        RETURN_VOID,
    };

    struct IrBasicBlock {
        std::string label;
        std::vector<IrInstruction> instructions; // données owned ici

        IrBlockTerminator terminator = IrBlockTerminator::NONE;
        IrOperand condition_operand = {};
        IrOperand return_operand = {};

        std::weak_ptr<IrBasicBlock> successor;
        std::weak_ptr<IrBasicBlock> false_successor; // For Else statement
    };

}