//
// Created by Roumite on 23/02/2026.
//

#include "GraphicalBackend.h"

#include "interpreter/exceptions.h"

#include <iostream>

#include "Logger.h"

bool GraphicalBackend::check_inited(bool throw_if_not_inited)
{
    if (not IsWindowReady() && throw_if_not_inited)
        throw GraphicalBackendError("No window created");
    return IsWindowReady();
}

void GraphicalBackend::create_window(uint16_t width, uint16_t height, const std::string &title)
{
    if (check_inited())
        throw GraphicalBackendError("A window is already created");
    InitWindow(width, height, title.c_str());
}

void GraphicalBackend::close_window()
{
    check_inited(true);

    CloseWindow();
}

void GraphicalBackend::poll_events()
{
    PollInputEvents();
}

bool GraphicalBackend::should_close()
{
    check_inited(true);

    return WindowShouldClose();
}

void GraphicalBackend::set_target_fps(uint16_t fps)
{
    check_inited(true);

    SetTargetFPS(fps);
}

void GraphicalBackend::clear_window(const Color& color)
{
    check_inited(true);

    BeginDrawing();
    ClearBackground(color);
}

void GraphicalBackend::present_window()
{
    check_inited(true);

    EndDrawing();
}

void GraphicalBackend::draw_text(const std::string& text, int x, int y)
{
    check_inited(true);

    DrawText(text.c_str(), x, y, _text_size, _text_color);
}

bool GraphicalBackend::key_down(uint16_t key)
{
    check_inited(true);

    return IsKeyDown(key);
}

bool GraphicalBackend::key_pressed(uint16_t key)
{
    check_inited(true);

    return IsKeyPressed(key);
}

