//
// Created by Roumite on 18/02/2026.
//

#pragma once

#include "../utils/Parameters.h"
#include "ByteBuffer.h"

class HandlerError final : public std::runtime_error
{
public:
    explicit HandlerError(const std::string& msg) : std::runtime_error(msg) {}
};

class IHandler
{
public:
    virtual ~IHandler() = default;

    [[nodiscard]] virtual std::string get_name() = 0;
    virtual ByteBuffer encode(const Parameters &parameters) = 0;
    virtual void decode(const Parameters &parameters,  ByteBuffer &buffer)  = 0;
};
