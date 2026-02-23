//
// Created by Roumite on 18/02/2026.
//

#include "Parameters.h"

#include <iostream>

void Parameters::display_help()
{
    std::cout << HELP_MESSAGE << std::endl;
    throw ParametersError();
}

#pragma warning(push)
#pragma warning(disable : 4715)

// ReSharper disable once CppNotAllPathsReturnValue
Mode Parameters::parse_mode(const std::string &mode)
{
    if (mode == std::string("encode")) return ENCODE;
    if (mode == std::string("decode")) return DECODE;

    std::cout << "Unknown mode: " << mode << std::endl;
    display_help();
}

// ReSharper disable once CppNotAllPathsReturnValue
DataType Parameters::parse_type(const std::string &type)
{
    if (type == std::string("string")) return STRING;
    if (type == std::string("file")) return FILE_T;
    if (type == std::string("code")) return CODE;

    std::cout << "Unknown type: " << type << std::endl;
    display_help();
}

#pragma warning(pop)

std::vector<std::string> Parameters::parse_parameters(int ac, char** av)
{
    std::vector<std::string> parameters;

    for (int i = 3; i < ac-2; ++i)
        parameters.emplace_back(av[i]);
    return parameters;
}

std::string Parameters::parse_image_path(int ac, char** av)
{
    if (mode == DECODE)
        return std::string{av[2]};
    else
        return std::string{av[ac-2]};
}

std::string Parameters::parse_output_path(int ac, char** av)
{
    return std::string{av[ac-1]};
}

Parameters::Parameters(int ac, char** av)
{
    if (ac < 3) display_help();
    mode = parse_mode(av[1]);
    image_path = parse_image_path(ac, av);

    if (mode == DECODE)
    {
        image_path = parse_image_path(ac, av);
        output_path.clear();
        parameters.clear();
        return;
    }

    if (ac < 6) display_help();

    type = parse_type(av[2]);
    parameters = parse_parameters(ac, av);
    output_path = parse_output_path(ac, av);
}
