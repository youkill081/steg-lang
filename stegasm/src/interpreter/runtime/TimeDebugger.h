//
// Created by Roumite on 06/04/2026.
//

#pragma once
#include <chrono>
#include <iostream>
#include <unordered_map>

class TimeDebugger {
public:
    static void process_debug_t(uint32_t clock_id) {
        auto now = std::chrono::high_resolution_clock::now();

        auto it = clocks.find(clock_id);

        if (it == clocks.end()) {
            clocks[clock_id] = now;
            std::cout << "[DEBUG_TIME]: Init Clock ID <" << clock_id << ">" << std::endl;
        } else {
            const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - it->second);

            std::cout << "[DEBUG_TIME]: Clock ID <" << clock_id
                      << "> | Delta: " << std::fixed << std::setprecision(3)
                      << duration.count() / 1000.0 << " ms" << std::endl;

            it->second = now;
        }
    }

private:
    static inline std::unordered_map<uint32_t, std::chrono::high_resolution_clock::time_point> clocks;
};
