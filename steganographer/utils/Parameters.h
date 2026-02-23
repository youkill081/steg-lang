//
// Created by Roumite on 18/02/2026.
//

#pragma once

#include <exception>
#include <sstream>
#include <string>
#include <vector>

#include "../steganographer/constant.h"

class ParametersError final : std::exception {};

enum Mode
{
    ENCODE,
    DECODE
};

inline std::string mode_to_string(Mode mode)
{
    switch (mode)
    {
        case ENCODE: return "ENCODE";
        case DECODE: return "DECODE";
        default: return "UNKNOWN";
    }
}

inline constexpr std::string_view HELP_MESSAGE = R"(Usage:
  stegnocode encode <string|file|code> <options...> <input_image_path> <output_image_path>
  stegnocode decode <image_path>
)";

class Parameters
{
private:
    Mode mode;
    DataType type;
    std::vector<std::string> parameters;
    std::string image_path;
    std::string output_path;

    static void display_help();

    static Mode parse_mode(const std::string &mode);
    static DataType parse_type(const std::string &type);
    static std::vector<std::string> parse_parameters(int ac, char **av);
    std::string parse_image_path(int ac,char **av);
    static std::string parse_output_path(int ac, char **av);
public:
    Parameters(int ac, char **av);

    [[nodiscard]] const Mode& get_mode() const { return mode; }
    [[nodiscard]] const DataType& get_type() const { return type; }
    [[nodiscard]] const std::vector<std::string>& get_parameters() const { return parameters; }
    [[nodiscard]] const std::string& get_image_path() const { return image_path; }
    [[nodiscard]] const std::string& get_output_path() const { return output_path; }
};

namespace std
{
    inline std::string to_string(Parameters usage)
    {
        std::ostringstream oss;
        oss << "Mode -> " << mode_to_string(usage.get_mode()) << "\n"
            << "Type -> " << data_type_to_string(usage.get_type()) << "\n"
            << "Input Image Path -> " << usage.get_image_path() << "\n"
            << "Output Image Path -> " << usage.get_output_path() << "\n"
            << "Parameters -> ";
        for (const auto &param : usage.get_parameters())
            oss << param << " ";
        return oss.str();
    }
}

