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
        Temporary,
        Constant,
        Label,
    };

    enum class IrValueType
    {
        UNKNOWN,
        BOOL,
        UINT8, UINT16, UINT32,
        INT8, INT16, INT32,
        PTR,
        FILE,
        CLOCK
    };

    struct IrOperand {
        IrOperandType type;
        std::string value;
        IrValueType value_type = IrValueType::UNKNOWN;

        [[nodiscard]] int32_t as_int() const { return std::stoi(value); }
        [[nodiscard]] bool is_signed() const {
            return value_type == IrValueType::INT8
                || value_type == IrValueType::INT16
                || value_type == IrValueType::INT32;
        }

        [[nodiscard]] bool empty() const { return value.empty(); }
    };

    enum class IrOpCode {
        ADD, SUB, MUL, DIV, MOD,
        NEG, NOT,

        EQ, NEQ,
        LT, GT, LEQ, GEQ,
        SLT, SGT, SLEQ, SGEQ, // Signed version

        SEXT, // Sign extention
        ZEXT, // Sign packing

        AND, OR,

        COPY, // result = args
        ADDR_OF, // result = #arg1

        LOAD_ARR, // result = arg1[arg2]
        STORE_ARR, // result[arg1] = arg2

        LOAD_8,  LOAD_16,  LOAD_32,
        STORE_8, STORE_16, STORE_32,

        // High levels operator
        DEREF, // result = *arg1
        DEREF_STORE, // *result = arg1 (result is a pointer)

        CALL,
        BUILTIN_CALL,
    };

    struct IrInstruction {
        IrOpCode op;
        IrOperand result;
        IrOperand arg1;
        IrOperand arg2;

        std::vector<IrOperand> call_args; // For function call

        static IrInstruction make_3addr(
            const IrOpCode op,
            const std::string &res,
            const std::string &a1,
            const std::string &a2)
        {
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
        IrValueType type = IrValueType::UNKNOWN;
        IrOperand initial_value = {};
    };

    enum class IrBlockTerminator {
        NONE,
        JUMP,
        BRANCH,
        RETURN,
        RETURN_VOID,
    };

    struct IrBasicBlock
    {
        std::string label;
        std::vector<IrInstruction> instructions;

        bool is_function_entry = false;
        std::string function_name;
        std::vector<std::string> parameters;

        IrBlockTerminator terminator = IrBlockTerminator::NONE;
        IrOperand condition_operand = {};
        IrOperand return_operand = {};

        std::weak_ptr<IrBasicBlock> successor;
        std::weak_ptr<IrBasicBlock> false_successor;
    };
}