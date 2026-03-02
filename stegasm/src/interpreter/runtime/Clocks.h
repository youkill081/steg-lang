//
// Created by Roumite on 02/03/2026.
//

#pragma once

#include <map>
#include <memory>
#include <chrono>

class Clock
{
private:
    std::chrono::steady_clock::time_point _start_time;
public:
    Clock();

    uint16_t get_elapsed_time_ms() const;
    uint16_t get_elapsed_time_s() const;

    void reset();
};

class Clocks
{
private:
    std::map<uint16_t, std::shared_ptr<Clock>> _clocks{};
    uint16_t _current_descriptor = 0;
public:
    uint16_t create_clock();
    void delete_clock(uint16_t descriptor);

    uint16_t get_clock_time_ms(uint16_t descriptor);
    uint16_t get_clock_time_s(uint16_t descriptor);

    void reset_clock(uint16_t descriptor);
};