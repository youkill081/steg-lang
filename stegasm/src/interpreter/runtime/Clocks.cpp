//
// Created by Roumite on 02/03/2026.
//

#include "Clocks.h"

#include "interpreter/exceptions.h"

/*
 * Clock
 */

Clock::Clock()
{
    this->reset();
}

uint16_t Clock::get_elapsed_time_ms() const
{
    const auto now = std::chrono::steady_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - _start_time);
    return static_cast<uint16_t>(duration.count());
}

uint16_t Clock::get_elapsed_time_s() const
{
    const auto now = std::chrono::steady_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - _start_time);
    return static_cast<uint16_t>(duration.count());

}

void Clock::reset()
{
    _start_time = std::chrono::steady_clock::now();
}

/*
 * Clocks
 */

uint16_t Clocks::create_clock()
{
    _clocks[++_current_descriptor] = std::make_shared<Clock>();
    return _current_descriptor;
}

void Clocks::delete_clock(uint16_t descriptor)
{
    if (not _clocks.contains(descriptor))
        return;
    _clocks.erase(descriptor);
}

uint16_t Clocks::get_clock_time_ms(uint16_t descriptor)
{
    if (not _clocks.contains(descriptor))
        throw ClockError("Clock descriptor not found");
    return _clocks[descriptor]->get_elapsed_time_ms();
}

uint16_t Clocks::get_clock_time_s(uint16_t descriptor)
{
    if (not _clocks.contains(descriptor))
        throw ClockError("Clock descriptor not found");
    return _clocks[descriptor]->get_elapsed_time_s();
}

void Clocks::reset_clock(uint16_t descriptor)
{
    if (not _clocks.contains(descriptor))
        throw ClockError("Clock descriptor not found");
    return _clocks[descriptor]->reset();
}


