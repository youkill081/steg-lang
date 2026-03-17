//
// Created by Roumite on 15/03/2026.
//

#pragma once

#include <span>
#include <optional>
#include <functional>
#include <tuple>
#include <vector>
#include <type_traits>
#include <string_view>
#include <string>
#include <iostream>

#include "ast/ASTNode.h"
#include "lexer/lexer_definitions.h"
#include "lexer/Lexer.h"

namespace compiler
{
    using CharSpan = std::span<const char>;

    using TokenSpan = std::span<const LexerToken>;

    /* Used to insert string into template */
    template <size_t N>
        struct FixedString
    {
        char string[N]{};

        constexpr FixedString(const char (&str)[N]) noexcept
        {
            std::copy_n(str, N, string);
        }

        [[nodiscard]] constexpr std::string_view view() const { return { string, N - 1 }; }
        [[nodiscard]] static constexpr size_t size() { return N - 1; }
    };

    template <LexerTokensTypes... Ts>
    struct RecoverySet {};

    template <typename T, typename Stream>
    struct Result
    {
        T value;
        Stream remaining;

        using value_type = T;
        using stream_type = Stream;
    };

    template <typename T, typename Stream>
    using Parser = std::function<std::optional<Result<T, Stream>>(Stream)>;

    /* Utility Methods */

    /*
     * Transform ParserA result with given function
     */
    template <typename ParserA, typename Func>
    auto map(ParserA parser, Func function)
    {
        return [=](auto input) {
            using Stream = decltype(input);
            auto res = parser(input);

            if (!res.has_value()) return std::optional<Result<std::invoke_result_t<Func, decltype(std::move(*res).value)>, Stream>>{};

            using B = std::invoke_result_t<Func, decltype(std::move(*res).value)>;
            return std::optional<Result<B, Stream>>{
                Result<B, Stream>{ function(std::move(*res).value), res->remaining }
            };
        };
    }


    template <typename P, typename S>
    using _parser_val_t = std::invoke_result_t<P, S>::value_type::value_type;

    /*
     * Exec x parsers and insert the result in a tuple
     */
    template <typename... Parsers>
    auto seq(Parsers... parsers)
    {
        return [=](auto input) {
            using Stream = decltype(input);
            using TupleType = std::tuple<_parser_val_t<Parsers, Stream>...>;

            auto parser_tuple = std::make_tuple(parsers...);

            return [&]<size_t... Is>(std::index_sequence<Is...>) -> std::optional<Result<TupleType, Stream>>
            {
                Stream current = input;
                bool failed  = false;

                std::tuple<std::optional<_parser_val_t<Parsers, Stream>>...> optionals;

                (([&] {
                    if (failed) return;
                    auto res = std::get<Is>(parser_tuple)(current);
                    if (!res) { failed = true; return; }
                    std::get<Is>(optionals) = std::move(*res).value;
                    current = res->remaining;
                }()), ...);

                if (failed) return std::nullopt;

                return { Result<TupleType, Stream>{
                    TupleType{ std::move(*std::get<Is>(optionals))... },
                    current
                }};
            } (std::index_sequence_for<Parsers...>{});
        };
    }

    /*
     * Apply 0 to X times the parser. Return the vector of all parsed values
     */
    template <typename Parser>
    auto many(Parser parser)
    {
        return [=](auto input) {
            using Stream = decltype(input);
            using OptRes = decltype(parser(input));
            using T = OptRes::value_type::value_type;

            std::vector<T> values;
            auto current_input = input;

            while (true)
            {
                auto res = parser(current_input);
                if (!res) break;

                values.push_back(std::move(*res).value);
                current_input = res->remaining;
            }

            return std::optional<Result<std::vector<T>, Stream>>{
                Result<std::vector<T>, Stream>{ std::move(values), current_input }
            };
        };
    }

    /*
     * Apply 1 to X times the parser.
     */
    template <typename Parser>
    auto many1(Parser parser)
    {
        return [=](auto input) {
            using Stream = decltype(input);
            using OptRes = decltype(parser(input));
            using T = OptRes::value_type::value_type;

            auto first_result = parser(input);
            if (!first_result) return std::optional<Result<std::vector<T>, Stream>>{};

            std::vector<T> values;
            values.push_back(std::move(*first_result).value);

            auto current_input = first_result->remaining;

            while (true)
            {
                auto res = parser(current_input);
                if (!res) break;

                values.push_back(std::move(*res).value);
                current_input = res->remaining;
            }

            return std::optional<Result<std::vector<T>, Stream>>{
                Result<std::vector<T>, Stream>{ std::move(values), current_input }
            };
        };
    }

    template <typename Parser>
    auto optional(Parser parser)
    {
        return [=](auto input) {
            using Stream = decltype(input);
            using T = _parser_val_t<Parser, Stream>;

            auto res = parser(input);

            if (res) {
                return std::optional<Result<std::optional<T>, Stream>>{
                    Result<std::optional<T>, Stream>{
                        std::optional<T>{ std::move(res->value) },
                        res->remaining
                    }
                };
            }

            return std::optional<Result<std::optional<T>, Stream>>{
                Result<std::optional<T>, Stream>{ std::nullopt, input }
            };
        };
    }

