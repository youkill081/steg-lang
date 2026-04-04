//
// Created by Roumite on 18/03/2026.
//

#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

#include "../../ast/ASTExpressionNode.h"

namespace compiler
{
    enum class SymbolKind {
        FUNCTION,
        VARIABLE,
        BUILT_IN_FUNCTION,
    };

    struct SymbolInfo {
        SymbolKind kind;
        ResolvedType type; // For variables and functions
        std::vector<ResolvedType> param_types; // For functions
        bool is_exported = false;

        bool is_builtin = false;
        std::optional<std::string> builtin_instruction;

        std::filesystem::path source_file;
        LexerToken token; // For errors
    };

    class SymbolTable {
    public:
        std::vector<std::unordered_map<std::string, std::shared_ptr<SymbolInfo>>> scopes;

        SymbolTable() { enter_scope(); setup_builtin_functions(); }

        void enter_scope() {
            scopes.emplace_back();
        }

        void exit_scope() {
            if (scopes.size() > 1)
                scopes.pop_back();
        }

        [[nodiscard]] bool contains_current(const std::string& name) const { // Search in current scope
            return scopes.back().contains(name);
        }

        [[nodiscard]] bool contains(const std::string& name) const { // Search in all previous scope
            for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
                if (it->contains(name)) return true;
            return false;
        }

        void declare(const std::string& name, const SymbolInfo& info) {
            scopes.back()[name] = std::make_shared<SymbolInfo>(info);
        }

