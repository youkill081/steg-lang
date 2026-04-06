//
// Created by Roumite on 19/03/2026.
//

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "ast/ASTTypeNode.h"

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
        INT,
        FLOAT,
        PTR8, PTR16, PTR32,
        FILE,
        CLOCK
    };

    inline bool ir_value_type_is_ptr(IrValueType t)
    {
        return t == IrValueType::PTR8 || t == IrValueType::PTR16 || t == IrValueType::PTR32;
    }

    struct IrOperand {
        IrOperandType type;
        std::string value;
        IrValueType value_type = IrValueType::UNKNOWN;

        [[nodiscard]] int32_t as_int() const { return std::stoi(value); }
        [[nodiscard]] bool is_signed() const {
            return value_type == IrValueType::INT;
        }

        [[nodiscard]] bool empty() const { return value.empty(); }
    };

    enum class IrOpCode {
        ADD, SUB, MUL, DIV, MOD,
        NEG, NOT,

        FADD, FSUB, FMUL, FDIV, FMOD, // Float versions
        FNEG,

        SDIV, SMUL,

        EQ, NEQ,
        LT, GT, LEQ, GEQ,
        SLT, SGT, SLEQ, SGEQ, // Signed version

        FEQ, FNEQ, // Float versions
        FLT, FGT, FLEQ, FGEQ,

        ITOF, UTOF, // int->float ; unsigned->float
        FTOI, FTOU, // float->int ; float->unsigned

        ZEXTEND, // Sign extention
        TRUNC, // Sign packing

        AND, OR,

        COPY, // result = args
        ADDR_OF, // result = #arg1

        LOAD_ARR, // result = arg1[arg2]
        STORE_ARR, // result[arg1] = arg2

        LOAD_8,  LOAD_16,  LOAD_32,
        STORE_8, STORE_16, STORE_32,

        SPILL_SAVE, SPILL_RESTORE,

        // High levels operator
        DEREF, // result = *arg1
        DEREF_STORE, // *result = arg1 (result is a pointer)

        CALL,
        BUILTIN_CALL,
    };

    inline uint64_t current_instr_nbr = 0;
    struct IrInstruction {
        IrOpCode op;
        IrOperand result;
        IrOperand arg1;
        IrOperand arg2;
        uint64_t instr_nbr = current_instr_nbr++;

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

    struct IrFile {
        std::string name;
        std::string path;
        std::string absolute_path;
    };

    struct IrGlobal {
        std::string name;
        IrValueType type = IrValueType::UNKNOWN;
        ASTTypeNode::Types default_ast_type = ASTTypeNode::Types::VOID;
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
