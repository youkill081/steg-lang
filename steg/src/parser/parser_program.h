//
// Created by Roumite on 15/03/2026.
//

#pragma once

#include "parser_utils.h"
#include <memory>
#include "ast/ASTProgramNode.h"
#include "expressions/parse_expressions.h"
#include "statements/parse_block.h"

namespace compiler
{
    /* Function parameters */
    inline Parser<std::unique_ptr<ASTParameterProgramNode>, TokenSpan>
    parseParameter =
         map(
            seq(
                lint<"Invalid type %v">(parseTypeNoVoid),
                lintedParseToken<TOKEN_IDENTIFIER>
            ),
            [](auto data)
            {
                return std::make_unique<ASTParameterProgramNode>(
                    std::get<1>(data).value,
                    std::move(std::get<0>(data))
                );
            }
        );

    inline Parser<std::unique_ptr<ASTParameterProgramNode>, TokenSpan>
    parseParamWithComma = parseToken<TOKEN_PUNCTUATION_COMMA> >> parseParameter;

    inline Parser<std::vector<std::unique_ptr<ASTParameterProgramNode>>, TokenSpan>
parseFunctionParameters =
    lintedParseToken<TOKEN_PUNCTUATION_LEFT_PARENTHESIS> >>
    (
        map( // No parameters
            parseToken<TOKEN_PUNCTUATION_RIGHT_PARENTHESIS>,
            [](auto) { return std::vector<std::unique_ptr<ASTParameterProgramNode>>{}; }
        )
        |
        map( // With parameters
            seq(
                parseParameter,
                many(parseParamWithComma),
                lintedParseToken<TOKEN_PUNCTUATION_RIGHT_PARENTHESIS>
            ),
            [](auto data) {
                auto first_param = std::move(std::get<0>(data));
                auto params_vec  = std::move(std::get<1>(data));

                std::vector<std::unique_ptr<ASTParameterProgramNode>> parameters;
                parameters.push_back(std::move(first_param));
                for (auto &p : params_vec)
                    parameters.push_back(std::move(p));

                return parameters;
            }
        )
    );

    /* Functions */

    inline Parser<LexerToken, TokenSpan> parseFunctionDeclaration = parseToken<TOKEN_KEYWORD_FUNCTION> >> lint<"missing function identifier">(parseToken<TOKEN_IDENTIFIER>);
    inline Parser<std::unique_ptr<ASTTypeNode>, TokenSpan> parseFunctionReturnType = lint<"Missing function return type">(parseToken<TOKEN_PUNCTUATION_ARROW>) >> lint<"Invalid or missing function type (got %v)">(parseType);

    inline Parser<std::unique_ptr<ASTFunctionProgramNode>, TokenSpan> parseFunction =
        map(seq(
                optional(parseToken<TOKEN_KEYWORD_EXPORT>),
                parseFunctionDeclaration,
                parseFunctionParameters,
                parseFunctionReturnType,
                parseBlock
            ), [](auto data)
            {
                return std::make_unique<ASTFunctionProgramNode>(
                    std::move(std::get<1>(data).value),
                    std::move(std::get<2>(data)),
                    std::move(std::get<3>(data)),
                    std::move(std::get<4>(data)),
                    std::get<0>(data).has_value()
                );
            });


    /* Files */

    inline Parser<std::unique_ptr<ASTFileProgramNode>, TokenSpan> parseAFile =
            map (
                seq(parseToken<TOKEN_IDENTIFIER> << lintedParseToken<TOKEN_PUNCTUATION_COLON>, lintedParseToken<TOKEN_STRING>),
                [](auto data)
                {
                    auto [name, path] = std::move(data);
                    return std::make_unique<ASTFileProgramNode>(name.value, path.value);
                });


    inline Parser<std::unique_ptr<ASTFilesProgramNode>, TokenSpan> parseFiles =
        map (parseToken<TOKEN_KEYWORD_FILES> >> lintedParseToken<TOKEN_PUNCTUATION_LEFT_BRACKET> >> many(parseAFile) << lintedParseToken<TOKEN_PUNCTUATION_RIGHT_BRACKET>,
            [](auto data)
            {
                return std::make_unique<ASTFilesProgramNode>(
                    std::move(data)
                );
            });

    /* Import */

