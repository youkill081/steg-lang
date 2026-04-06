//
// Created by Roumite on 22/02/2026.
//

#pragma once

#include <string>

/*
 * AI function to check if two string are equal, case_insensitive
 */
constexpr bool string_equal(const std::string_view& a, const std::string_view& b)
{
    return std::equal(a.begin(), a.end(),
                      b.begin(), b.end(),
                      [](char a, char b)
                      {
                          return std::tolower(static_cast<unsigned char>(a)) ==
                              std::tolower(static_cast<unsigned char>(b));
                      });
}

inline bool is_float(const std::string &token)
{
    return (token.find('.') != std::string::npos) ||
        token.ends_with('f') ||
        token.ends_with('F');
}

inline bool token_is_numeric_value(const std::string &token)
{
    if (token.empty()) return false;

    if (is_float(token))
    {
        std::string clean_token = token;
        if (clean_token.ends_with('f') || clean_token.ends_with('F')) {
            clean_token.pop_back();
        }

        float result;
        auto [ptr, ec] = std::from_chars(clean_token.data(), clean_token.data() + clean_token.size(), result);

        return ec == std::errc() && ptr == clean_token.data() + clean_token.size();
    }
    uint32_t result;
    auto [ptr, ec] = std::from_chars(token.data(), token.data() + token.size(), result);
    return ec == std::errc() && ptr == token.data() + token.size();

}

inline uint32_t token_to_numeric(const std::string &token)
{
    if (is_float(token))
    {
        std::string clean_token = token;
        if (clean_token.ends_with('f') || clean_token.ends_with('F')) {
            clean_token.pop_back();
        }

        float result;
        auto [ptr, ec] = std::from_chars(clean_token.data(), clean_token.data() + clean_token.size(), result);

        if (ec == std::errc() && ptr == clean_token.data() + clean_token.size()) {
            return std::bit_cast<uint32_t>(result);
        }
    } else
    {
        uint32_t result;
        auto [ptr, ec] = std::from_chars(token.data(), token.data() + token.size(), result);

        if (ec == std::errc() && ptr == token.data() + token.size()) {
            return result;
        }
    }

    return 0;
}