cmake_minimum_required(VERSION 4.0)

project(steg)

set(CMAKE_CXX_STANDARD 23)

set(STEG_LIB_SOURCES
        src/lexer/lexer_definitions.h
        src/lexer/Lexer.cpp
        src/lexer/Lexer.h
        src/lexer/TokenMap.cpp
        src/lexer/TokenMap.h
        src/lexer/TextParser.cpp
        src/lexer/TextParser.h
        src/exceptions.h
        src/exceptions.h
        src/ast/ASTNode.h
        src/ast/ASTStatementNode.cpp
        src/ast/ASTStatementNode.h
        src/ast/ASTExpressionNode.cpp
        src/ast/ASTExpressionNode.h
        src/ast/ASTProgramNode.cpp
        src/ast/ASTProgramNode.h
        src/parser/monadic/monadic.hpp
        src/parser/monadic/string_parser.h
        src/parser/parser_utils.h
        src/parser/parser_program.h
        src/parser/expressions/parse_expressions.h
        src/parser/statements/parse_block.h
        src/parser/statements/parse_statements.h
        src/parser/statements/parse_if.h
        src/parser/statements/parse_variables.h
        src/parser/statements/parse_while.h
        src/parser/statements/parse_for.h
        src/parser/statements/parse_return.h
        src/parser/statements/parse_break_continue.h
        src/parser/statements/parse_expression_statement.h
        src/parser/expressions/parse_types.h
        src/parser/expressions/parse_binary.h
        src/parser/expressions/parse_expressions_utils.h
        src/parser/expressions/parse_primary.h
        src/parser/expressions/parser_assignments.h
        src/parser/expressions/parse_unary.h
        src/linter/Linter.h
        src/linter/Linter.cpp
        src/ast/ASTVisitor.h
        src/ast/ASTTypeNode.h
        src/semantic_analysis/utils/SymbolTable.h
        src/semantic_analysis/step1/SymbolCollector.h
        src/semantic_analysis/step4/SemanticTokensVisitor.h
        src/semantic_analysis/exports/ExportCollector.h
        src/semantic_analysis/utils/helpers.h
        src/semantic_analysis/utils/ModuleManager.h
        src/semantic_analysis/analyzeExports.h
        src/semantic_analysis/utils/ResolvedType.h
        src/semantic_analysis/step2/TypeInferenceVisitor.h
        src/compiler.h
        src/semantic_analysis/step2/TypeInferenceVisitor.cpp
        src/semantic_analysis/step3/ControlFlowVisitor.cpp
        src/semantic_analysis/step3/ControlFlowVisitor.h
        src/IR/ir_structure.h
        src/IR/IRGenerator.cpp
        src/IR/IRGenerator.h
        src/IR/IRprinter.cpp
        src/IR/IRprinter.h
        src/IR/Irlowering.cpp
        src/IR/Irlowering.h
        src/asm_gen/Registerallocator.cpp
        src/asm_gen/Registerallocator.h
        src/asm_gen/AsmGenerator.h
        src/asm_gen/AsmGenerator.cpp
)

add_library(steg_lib STATIC ${STEG_LIB_SOURCES})

target_include_directories(steg_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(steg_lib PUBLIC stegcommon)

add_executable(steg src/main.cpp)

target_link_libraries(steg PRIVATE steg_lib stegasm_lib)