//
// Created by Roumite on 23/02/2026.
//

#pragma once

#include <string>
#include <cstdint>
#include <raylib.h>

class GraphicalBackend
{
private:
    bool check_inited(bool throw_if_not_inited = false);

    uint16_t _text_size = 8;
    Color _text_color = WHITE;
public:
    // Windows Management

    void create_window(uint16_t width, uint16_t height, const std::string &title);
    void close_window();
    void poll_events();
    bool should_close();

    void set_target_fps(uint16_t fps);

    // Drawing management

    void clear_window(const Color &color);
    void present_window();

    void set_text_size(const uint16_t size) { _text_size = size; }
    void set_text_color(const Color &color) { _text_color = color; }
    void draw_text(const std::string &text, int x, int y);

    // Input management
    bool key_down(uint16_t key);
    bool key_pressed(uint16_t key);
};