//
// Created by Roumite on 23/02/2026.
//

#pragma once

#include <string>
#include <cstdint>

class Runtime;

class Utils
{
private:
    Runtime &_runtime;

    uint32_t _last_image_width = 0;
    uint32_t _last_image_height = 0;
public:
    explicit Utils(Runtime &runtime) : _runtime(runtime) {}

    [[nodiscard]] std::string get_string_from_address(uint32_t address) const;

    uint32_t load_image_and_map(const std::string &path);
    [[nodiscard]] uint32_t get_last_image_width() const { return _last_image_width; }
    [[nodiscard]] uint32_t get_last_image_height() const { return _last_image_height; }
};
