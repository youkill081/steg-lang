//
// Created by Roumite on 18/02/2026.
//

#include "StringHandler.h"

#include <iostream>

std::string StringHandler::get_name()
{
    return "String";
}

ByteBuffer StringHandler::encode(const Parameters& parameters)
{
    if (parameters.get_parameters().empty())
        throw HandlerError("Missing string to encode !");

    ByteBuffer buffer;
    for (auto & c : parameters.get_parameters()[0])
        buffer.append_uint8(c);
    return buffer;
}

void StringHandler::decode(const Parameters& parameters, ByteBuffer& buffer)
{
    std::string end_string;

    while (buffer.remaining_uint8() > 0)
    {
        end_string.push_back(static_cast<char>(buffer.read_uint8()));
    }

    std::cout << "String décoder -> " << end_string << std::endl;
}