        std::shared_ptr<SymbolInfo> get(const std::string &name) {
            for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
                auto found = it->find(name);
                if (found != it->end())
                    return found->second;
            }
            return nullptr;
        }
    private:
        void setup_builtin_functions()
        {
            auto builtin = [&](
                const std::string& name,
                const ResolvedType &ret,
                std::vector<ResolvedType> params,
                std::optional<std::string> instr = std::nullopt)
            {
                declare(name, SymbolInfo{
                            .kind = SymbolKind::BUILT_IN_FUNCTION,
                            .type = ret,
                            .param_types = std::move(params),
                            .is_builtin = true,
                            .builtin_instruction = std::move(instr),
                        });
            };

            using T = ASTTypeNode::Types;
            auto v = [](const T t, int d = 0) { return ResolvedType::from(t, d); };

            builtin("print_n", v(T::VOID), {v(T::UINT32)}, "DISPLAY_N");
            builtin("print_c", v(T::VOID), {v(T::UINT8)}, "DISPLAY_C");
            builtin("exit", v(T::VOID), {v(T::UINT8)}, "HALT");
            builtin("aloc", v(T::VOID, 1), {v(T::UINT32)}, "ALOC");
            builtin("free", v(T::VOID), {v(T::VOID, 1)}, "FREE");
            builtin("rand", v(T::UINT32), {}, "RAND");

            builtin("window_create", v(T::VOID), {v(T::UINT32), v(T::UINT32), v(T::UINT8, 1)}, "WINDOW_CREATE");
            builtin("window_close", v(T::VOID), {}, "WINDOW_CLOSE");
            builtin("window_set_viewport_size", v(T::VOID), {v(T::UINT32), v(T::UINT32)}, "WINDOW_SET_VIEWPORT_SIZE");
            builtin("window_disable_viewport", v(T::VOID), {}, "WINDOW_DISABLE_VIEWPORT");
            builtin("window_should_close", v(T::BOOL), {}, "WINDOW_SHOULD_CLOSE");
            builtin("window_clear", v(T::VOID), {v(T::UINT8), v(T::UINT8), v(T::UINT8)}, "WINDOW_CLEAR");
            builtin("window_present", v(T::VOID), {}, "WINDOW_PRESENT");
            builtin("window_key_pressed", v(T::BOOL), {v(T::UINT32)}, "WINDOW_KEY_PRESSED");
            builtin("window_key_down", v(T::BOOL), {v(T::UINT32)}, "WINDOW_KEY_DOWN");
            builtin("window_set_target_fps", v(T::VOID), {v(T::UINT32)}, "WINDOW_SET_TARGET_FPS");
            builtin("window_set_text_size", v(T::VOID), {v(T::UINT32)}, "WINDOW_SET_TEXT_SIZE");
            builtin("window_set_text_color", v(T::VOID), {v(T::UINT8), v(T::UINT8), v(T::UINT8)},
                    "WINDOW_SET_TEXT_COLOR");
            builtin("window_set_font", v(T::VOID), {v(T::FILE)}, "WINDOW_SET_FONT");
            builtin("window_draw_text", v(T::VOID), {v(T::UINT32), v(T::UINT32), v(T::UINT8, 1)}, "WINDOW_DRAW_TEXT");
            builtin("window_draw_texture", v(T::VOID), {v(T::UINT32), v(T::UINT32), v(T::FILE)}, "WINDOW_DRAW_TEXTURE");
            builtin("window_set_texture_color_mask", v(T::VOID), {v(T::UINT8), v(T::UINT8), v(T::UINT8)},
                    "WINDOW_SET_TEXTURE_COLOR_MASK");
            builtin("window_reset_texture_color_mask", v(T::VOID), {}, "WINDOW_RESET_TEXTURE_COLOR_MASK");
            builtin("window_set_icon", v(T::VOID), {v(T::FILE)}, "WINDOW_SET_ICON");
            builtin("window_mouse_x", v(T::UINT32), {}, "WINDOW_MOUSE_X");
            builtin("window_mouse_y", v(T::UINT32), {}, "WINDOW_MOUSE_Y");
            builtin("window_mouse_delta_x", v(T::INT32), {}, "WINDOW_MOUSE_DELTA_X");
            builtin("window_mouse_delta_y", v(T::INT32), {}, "WINDOW_MOUSE_DELTA_Y");
            builtin("window_mouse_button_pressed", v(T::BOOL), {v(T::UINT32)}, "WINDOW_MOUSE_BUTTON_PRESSED");
            builtin("window_mouse_button_down", v(T::BOOL), {v(T::UINT32)}, "WINDOW_MOUSE_BUTTON_DOWN");
            builtin("window_mouse_button_released", v(T::BOOL), {v(T::UINT32)}, "WINDOW_MOUSE_BUTTON_RELEASED");
            builtin("window_mouse_wheel_delta", v(T::INT32), {}, "WINDOW_MOUSE_WHEEL_DELTA");
            builtin("window_hide_cursor", v(T::VOID), {}, "WINDOW_HIDE_CURSOR");
            builtin("window_show_cursor", v(T::VOID), {}, "WINDOW_SHOW_CURSOR");

            builtin("file_open", v(T::FILE), {v(T::UINT8, 1)}, "FILE_OPEN");
            builtin("file_create", v(T::FILE), {v(T::UINT8, 1)}, "FILE_CREATE");
            builtin("file_save", v(T::VOID), {v(T::FILE)}, "FILE_SAVE");
            builtin("file_delete", v(T::VOID), {v(T::FILE)}, "FILE_DELETE");
            builtin("file_close", v(T::VOID), {v(T::FILE)}, "FILE_CLOSE");
            builtin("file_reset_cursor", v(T::VOID), {v(T::FILE)}, "FILE_RESET_CURSOR");
            builtin("file_seek_cursor", v(T::VOID), {v(T::UINT32), v(T::FILE)}, "FILE_SEEK_CURSOR");
            builtin("file_clear_data", v(T::VOID), {v(T::FILE)}, "FILE_CLEAR_DATA");
            builtin("file_read_uint8", v(T::UINT8), {v(T::FILE)}, "FILE_READ_BYTE");
            builtin("file_read_uint16", v(T::UINT16), {v(T::FILE)}, "FILE_READ_WORD");
            builtin("file_read_uint32", v(T::UINT32), {v(T::FILE)}, "FILE_READ_DOUBLEWORD");
            builtin("file_write_uint8", v(T::VOID), {v(T::UINT8), v(T::FILE)}, "FILE_APPEND_BYTE");
            builtin("file_write_uint16", v(T::VOID), {v(T::UINT16), v(T::FILE)}, "FILE_APPEND_WORD");
            builtin("file_write_uint32", v(T::VOID), {v(T::UINT32), v(T::FILE)}, "FILE_APPEND_DOUBLEWORD");
            builtin("file_is_uint8_remaining", v(T::BOOL), {v(T::FILE)}, "FILE_IS_BYTE_REMAINING");
            builtin("file_is_uint16_remaining", v(T::BOOL), {v(T::FILE)}, "FILE_IS_WORD_REMAINING");
            builtin("file_is_uint32_remaining", v(T::BOOL), {v(T::FILE)}, "FILE_IS_DOUBLEWORD_REMAINING");

            builtin("clock_create", v(T::CLOCK), {}, "CLOCK_CREATE");
            builtin("clock_delete", v(T::VOID), {v(T::CLOCK)}, "CLOCK_DELETE");
            builtin("clock_get_elapsed_ms", v(T::UINT32), {v(T::CLOCK)}, "CLOCK_GET_ELAPSED_MS");
            builtin("clock_get_elapsed_s", v(T::UINT32), {v(T::CLOCK)}, "CLOCK_GET_ELAPSED_S");
            builtin("clock_reset", v(T::VOID), {v(T::CLOCK)}, "CLOCK_RESET");
        }
    };
}
