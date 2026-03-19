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
            this->declare("window_create", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::VOID, 0),
                .param_types = {
                    ResolvedType::from(ASTTypeNode::Types::UINT32, 0) ,
                    ResolvedType::from(ASTTypeNode::Types::UINT32, 0),
                    ResolvedType::from(ASTTypeNode::Types::UINT8, 1)
                }
            });

            this->declare("window_close", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::VOID),
                .param_types = {  }
            });

            this->declare("window_set_viewport_size", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::VOID),
                .param_types = {
                    ResolvedType::from(ASTTypeNode::Types::UINT32, 0),
                    ResolvedType::from(ASTTypeNode::Types::UINT32, 0)
                }
            });

            this->declare("window_disable_viewport", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::VOID),
                .param_types = { }
            });

            this->declare("window_pool", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::VOID),
                .param_types = { }
            });

            this->declare("window_should_close", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::BOOL, 0),
                .param_types = { }
            });

            this->declare("window_clear", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::VOID, 0),
                .param_types = {
                    ResolvedType::from(ASTTypeNode::Types::UINT8, 0),
                    ResolvedType::from(ASTTypeNode::Types::UINT8, 0),
                    ResolvedType::from(ASTTypeNode::Types::UINT8, 0)
                }
            });

            this->declare("window_present", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::BOOL, 0),
                .param_types = {}
            });

            this->declare("window_key_pressed", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::BOOL, 0),
                .param_types = { ResolvedType::from(ASTTypeNode::Types::UINT16, 0) }
            });

            this->declare("window_key_down", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::BOOL, 0),
                .param_types = { ResolvedType::from(ASTTypeNode::Types::UINT32, 0) }
            });

            this->declare("window_set_target_fps", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::VOID),
                .param_types = { ResolvedType::from(ASTTypeNode::Types::UINT32, 0) }
            });

            this->declare("window_set_text_size", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::VOID),
                .param_types = { ResolvedType::from(ASTTypeNode::Types::UINT32, 0) }
            });

            this->declare("window_set_text_color", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::VOID),
                .param_types = {
                    ResolvedType::from(ASTTypeNode::Types::UINT8, 0),
                    ResolvedType::from(ASTTypeNode::Types::UINT8, 0),
                    ResolvedType::from(ASTTypeNode::Types::UINT8, 0)
                }
            });

            this->declare("window_set_font", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::VOID),
                .param_types = { ResolvedType::from(ASTTypeNode::Types::FILE, 0) }
            });

            this->declare("window_draw_text", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::VOID),
                .param_types = {
                    ResolvedType::from(ASTTypeNode::Types::UINT32, 0),
                    ResolvedType::from(ASTTypeNode::Types::UINT32, 0),
                    ResolvedType::from(ASTTypeNode::Types::UINT8, 1)
                }
            });

            this->declare("window_draw_texture", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::VOID),
                .param_types = {
                    ResolvedType::from(ASTTypeNode::Types::UINT32, 0),
                    ResolvedType::from(ASTTypeNode::Types::UINT32, 0),
                    ResolvedType::from(ASTTypeNode::Types::FILE, 0)
                }
            });

            this->declare("window_set_texture_color_mask", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::VOID),
                .param_types = {
                    ResolvedType::from(ASTTypeNode::Types::UINT8, 0),
                    ResolvedType::from(ASTTypeNode::Types::UINT8, 0),
                    ResolvedType::from(ASTTypeNode::Types::UINT8, 0)
                }
            });

            this->declare("window_reset_texture_color_mask", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::VOID),
                .param_types = {}
            });

            this->declare("window_set_icon", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::VOID),
                .param_types = { ResolvedType::from(ASTTypeNode::Types::FILE, 0) }
            });

            this->declare("file_open", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::FILE, 0),
                .param_types = { ResolvedType::from(ASTTypeNode::Types::UINT8, 1) }
            });

            this->declare("file_create", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::FILE, 0),
                .param_types = { ResolvedType::from(ASTTypeNode::Types::UINT8, 1) }
            });

            this->declare("file_save", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::VOID),
                .param_types = { ResolvedType::from(ASTTypeNode::Types::FILE, 0) }
            });

            this->declare("file_delete", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::VOID),
                .param_types = { ResolvedType::from(ASTTypeNode::Types::FILE, 0) }
            });

            this->declare("file_close", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::VOID),
                .param_types = { ResolvedType::from(ASTTypeNode::Types::FILE, 0) }
            });

            this->declare("file_reset_cursor", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::VOID),
                .param_types = { ResolvedType::from(ASTTypeNode::Types::FILE, 0) }
            });

            this->declare("file_clear_data", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::VOID),
                .param_types = { ResolvedType::from(ASTTypeNode::Types::FILE, 0) }
            });

            this->declare("file_read_byte", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::UINT8, 0),
                .param_types = { ResolvedType::from(ASTTypeNode::Types::FILE, 0) }
            });

            this->declare("file_read_word", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::UINT16, 0),
                .param_types = { ResolvedType::from(ASTTypeNode::Types::FILE, 0) }
            });

            this->declare("file_append_byte", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::VOID),
                .param_types = {
                    ResolvedType::from(ASTTypeNode::Types::FILE, 0),
                    ResolvedType::from(ASTTypeNode::Types::UINT8, 0)
                }
            });

            this->declare("file_append_word", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::VOID),
                .param_types = {
                    ResolvedType::from(ASTTypeNode::Types::FILE, 0),
                    ResolvedType::from(ASTTypeNode::Types::UINT16, 0)
                }
            });

            this->declare("file_is_byte_remaining", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::BOOL, 0),
                .param_types = { ResolvedType::from(ASTTypeNode::Types::FILE, 0) }
            });

            this->declare("file_is_word_remaining", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type = ResolvedType::from(ASTTypeNode::Types::BOOL, 0),
                .param_types = { ResolvedType::from(ASTTypeNode::Types::FILE, 0) }
            });

            this->declare("clock_create", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type =  ResolvedType::from(ASTTypeNode::Types::CLOCK, 0),
                .param_types = {}
            });

            this->declare("clock_delete", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type =  ResolvedType::from(ASTTypeNode::Types::VOID),
                .param_types = {ResolvedType::from(ASTTypeNode::Types::CLOCK, 0)}
            });

            this->declare("clock_get_elapsed_ms", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type =  ResolvedType::from(ASTTypeNode::Types::UINT32, 0),
                .param_types = {ResolvedType::from(ASTTypeNode::Types::CLOCK, 0)}
            });

            this->declare("clock_get_elapsed_s", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type =  ResolvedType::from(ASTTypeNode::Types::UINT32, 0),
                .param_types = {ResolvedType::from(ASTTypeNode::Types::CLOCK, 0)}
            });

            this->declare("clock_reset", SymbolInfo{
                .kind = SymbolKind::BUILT_IN_FUNCTION,
                .type =  ResolvedType::from(ASTTypeNode::Types::VOID),
                .param_types = {ResolvedType::from(ASTTypeNode::Types::CLOCK, 0)}
            });
        }
    };
}