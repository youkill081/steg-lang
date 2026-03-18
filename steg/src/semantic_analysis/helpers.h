//
// Created by Roumite on 18/03/2026.
//

#pragma once

#include <filesystem>
#include <optional>

namespace compiler::utils {
    inline std::optional<std::filesystem::path> resolve_path(
        const std::filesystem::path& current_file_path,
        const std::string& target_path_str)
    {
        std::filesystem::path target(target_path_str);
        std::filesystem::path source_dir = current_file_path.parent_path();
        std::filesystem::path exec_dir = std::filesystem::current_path();

        if (std::filesystem::exists(source_dir / target)) return source_dir / target;
        if (std::filesystem::exists(exec_dir / target)) return exec_dir / target;

        return std::nullopt;
    }
}
