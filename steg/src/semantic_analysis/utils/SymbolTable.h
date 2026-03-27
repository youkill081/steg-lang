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

            builtin("print_n", v(T::VOID), {v(T::UINT32, 0)}, "DISPLAY_N");

            builtin("window_create", v(T::VOID), {v(T::UINT32), v(T::UINT32), v(T::UINT8, 1)});
            builtin("window_close", v(T::VOID), {}, "WINDOW_CLOSE");
            builtin("window_set_viewport_size", v(T::VOID), {v(T::UINT32), v(T::UINT32)}, "WINDOW_SET_VIEWPORT_SIZE");
            builtin("window_disable_viewport", v(T::VOID), {}, "WINDOW_DISABLE_VIEWPORT");
            builtin("window_pool", v(T::VOID), {}, "WINDOW_POOL");
            builtin("window_should_close", v(T::BOOL), {}, "WINDOW_SHOULD_CLOSE");
            builtin("window_clear", v(T::VOID), {v(T::UINT8), v(T::UINT8), v(T::UINT8)}, "WINDOW_CLEAR");
            builtin("window_present", v(T::BOOL), {}, "WINDOW_PRESENT");

            builtin("window_key_pressed", v(T::BOOL), {v(T::UINT16)});
            builtin("window_key_down", v(T::BOOL), {v(T::UINT32)});
            builtin("window_set_target_fps", v(T::VOID), {v(T::UINT32)});
            builtin("window_set_text_size", v(T::VOID), {v(T::UINT32)});
            builtin("window_set_text_color", v(T::VOID), {v(T::UINT8), v(T::UINT8), v(T::UINT8)},
                    "WINDOW_SET_TEXT_COLOR");
            builtin("window_set_font", v(T::VOID), {v(T::FILE)});
            builtin("window_draw_text", v(T::VOID), {v(T::UINT32), v(T::UINT32), v(T::UINT8, 1)});
            builtin("window_draw_texture", v(T::VOID), {v(T::UINT32), v(T::UINT32), v(T::FILE)});
            builtin("window_set_texture_color_mask", v(T::VOID), {v(T::UINT8), v(T::UINT8), v(T::UINT8)},
                    "WINDOW_SET_TEXTURE_COLOR_MASK");
            builtin("window_reset_texture_color_mask", v(T::VOID), {}, "WINDOW_RESET_TEXTURE_COLOR_MASK");
            builtin("window_set_icon", v(T::VOID), {v(T::FILE)});

            builtin("file_open", v(T::FILE), {v(T::UINT8, 1)});
            builtin("file_create", v(T::FILE), {v(T::UINT8, 1)});
            builtin("file_save", v(T::VOID), {v(T::FILE)}, "FILE_SAVE");
            builtin("file_delete", v(T::VOID), {v(T::FILE)}, "FILE_DELETE");
            builtin("file_close", v(T::VOID), {v(T::FILE)}, "FILE_CLOSE");
            builtin("file_reset_cursor", v(T::VOID), {v(T::FILE)}, "FILE_RESET_CURSOR");
            builtin("file_clear_data", v(T::VOID), {v(T::FILE)}, "FILE_CLEAR_DATA");
            builtin("file_read_byte", v(T::UINT8), {v(T::FILE)}, "FILE_READ_BYTE");
            builtin("file_read_word", v(T::UINT16), {v(T::FILE)}, "FILE_READ_WORD");
            builtin("file_append_byte", v(T::VOID), {v(T::FILE), v(T::UINT8)}, "FILE_APPEND_BYTE");
            builtin("file_append_word", v(T::VOID), {v(T::FILE), v(T::UINT16)}, "FILE_APPEND_WORD");
            builtin("file_is_byte_remaining", v(T::BOOL), {v(T::FILE)}, "FILE_IS_BYTE_REMAINING");
            builtin("file_is_word_remaining", v(T::BOOL), {v(T::FILE)}, "FILE_IS_WORD_REMAINING");

            builtin("clock_create", v(T::CLOCK), {}, "CLOCK_CREATE");
            builtin("clock_delete", v(T::VOID), {v(T::CLOCK)}, "CLOCK_DELETE");
            builtin("clock_get_elapsed_ms", v(T::UINT32), {v(T::CLOCK)}, "CLOCK_GET_ELAPSED_MS");
            builtin("clock_get_elapsed_s", v(T::UINT32), {v(T::CLOCK)}, "CLOCK_GET_ELAPSED_S");
            builtin("clock_reset", v(T::VOID), {v(T::CLOCK)}, "CLOCK_RESET");
        }
    };
}
