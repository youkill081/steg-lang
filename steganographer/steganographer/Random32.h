//
// Created by Roumite on 17/02/2026.
//

#pragma once

#include <random>
#include <cstdint>

class Random32
{
public:
    explicit Random32(uint32_t seed = std::random_device{}())
        : engine(seed)
    {}

    void reseed(uint32_t seed)
    {
        engine.seed(seed);
    }

    uint32_t get_next_rand()
    {
        return dist(engine);
    }


    [[nodiscard]] std::mt19937 &get_engine() { return engine; }
private:
    std::mt19937 engine;
    std::uniform_int_distribution<uint32_t> dist{0, 0xFFFFFFFFu};
};
