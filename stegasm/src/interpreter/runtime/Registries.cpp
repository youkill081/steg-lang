//
// Created by Roumite on 21/02/2026.
//

#include "Registries.h"

#include "Logger.h"
#include "../exceptions.h"
#include "interpreter/runtime/InstructionView.h"

void Registries::write(const RegNames registry_name, const uint32_t value)
{
    this->registries[static_cast<uint16_t>(registry_name)] = value;
}

void Registries::write(const uint16_t registry_number, const uint32_t value)
{
    registries[registry_number] = value;
}

uint32_t Registries::read(const RegNames registry_name) const
{
    return registries[static_cast<uint16_t>(registry_name)];
}

uint32_t Registries::read(uint16_t registry_number) const
{
    return registries[registry_number];
}

void Registries::display() const
{
    for (uint16_t i = 0; i < number_of_registries; i++)
    {
        auto reg = static_cast<RegNames>(i);
        Logger::log(registryToString[reg] + " -> " + std::to_string(registries[i]), "Registries");
    }
}