    inline void report_error(const std::string &error_message, const LexerToken &token)
    {
        std::cout << "Erreur -> " << error_message << " (" << token.line_number << ":" << token.column_number << ")" << std::endl;
    }

    template <FixedString error_message, typename Parser>
    auto lint(Parser parser)
    {
        return [=](auto input) {
            auto res = parser(input);
            if (!res)
            {
                std::string error_message_string;
                error_message_string = error_message.view();
                size_t pos = error_message_string.find("%v");

                if (input.size() != 0 && pos != std::string::npos)
                {
                    std::string token_value = std::string(input[0].value);
                    error_message_string.replace(pos, 2, "\"" + token_value + "\"");
                }
                report_error(error_message_string, input[0]);
            }
            return res;
        };
    }

    template <LexerTokensTypes... tokens>
    struct StopSet {};

    template <typename S1, typename S2>
    struct ConcatStopSet;

    template <LexerTokensTypes... A, LexerTokensTypes... B>
    struct ConcatStopSet<StopSet<A...>, StopSet<B...>> {
        using type = StopSet<A..., B...>;
    };

    template <LexerTokensTypes... tokens>
    struct SyncSet {};

    template <typename S1, typename S2>
    struct ConcatSyncSet;

    template <LexerTokensTypes... A, LexerTokensTypes... B>
    struct ConcatSyncSet<SyncSet<A...>, SyncSet<B...>> {
        using type = SyncSet<A..., B...>;
    };

    template <typename TStopSet, typename TSyncSet, typename TErrorNode, typename Parser>
    auto lint_checkpoint(Parser parser) {
        return _lint_checkpoint_impl<TErrorNode>(parser, TStopSet{}, TSyncSet{});
    }

    template <typename TErrorNode, typename Parser,
              LexerTokensTypes... stop_tokens,
              LexerTokensTypes... sync_tokens>
    auto _lint_checkpoint_impl(Parser parser,
                               StopSet<stop_tokens...>,
                               SyncSet<sync_tokens...>)
    {
        return [=](auto input) -> std::invoke_result_t<Parser, decltype(input)>
        {
            using FullResultType = typename std::invoke_result_t<Parser, decltype(input)>::value_type;

            if (input.empty()) return std::nullopt;

            auto res = parser(input);
            if (res) return res;

            if (((input[0].type == stop_tokens) || ...))
                return std::nullopt;

            auto current_input = input.subspan(1);

            while (!current_input.empty())
            {
                if (((current_input[0].type == stop_tokens) || ...))
                    break;
                if (((current_input[0].type == sync_tokens) || ...))
                    break;
                current_input = current_input.subspan(1);
            }

            return std::optional<FullResultType>{
                FullResultType{std::make_unique<TErrorNode>(), current_input}
            };
        };
    }

    template<typename P>
    auto ref(P& parser)
    {
        return [&parser](auto input)
        {
            return parser(input);
        };
    }

    /* operators */

    /*
     * Try a parser, if didn't work try the second one (<|> equivalent in haskell)
     */
    template <typename ParserA, typename ParserB>
    auto operator|(ParserA parser_a, ParserB parser_b)
    {
        return [=](auto input) -> decltype(parser_a(input))
        {
            auto parser_a_result = parser_a(input);
            if (parser_a_result) return parser_a_result;
            return parser_b(input);
        };
    }

    template <typename T>
    concept IsParser =  requires(T p, TokenSpan ts) { { p(ts) }; } ||
                    requires(T p, CharSpan cs)  { { p(cs) }; };

    /*
     * Applicative right
     * Exec the first parser, if success, exec the second
     * Keep the result of the second one
     */
    template <IsParser ParserA, IsParser ParserB>
    auto operator>>(ParserA parser_a, ParserB parser_b)
    {
        return [=](auto input) -> decltype(parser_b(input))
        {
            auto parser_a_result = parser_a(input);
            if (!parser_a_result) return std::nullopt;
            return parser_b(parser_a_result->remaining);
        };
    }

    /*
     * Applicative left
     * Exec the first parser, if success, exec the second
     * Keep the result of the first one (but token stream of the second)
     */
    template <IsParser ParserA, IsParser ParserB>
    auto operator<<(ParserA parser_a, ParserB parser_b)
    {
        return [=](auto input) -> decltype(parser_a(input))
        {
            auto parser_a_result = parser_a(input);
            if (!parser_a_result) return std::nullopt;

            auto parser_b_result = parser_b(parser_a_result->remaining);
            if (!parser_b_result) return std::nullopt;

            using ResType = decltype(parser_a(input))::value_type;

            return std::optional<ResType>{
                ResType{
                    std::move(*parser_a_result).value,
                    parser_b_result->remaining
                }
            };
        };
    }
}