    inline Parser<std::unique_ptr<ASTImportProgramNode>, TokenSpan> parseImport =
        map(seq(
                parseToken<TOKEN_KEYWORD_IMPORT> >> lintedParseToken<TOKEN_PUNCTUATION_LEFT_BRACKET> >> many(
                    optional(parseToken<TOKEN_PUNCTUATION_COMMA>) >> parseToken<TOKEN_IDENTIFIER>),
                lintedParseToken<TOKEN_PUNCTUATION_RIGHT_BRACKET> >> lintedParseToken<TOKEN_KEYWORD_FROM> >>
                lintedParseToken<TOKEN_STRING>
            ), [](auto data)
            {
                auto [function_variables, aze] = std::move(data);

                std::vector<std::string> function_variables_vec;
                for (const auto& token : function_variables)
                    function_variables_vec.push_back(token.value);

                return std::make_unique<ASTImportProgramNode>(
                    function_variables_vec,
                    aze.value
                );
            });

    /* Main Program Node*/

    // Stop sets
    using AllTypes = StopSet<
        TOKEN_TYPE_UINT8, TOKEN_TYPE_INT8, TOKEN_TYPE_UINT16,
        TOKEN_TYPE_INT16, TOKEN_TYPE_UINT32, TOKEN_TYPE_INT32, TOKEN_TYPE_BOOL>;

    using function_stop = ConcatStopSet<StopSet<TOKEN_KEYWORD_FILES, TOKEN_KEYWORD_IMPORT>, AllTypes>::type;
    using import_stop = ConcatStopSet<StopSet<TOKEN_KEYWORD_FUNCTION, TOKEN_KEYWORD_EXPORT, TOKEN_KEYWORD_FILES>, AllTypes>::type;
    using files_stop = ConcatStopSet<StopSet<TOKEN_KEYWORD_FUNCTION, TOKEN_KEYWORD_EXPORT, TOKEN_KEYWORD_IMPORT>, AllTypes>::type;
    using globals_stop = StopSet<TOKEN_KEYWORD_FUNCTION, TOKEN_KEYWORD_EXPORT, TOKEN_KEYWORD_IMPORT, TOKEN_KEYWORD_FILES>;

    // Resync sets
    using AllTypesSyncSet = SyncSet<
        TOKEN_TYPE_UINT8, TOKEN_TYPE_INT8, TOKEN_TYPE_UINT16,
        TOKEN_TYPE_INT16, TOKEN_TYPE_UINT32, TOKEN_TYPE_INT32, TOKEN_TYPE_BOOL>;

    using main_sync = ConcatSyncSet<
        SyncSet<TOKEN_KEYWORD_FUNCTION, TOKEN_KEYWORD_EXPORT, TOKEN_KEYWORD_IMPORT, TOKEN_KEYWORD_FILES>,
        AllTypesSyncSet
    >::type;

    inline Parser<std::unique_ptr<ASTMainProgramNode>, TokenSpan> parseMainProgram =
    map(many(
        lint_checkpoint<globals_stop, main_sync, ASTErrorNode>(
            map(parseVariableDeclarationWithSemicolon, [](auto v) -> std::unique_ptr<ASTNode> { return std::move(v); })
        )
        |
        lint_checkpoint<function_stop, main_sync, ASTErrorNode>(
        map(parseFunction, [](auto f) -> std::unique_ptr<ASTNode> { return std::move(f); })
        )
        |
            lint_checkpoint<import_stop, main_sync, ASTErrorNode>(
                map(parseImport, [](auto v) -> std::unique_ptr<ASTNode> { return std::move(v); })
            )
        |
            lint_checkpoint<files_stop, main_sync, ASTErrorNode>(
                map(parseFiles, [](auto v) -> std::unique_ptr<ASTNode> { return std::move(v); })
            )
    ), [](std::vector<std::unique_ptr<ASTNode>> nodes)
    {
        std::vector<std::unique_ptr<ASTFunctionProgramNode>> functions;
        std::vector<std::unique_ptr<ASTVariableStatement>> global_variables;
        std::vector<std::unique_ptr<ASTImportProgramNode>> imports;
        std::vector<std::unique_ptr<ASTFileProgramNode>> files;

        for (auto &node : nodes)
        {
            if (auto *f = dynamic_cast<ASTFunctionProgramNode*>(node.get()))
            {
                node.release();
                functions.push_back(std::unique_ptr<ASTFunctionProgramNode>(f));
            }
            else if (auto* v = dynamic_cast<ASTVariableStatement*>(node.get()))
            {
                node.release();
                global_variables.push_back(std::unique_ptr<ASTVariableStatement>(v));
            }
            else if (auto* l = dynamic_cast<ASTImportProgramNode*>(node.get()))
            {
                node.release();
                imports.push_back(std::unique_ptr<ASTImportProgramNode>(l));
            } else if (auto* f = dynamic_cast<ASTFilesProgramNode*>(node.get()))
            {
                node.release();
                for (auto& file : f->files)
                    files.push_back(std::move(file));
                delete f;
            }
        }

        return std::make_unique<ASTMainProgramNode>(
            std::move(functions),
            std::move(global_variables),
            std::move(imports),
            std::move(files)
        );
    });
}
