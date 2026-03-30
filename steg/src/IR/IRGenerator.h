//
// Created by Roumite on 19/03/2026.
//

#pragma once

#include "ir_structure.h"
#include "ast/ASTVisitor.h"

namespace compiler
{
    static uint64_t _label_count = 0;

    class IRGenerator final : public ASTVisitor
    {
    public:
        std::vector<std::shared_ptr<IrBasicBlock>> all_blocks;
        std::vector<IrGlobal> globals;

    private:
        std::shared_ptr<IrBasicBlock> _current_block{};
        uint64_t _temp_count = 0;
        IrOperand _current_operand = {};

        std::vector<std::weak_ptr<IrBasicBlock>> loop_start_blocks_{}; // Need for the 'continue' statement
        std::vector<std::weak_ptr<IrBasicBlock>> loop_end_blocks_{}; // Need for the 'break' statement

        std::string new_temp() { return "t" + std::to_string(_temp_count++); }

        std::shared_ptr<IrBasicBlock> new_block(const std::string& label = "");

        [[nodiscard]] bool is_terminated() const { return _current_block->terminator != IrBlockTerminator::NONE; }
        void add_instruction(IrInstruction i) const { _current_block->instructions.push_back(std::move(i)); }

        static std::string gen_function_label(std::filesystem::path path, const std::string& name);

        /* Different block termination */

        void terminate_jump(const std::shared_ptr<IrBasicBlock> &target) const; // Simple jump
        void terminate_branch( // Jump in true_block of false_block depending on cond
            IrOperand cond,
            const std::shared_ptr<IrBasicBlock> &true_block,
            const std::shared_ptr<IrBasicBlock> &false_block) const;
        void terminate_return(IrOperand val = {}) const; // Return


        // Opérands

        static IrOperand temp_op(const std::string& v) { return {IrOperandType::Temporary, v}; }
        static IrOperand const_op(const std::string& v) { return {IrOperandType::Constant, v}; }
        static IrOperand label_op(const std::string& v) { return {IrOperandType::Label, v}; }

        IrOperand eval(ASTExpressionNode* expr)
        {
            expr->accept(this);
            return _current_operand;
        }

        /* opcode converters*/
        static IrOpCode binary_opcode(const ASTBinaryExpressionNode::binaryOperationType op);
        static IrOpCode binary_opcode(const ASTBinaryExpressionNode::binaryOperationType op, bool is_signed);
        static IrOpCode unary_opcode(const ASTUnaryExpressionNode::unaryOperationType op);
        static IrOpCode composed_opcode(const ASTAssignExpressionStatement::assignmentType op);
        static IrValueType type_to_ptr_type(const ResolvedType &t);
        static IrValueType resolved_to_ir_type(const ResolvedType &t);
    public:
        /* --Visitors-- */

        /* Expressions */

        void visit(ASTLiteralExpressionNode *node) override; // t0 = ConstantValue
        void visit(ASTIdentifierExpressionNode *node) override;
        void visit(ASTBinaryExpressionNode *node) override; // t0 = 5 + 3 (or t1 = t0 + 5, for example)
        void visit(ASTUnaryExpressionNode *node) override;
        void visit(ASTIndexExpressionNode *node) override; // t0 = array[50] (or t1 = array[t0], for example)
        void visit(ASTCallExpressionNode* node) override;
        void visit(ASTAssignExpressionStatement* node) override;
        void visit(ASTAddressOfExpressionNode *node) override; // t0 = @variable
        void visit(ASTDereferenceExpressionNode *node) override; // T0 = #variable


        /* Statements */

        void visit(ASTExpressionStatement *node) override
        {
            eval(node->expression.get());
        }

        void visit(ASTVariableStatement *node) override;
        void visit(ASTBlockStatementNode *node) override;
        void visit(ASTIfStatementNode *node) override;
        void visit(ASTWhileStatementNode *node) override;
        void visit(ASTForStatementNode *node) override;
        void visit(ASTReturnStatement *node) override;
        void visit(ASTBreakStatement*) override;
        void visit(ASTContinueStatement*) override;

        /* Programs nodes */

        void visit(ASTFunctionProgramNode* node) override;

        void visit(ASTMainProgramNode* node) override
        {
            for (const auto& var : node->global_variables)
                var->accept(this);
            for (const auto& fn : node->functions)
                fn->accept(this);
        }

        void visit(ASTTypeNode *) override {}
        void visit(ASTErrorNode *) override {}
        void visit(ASTStatementError *) override {}
        void visit(ASTParameterProgramNode *) override {}
        void visit(ASTImportProgramNode *) override {}
        void visit(ASTFilesProgramNode *) override {}
        void visit(ASTFileProgramNode *) override {}
    };
}
