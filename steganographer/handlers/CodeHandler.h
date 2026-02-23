//
// Created by Roumite on 22/02/2026.
//

#pragma once

#include "IHander.h"

class CodeHandler final : public IHandler
{
public:
    [[nodiscard]] std::string get_name() override;
    ByteBuffer encode(const Parameters& parameters) override;
    void decode(const Parameters& parameters, ByteBuffer& buffer) override;
};
