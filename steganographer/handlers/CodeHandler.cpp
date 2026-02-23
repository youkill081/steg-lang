//
// Created by Roumite on 22/02/2026.
//

#include "CodeHandler.h"

#include "../../stegasm/src/compiler/Compiler.h"
#include "../../stegasm/src/interpreter/Vm.h"

std::string CodeHandler::get_name()
{
    return "code";
}

ByteBuffer CodeHandler::encode(const Parameters& parameters)
{
    if (parameters.get_parameters().empty())
        throw HandlerError("Missing file to encode !");

    return compiler::Compiler::compile(parameters.get_parameters()[0]);
}

void CodeHandler::decode(const Parameters& parameters, ByteBuffer& buffer)
{
    Vm::run(buffer);
}


