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
#include "linter/Linter.h"

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

    template <LexerTokenType... Ts>
    struct RecoverySet {};

    template <typename T, typename Stream>
    struct Result
    {
        T value;
        Stream remaining;

        using value_type = T;
        using stream_type = Stream;
    };

    template<typename Sig>
    class ErasedCallable;

    template<typename Ret, typename Arg>
    class ErasedCallable<Ret(Arg)>
    {
        struct Base {
            virtual Ret call(Arg) const = 0;
            virtual ~Base() = default;
        };

        template<typename F>
        struct Impl : Base {
            F f;
            explicit Impl(F&& func) : f(std::forward<F>(func)) {}
            Ret call(Arg a) const override { return f(a); }
        };

        std::shared_ptr<Base> ptr;

    public:
        ErasedCallable() = default;
        ErasedCallable(ErasedCallable const&) = default;
        ErasedCallable(ErasedCallable&&) = default;
        ErasedCallable& operator=(ErasedCallable const&) = default;
        ErasedCallable& operator=(ErasedCallable&&) = default;

        template<typename F>
        requires (!std::is_same_v<std::decay_t<F>, ErasedCallable>)
        ErasedCallable(F&& f)
            : ptr(std::make_shared<Impl<std::decay_t<F>>>(std::forward<F>(f)))
        {}

        Ret operator()(Arg a) const { return ptr->call(a); }
        explicit operator bool() const { return bool(ptr); }
    };

    template <typename T, typename Stream>
    using Parser = ErasedCallable<std::optional<Result<T, Stream>>(Stream)>;

    template <typename P, typename S>
    using _parser_val_t = std::invoke_result_t<P, S>::value_type::value_type;

    template<typename T, typename Stream, typename F>
    Parser<T, Stream> to_parser(F&& f)
    {
        return Parser<T, Stream>(std::forward<F>(f));
    }

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
        Linter::instance().report(
            error_message,
            token.path,
            token.line_number,
            token.column_number,
            token.value.size(),
            LintData::Severity::ERR
        );
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

    template <LexerTokenType... tokens>
    struct StopSet {};

    template <typename S1, typename S2>
    struct ConcatStopSet;

    template <LexerTokenType... A, LexerTokenType... B>
    struct ConcatStopSet<StopSet<A...>, StopSet<B...>> {
        using type = StopSet<A..., B...>;
    };

    template <LexerTokenType... tokens>
    struct SyncSet {};

    template <typename S1, typename S2>
    struct ConcatSyncSet;

    template <LexerTokenType... A, LexerTokenType... B>
    struct ConcatSyncSet<SyncSet<A...>, SyncSet<B...>> {
        using type = SyncSet<A..., B...>;
    };

    template <typename TStopSet, typename TSyncSet, typename TErrorNode, typename Parser>
    auto lint_checkpoint(Parser parser) {
        return _lint_checkpoint_impl<TErrorNode>(parser, TStopSet{}, TSyncSet{});
    }

    template <typename TErrorNode, typename Parser,
              LexerTokenType... stop_tokens,
              LexerTokenType... sync_tokens>
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
                FullResultType{std::make_unique<TErrorNode>(LexerToken::dummy()), current_input}
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
    template <typename... Parsers>
auto choice(Parsers... parsers)
    {
        return [=](auto input) {
            using Stream = decltype(input);
            using FirstParser = std::tuple_element_t<0, std::tuple<Parsers...>>;
            using RetType = std::invoke_result_t<FirstParser, Stream>;

            RetType result = std::nullopt;

            auto execute_choice = [&](const auto& parser) -> bool {
                result = parser(input);
                return result.has_value();
            };

            (execute_choice(parsers) || ...);
            return result;
        };
    }

    template <typename T>
    concept IsParser = requires(T p, TokenSpan ts) {
            { p(ts) };
    };

    /*
     * Applicative right
     * Exec the first parser, if success, exec the second
     * Keep the result of the second one
     */
    template <IsParser ParserA, IsParser ParserB>
    auto operator>>(ParserA parser_a, ParserB parser_b)
    {
        using Stream = TokenSpan;
        using T = _parser_val_t<ParserB, Stream>;

        return to_parser<T, Stream>([=](Stream input) -> std::optional<Result<T, Stream>>
        {
            auto res_a = parser_a(input);
            if (!res_a) return std::nullopt;
            return parser_b(res_a->remaining);
        });
    }

    /*
     * Applicative left
     * Exec the first parser, if success, exec the second
     * Keep the result of the first one (but token stream of the second)
     */
    template <IsParser ParserA, IsParser ParserB>
    auto operator<<(ParserA parser_a, ParserB parser_b)
    {
        using Stream = TokenSpan;
        using T = _parser_val_t<ParserA, Stream>;

        return to_parser<T, Stream>([=](Stream input) -> std::optional<Result<T, Stream>>
        {
            auto res_a = parser_a(input);
            if (!res_a) return std::nullopt;

            auto res_b = parser_b(res_a->remaining);
            if (!res_b) return std::nullopt;

            return std::optional<Result<T, Stream>>{
                Result<T, Stream>{
                    std::move(*res_a).value,
                    res_b->remaining
                }
            };
        });
    }
}
