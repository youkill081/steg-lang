//
// Created by Roumite on 21/02/2026.
//

#pragma once

#include <array>

#include "../../instructions.h"

class Registries
{
private:
    std::array<uint32_t, number_of_registries> registries{};
public:
    void write(uint16_t registry_number, uint32_t value);
    void write(RegNames registry_name, uint32_t value);

    [[nodiscard]] uint32_t read(RegNames registry_name) const;
    [[nodiscard]] uint32_t read(uint16_t registry_number) const;

    void display() const;
};
