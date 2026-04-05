//
// Created by Roumite on 23/02/2026.
//

#include "GraphicalBackend.h"

#include <cmath>
#include <cstring>

#include "interpreter/exceptions.h"

#include <iostream>

#include "Runtime.h"

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

void draw_texture_internal(
    const Texture2D& texture,
    int x, int y,
    int width, int height,
    const Color &tint,
    const bool flipY = false
)
{
    Rectangle source = {
        0.0f,
        0.0f,
        static_cast<float>(width),
        flipY ? -static_cast<float>(height) : static_cast<float>(height)
    };

    const Rectangle dest = {
        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(width),
        static_cast<float>(height)
    };

    const Vector2 origin = {0.0f, 0.0f};
    DrawTexturePro(texture, source, dest, origin, 0.0f, tint);
}

void GraphicalBackend::draw_texture(const std::shared_ptr<FileBase>& file, int x, int y)
{
    check_inited(true);

    if (!_textures.contains(file))
        load_texture(file);

    const Texture2D& tex = _textures[file];

    if (const auto crop = file->get_crop())
    {
        const Rectangle src = {
            static_cast<float>(crop->x),
            static_cast<float>(crop->y),
            static_cast<float>(crop->w),
            static_cast<float>(crop->h)
        };
        const Rectangle dest = {
            static_cast<float>(x),
            static_cast<float>(y),
            static_cast<float>(crop->w),
            static_cast<float>(crop->h)
        };

        DrawTexturePro(tex, src, dest, {0.0f, 0.0f}, 0.0f, _texture_color_mask);
    }
    else
    {
        draw_texture_internal(
            tex,
            x,
            y,
            tex.width,
            tex.height,
            _texture_color_mask,
            false
        );
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

int32_t GraphicalBackend::mouse_x() const
{
    Vector2 pos = screen_to_viewport(GetMousePosition());
    return static_cast<int32_t>(pos.x);
}

int32_t GraphicalBackend::mouse_y() const
{
    Vector2 pos = screen_to_viewport(GetMousePosition());
    return static_cast<int32_t>(pos.y);
}

int32_t GraphicalBackend::mouse_delta_x() const
{
    return static_cast<int32_t>(GetMouseDelta().x);
}

int32_t GraphicalBackend::mouse_delta_y() const
{
    return static_cast<int32_t>(GetMouseDelta().y);
}

bool GraphicalBackend::mouse_button_pressed(int button) const
{
    return IsMouseButtonPressed(button);
}

bool GraphicalBackend::mouse_button_down(int button) const
{
    return IsMouseButtonDown(button);
}

bool GraphicalBackend::mouse_button_released(int button) const
{
    return IsMouseButtonReleased(button);
}

int32_t GraphicalBackend::mouse_wheel_delta() const
{
    return static_cast<int32_t>(GetMouseWheelMove());
}

void GraphicalBackend::hide_cursor() const
{
    HideCursor();
}

void GraphicalBackend::show_cursor() const
{
    ShowCursor();
}

uint32_t GraphicalBackend::create_framebuffer(Runtime &runtime, uint32_t width, uint32_t height)
{
    static uint32_t current_id = 0;

    auto nbr_uint32 =  width * height;
    uint32_t address = runtime.memory.allocate(nbr_uint32 * sizeof(uint32_t));

    auto &block_data = runtime.memory.get_block(address).data;

    std::memset(block_data.data(), 0xFF, nbr_uint32 * sizeof(uint32_t)); // All white

    this->_framebuffers[current_id++] = Framebuffer{
        .renderTex = LoadRenderTexture(width, height),
        .memoryAddress = address,
        .width = width,
        .height = height,
    };

    return current_id - 1;
}

void GraphicalBackend::sync_framebuffer(Runtime &runtime, uint32_t framebuffer_id)
{
    const Framebuffer &framebuffer = _framebuffers[framebuffer_id];
    auto& data = runtime.memory.get_block(framebuffer.memoryAddress).data;
    UpdateTexture(framebuffer.renderTex.texture, data.data());
}

uint32_t GraphicalBackend::framebuffer_get_address(uint32_t framebuffer_id)
{
    if (not _framebuffers.contains(framebuffer_id))
        throw GraphicalBackendError("Invalid framebuffer id");
    return _framebuffers[framebuffer_id].memoryAddress;
}

void GraphicalBackend::draw_framebuffer(uint32_t framebuffer_id, int x, int y)
{
    check_inited(true);

    auto it = _framebuffers.find(framebuffer_id);
    if (it == _framebuffers.end())
        throw GraphicalBackendError("Invalid framebuffer id");

    const Framebuffer& fb = it->second;

    if (fb.renderTex.id == 0 || fb.renderTex.texture.id == 0)
        throw GraphicalBackendError("Invalid framebuffer texture");

    draw_texture_internal(
        fb.renderTex.texture,
        x,
        y,
        fb.width,
        fb.height,
        _texture_color_mask,
        true
    );
}

Vector2 GraphicalBackend::screen_to_viewport(const Vector2& screenPos) const
{
    if (!_viewport_active || _target.id == 0)
        return screenPos;

    float sx = static_cast<float>(GetScreenWidth())  / static_cast<float>(_viewport_width);
    float sy = static_cast<float>(GetScreenHeight()) / static_cast<float>(_viewport_height);

    return { screenPos.x / sx, screenPos.y / sy };
}

