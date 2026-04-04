//
// Created by Roumite on 03/04/2026.
//

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string_view>
#include <string>

#include "version.h"
#include "Colors.h"

#include "steg_exe.h"
#include "steglsp_exe.h"
#include "mainsteg.h"
#include "stegmd.h"

constexpr std::string_view APP_NAME = R"(
----------------------------------------

   _____ __             ________    ____
  / ___// /____  ____  / ____/ /   /  _/
  \__ \/ __/ _ \/ __ `/ /   / /    / /
 ___/ / /_/  __/ /_/ / /___/ /____/ /
/____/\__/\___/\__, /\____/_____/___/
              /____/

----------------------------------------
)";

void displayHelp() {
    std::cout << AnsiColors::Bold << AnsiColors::Cyan << "USAGE:" << AnsiColors::Reset << "\n";
    std::cout << "  stegcli init <project_name>\n\n";
    std::cout << AnsiColors::Bold << AnsiColors::Cyan << "COMMANDS:" << AnsiColors::Reset << "\n";
    std::cout << "  init    Scaffold a new Steg project with default directories and binaries.\n";
}

void extract_file(const std::filesystem::path& dest, const unsigned char* data, unsigned int len) {
    std::ofstream ofs(dest, std::ios::binary);
    if (!ofs) {
        throw std::runtime_error("Cannot open file for writing: " + dest.string());
    }
    ofs.write(reinterpret_cast<const char*>(data), len);
}

// --- Commande Init ---
int init_project(const std::string& project_name) {
    std::filesystem::path root(project_name);

    if (std::filesystem::exists(root)) {
        std::cerr << AnsiColors::Red << "Error: Directory '" << project_name << "' already exists." << AnsiColors::Reset << "\n";
        return 1;
    }

    try {
        std::cout << AnsiColors::Cyan << "--> Initializing Steg project: " << AnsiColors::Bold << project_name << AnsiColors::Reset << "...\n\n";

        std::cout << "  " << AnsiColors::Yellow << "[1/3]" << AnsiColors::Reset << " Creating directories...\n";
        std::filesystem::create_directories(root / "bin");

        std::cout << "  " << AnsiColors::Yellow << "[2/3]" << AnsiColors::Reset << " Unpacking Steg toolchain...\n";
        extract_file(root / "bin" / "steg.exe", data_steg_exe, data_steg_exe_len);
        extract_file(root / "bin" / "steglsp.exe", data_lsp_exe, data_lsp_exe_len);
        extract_file(root / "main.steg", mainsteg, mainsteg_len);
        extract_file(root / "steg.md", stegmd, stegmd_len);

        std::cout << "\n" << AnsiColors::Bold << AnsiColors::Green << "Success!" << AnsiColors::Reset << " Project created at " << std::filesystem::absolute(root) << "\n\n";
        std::cout << "Next steps:\n";
        std::cout << "  cd " << project_name << "\n";
        std::cout << "  bin\\steg.exe run main.steg\n";

        return 0;

    } catch (const std::exception& e) {
        std::cerr << AnsiColors::Red << "\nFatal Error during initialization: " << e.what() << AnsiColors::Reset << "\n";
        if (std::filesystem::exists(root)) {
            std::filesystem::remove_all(root);
        }
        return 1;
    }
}

int main(int ac, char** av) {
    std::cout << AnsiColors::Cyan << APP_NAME << AnsiColors::Reset << "\n";

    if (ac < 2) {
        displayHelp();
        return 1;
    }

    const std::string command = av[1];

    if (command == "init") {
        if (ac < 3) {
            std::cout << "Usage: stegcli init <project_name>\n";
            return 1;
        }
        return init_project(av[2]);
    }

    std::cerr << AnsiColors::Red << "Unknown command: " << command << AnsiColors::Reset << "\n\n";
    displayHelp();
    return 1;
}