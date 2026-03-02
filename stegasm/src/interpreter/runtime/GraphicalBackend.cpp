//
// Created by Roumite on 23/02/2026.
//

#include "GraphicalBackend.h"

#include "interpreter/exceptions.h"

#include <iostream>

bool GraphicalBackend::check_inited(bool throw_if_not_inited)
{
    if (not IsWindowReady() && throw_if_not_inited)
        throw GraphicalBackendError("No window created");
    return IsWindowReady();
}

void GraphicalBackend::load_texture(const std::shared_ptr<FileBase> &file)
{
    check_inited(true);

    file->add_callback([this, weak = std::weak_ptr(file)]() {
        if (auto f = weak.lock())
            unload_texture(f);
    });

    this->_textures[file] = LoadTexture(file->get_path().c_str());
}

void GraphicalBackend::unload_texture(const std::shared_ptr<FileBase> &file)
{
    if (_textures.contains(file))
    {
        UnloadTexture(_textures[file]);
        _textures.erase(file);
    }
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

void GraphicalBackend::set_window_icon(const std::shared_ptr<FileBase>& file)
{
    check_inited(true);

    Image icon = LoadImage(file->get_path().c_str());
    if (!IsImageValid(icon))
        throw GraphicalBackendError("Invalid icon");
    SetWindowIcon(icon);
    UnloadImage(icon);
}

void GraphicalBackend::set_viewport_size(uint16_t width, uint16_t height)
{
    check_inited(true);

    if (_viewport_active && _target.id != 0) // Reset viewport
    {
        EndTextureMode();
        UnloadRenderTexture(_target);
        _target.id = 0;
    }

    if (width == 0 || height == 0)
        throw GraphicalBackendError("Viewport size must be > 0");

    _viewport_width  = width;
    _viewport_height = height;

    _target = LoadRenderTexture(_viewport_width, _viewport_height);
    if (_target.id == 0)
        throw GraphicalBackendError("Failed to create render texture for viewport");

    SetTextureFilter(_target.texture, TEXTURE_FILTER_POINT);
    _viewport_active = true;
}

void GraphicalBackend::disable_viewport()
{
    check_inited(true);

    if (_viewport_active && _target.id != 0)
    {
        UnloadRenderTexture(_target);
        _target.id = 0;
    }
    _viewport_active = false;
}

void GraphicalBackend::clear_window(const Color& color)
{
    check_inited(true);

    if (_viewport_active && _target.id != 0)
    {
        BeginTextureMode(_target);
        ClearBackground(color);
    }
    else
    {
        BeginDrawing();
        ClearBackground(color);
    }

    last_clear_color = color;
}

void GraphicalBackend::present_window() const
{
    if (_viewport_active && _target.id != 0)
    {
        EndTextureMode();

        BeginDrawing();
        ClearBackground(last_clear_color);

        float scale = std::floor(
            std::min(
                static_cast<float>(GetScreenWidth()) / _viewport_width,
            static_cast<float>(GetScreenHeight()) / _viewport_height
            )
        );

        float scaledWidth  = _viewport_width  * scale;
        float scaledHeight = _viewport_height * scale;

        float offsetX = (GetScreenWidth()  - scaledWidth)  / 2.0f;
        float offsetY = (GetScreenHeight() - scaledHeight) / 2.0f;

        Rectangle dest = {
            offsetX,
            offsetY,
            scaledWidth,
            scaledHeight
        };
        Rectangle source = { 0.0f, 0.0f, static_cast<float>(_viewport_width), - static_cast<float>(_viewport_height) };
        Vector2 origin = { 0.0f, 0.0f };

        DrawTexturePro(_target.texture, source, dest, origin, 0.0f, WHITE);
    }

    EndDrawing();
}

void GraphicalBackend::set_text_size(const uint16_t size)
{
    check_inited(true);

    _text_size = size;
}


void GraphicalBackend::draw_text(const std::string& text, int x, int y)
{
    check_inited(true);

    if (_font_loaded)
    {
        DrawTextEx(
            _current_font,
            text.c_str(),
            { static_cast<float>(x), static_cast<float>(y) },
            _text_size,
            FONT_TEXT_SPACING,
            _text_color
        );
    }
    else
    {
        DrawText(text.c_str(), x, y, _text_size, _text_color);
    }
}

void GraphicalBackend::set_font(const std::shared_ptr<FileBase>& file)
{
    check_inited(true);

    const std::string& path = file->get_path();
    if (_last_font_path != path)
    {
        Font f = LoadFontEx(path.c_str(), FONT_BASE_SIZE, nullptr, 0);
        if (f.texture.id == 0)
            throw GraphicalBackendError("Failed to load font");

        SetTextureFilter(f.texture, TEXTURE_FILTER_BILINEAR);
        SetTextureFilter(f.texture, TEXTURE_FILTER_BILINEAR);
        _last_font_path = path;
    }

    _last_font_path = path;
    _font_loaded = true;
}

void GraphicalBackend::set_texture_color_mask(const Color &color)
{
    _texture_color_mask = color;
}

void GraphicalBackend::draw_texture(const std::shared_ptr<FileBase> &file, int x, int y)
{
    check_inited(true);

    if (not _textures.contains(file))
        this->load_texture(file);

    if (auto crop = file->get_crop())
    {
        Rectangle src = { (float)crop->x, (float)crop->y, (float)crop->w, (float)crop->h };
        DrawTextureRec(_textures[file], src, { (float)x, (float)y }, _texture_color_mask);
    }
    else
    {
        DrawTexture(_textures[file], x, y, _texture_color_mask);
    }

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

Vector2 GraphicalBackend::screen_to_viewport(const Vector2& screenPos) const
{
    if (!_viewport_active || _target.id == 0)
        return screenPos;

    float sx = static_cast<float>(GetScreenWidth())  / static_cast<float>(_viewport_width);
    float sy = static_cast<float>(GetScreenHeight()) / static_cast<float>(_viewport_height);

    return { screenPos.x / sx, screenPos.y / sy };
}

