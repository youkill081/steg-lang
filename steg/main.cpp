//
// Created by Roumite on 02/04/2026.
//

/* Steg Compiler CLI interface */

#include <iostream>

#include "Colors.h"
#include "version.h"
#include "compiler.h"
#include "Steganographer.h"
#include "../steganographer/handlers/handlers.h"
#include "assembler/Assembler.h"
#include "interpreter/Vm.h"
#include "linter/Linter.h"
#include "parser/parser_program.h"
#include "../steganographer/images/Image.h"
#include "sequences/ReadSequence.h"

constexpr std::string_view APP_NAME = R"(
------------------------------------------------------------------------------------------------

          _____                _____                    _____                    _____
         /\    \              /\    \                  /\    \                  /\    \
        /::\    \            /::\    \                /::\    \                /::\    \
       /::::\    \           \:::\    \              /::::\    \              /::::\    \
      /::::::\    \           \:::\    \            /::::::\    \            /::::::\    \
     /:::/\:::\    \           \:::\    \          /:::/\:::\    \          /:::/\:::\    \
    /:::/__\:::\    \           \:::\    \        /:::/__\:::\    \        /:::/  \:::\    \
    \:::\   \:::\    \          /::::\    \      /::::\   \:::\    \      /:::/    \:::\    \
  ___\:::\   \:::\    \        /::::::\    \    /::::::\   \:::\    \    /:::/    / \:::\    \
 /\   \:::\   \:::\    \      /:::/\:::\    \  /:::/\:::\   \:::\    \  /:::/    /   \:::\ ___\
/::\   \:::\   \:::\____\    /:::/  \:::\____\/:::/__\:::\   \:::\____\/:::/____/  ___\:::|    |
\:::\   \:::\   \::/    /   /:::/    \::/    /\:::\   \:::\   \::/    /\:::\    \ /\  /:::|____|
 \:::\   \:::\   \/____/   /:::/    / \/____/  \:::\   \:::\   \/____/  \:::\    /::\ \::/    /
  \:::\   \:::\    \      /:::/    /            \:::\   \:::\    \       \:::\   \:::\ \/____/
   \:::\   \:::\____\    /:::/    /              \:::\   \:::\____\       \:::\   \:::\____\
    \:::\  /:::/    /    \::/    /                \:::\   \::/    /        \:::\  /:::/    /
     \:::\/:::/    /      \/____/                  \:::\   \/____/          \:::\/:::/    /
      \::::::/    /                                 \:::\    \               \::::::/    /
       \::::/    /                                   \:::\____\               \::::/    /
        \::/    /                                     \::/    /                \::/____/
         \/____/                                       \/____/

------------------------------------------------------------------------------------------------
)";

void displayHelp() {
    std::cout << AnsiColors::Bold << AnsiColors::Cyan << "USAGE:" << AnsiColors::Reset << "\n";
    std::cout << "  steg run <file.stegasm|file.steg> [-d]\n";
    std::cout << "  steg build <file.stegasm|file.steg> <input.png> <output.png>\n";
    std::cout << "  steg run_img <img.png>\n\n";
    std::cout << AnsiColors::Bold << AnsiColors::Cyan << "OPTIONS:" << AnsiColors::Reset << "\n";
    std::cout << "  -d    Enable debug mode for compiler (Display, reg allocation, IR and AST code)\n";
}

int run(int ac, char** av);
int build(int ac, char** av);
int run_img(int ac, char** av);

int main(int ac, char** av)
{
    std::cout << AnsiColors::Cyan << APP_NAME << AnsiColors::Reset << std::endl;
    std::cout << AnsiColors::Bold << " Version: " << AnsiColors::Green << Config::VERSION << AnsiColors::Reset << "\n\n";

    if (ac < 2) {
        displayHelp();
        return 1;
    }

    const std::string command = av[1];
    if (command == "run") {
        if (ac < 3) {
            std::cerr << AnsiColors::Red << "Error: Missing file to run." << AnsiColors::Reset << std::endl;
            return 1;
        }
        return run(ac, av);
    }
    if (command == "build") {
        if (ac < 5) {
            std::cerr << AnsiColors::Red << "Error: Build requires <file> <in.png> <out.png>" << AnsiColors::Reset << std::endl;
            return 1;
        }
        return build(ac, av);
    }
    if (command == "run_img")
    {
        if (ac < 3)
        {
            std::cerr << AnsiColors::Red << "Error: Missing image to run." << AnsiColors::Reset << std::endl;
            return 1;
        }
        return run_img(ac, av);
    }

    std::cerr << AnsiColors::Red << "Unknown command: " << command << AnsiColors::Reset << std::endl;
    displayHelp();

    return 0;
}

/*
 * Utils
 */

bool hasDebugFlag(int ac, char** av) {
    for (int i = 2; i < ac; ++i) {
        if (std::string_view(av[i]) == "-d")
            return true;
    }
    return false;
}

enum FileExtension
{
    STEG,
    STEGASM,
    PNG,
    UNKNOWN
};

FileExtension getFileType(std::string_view path) {
    if (path.ends_with(".steg"))
        return STEG;
    if (path.ends_with(".stegasm"))
        return STEGASM;
    if (path.ends_with(".png"))
        return PNG;
    return UNKNOWN;
}


void display_debug(const compiler::CompilationResult &result)
{
    /* Delimiter */
    std::cout << AnsiColors::Magenta << "|-| ------------------------ |-|" << AnsiColors::Reset << std::endl;

    /* Debug IR */
    std::cout << AnsiColors::Magenta << "[DEBUG] Generated IR" << AnsiColors::Reset << std::endl;
    std::cout << compiler::IRPrinter{result.ir_blocks, result.globals, result.files}.print() << std::endl;

    /* Debug Register Allocation */
    std::cout << AnsiColors::Magenta << "\n\n[DEBUG] Register Allocation" << AnsiColors::Reset << std::endl;
    if (result.registers.reg_map.empty()) {
        std::cout << "No allocation\n";
    } else {
        for (const auto& [key, reg] : result.registers.reg_map) {
            std::cout << "  " << key << " -> " << reg << "\n";
        }
    }

    /* Debug ASM */
    std::cout << AnsiColors::Magenta << "\n\n[DEBUG] Generated ASM" << AnsiColors::Reset << std::endl;
    std::cout << result.asm_output << std::endl;

    /* Delimiter */
    std::cout << AnsiColors::Magenta << "\n\n[DEBUG] End of debug" << std::endl;
    std::cout << "|-| ------------------------ |-|" << AnsiColors::Reset << std::endl;
}

compiler::CompilationResult local_compile(const std::string_view path, const bool debug=false)
{
    auto result = compiler::compile(path);
    if (compiler::Linter::instance().has_errors())
    {
        std::cerr << AnsiColors::Red << "Abort compilations - Error found" << AnsiColors::Reset << std::endl;
        exit(1);
    }
    if (debug)
        display_debug(result.value());
    return std::move(result.value());
}

ByteBuffer process_assembly_result(ByteBuffer &&buffer, const assembler::Linter& linter, const std::string &path)
{
    if (linter.has_errors()) {
        std::cerr << AnsiColors::Red << "Abort assembly - Error found";
        std::cout << " in " << path;
        std::cerr << AnsiColors::Reset << std::endl;
        exit(1);
    }

    const float sizeKo = static_cast<float>(buffer.size()) / 1000.0f;
    std::cout << AnsiColors::Green << "Assembly successful!" << AnsiColors::Reset << "\n";
    std::cout << "  Source: " << path << "\n";
    std::cout << "  Compiled size: " << AnsiColors::Cyan << sizeKo << " Ko" << AnsiColors::Reset << "\n\n";

    return std::move(buffer);
}

ByteBuffer assemble_local(const std::string &output, const std::string &path)
{
    assembler::Linter linter{true};
    auto buffer = assembler::Assembler::assemble_from_text(output, linter, false);
    return process_assembly_result(std::move(buffer), linter, path);
}

ByteBuffer assemble_local_from_file(const std::string &path)
{
    assembler::Linter linter{true};
    auto buffer = assembler::Assembler::assemble(std::string(path), linter);
    return process_assembly_result(std::move(buffer), linter, path);
}

ByteBuffer compile_or_assemble_local(const std::string &file,  bool debug, FileExtension extension)
{
    if (extension == STEG)
    {
        const auto result = local_compile(file, debug);
        return assemble_local(result.asm_output, file);
    }
    return assemble_local_from_file(file);
}

int run_local(ByteBuffer &buffer)
{
    std::cout << AnsiColors::Bold << AnsiColors::Magenta << "Running Virtual Machine..." << AnsiColors::Reset << std::endl;
    return Vm::run(buffer);
}

/*
 * Run
 */

int run(int ac, char** av)
{
    const std::string file_path = av[2];
    const bool debug = hasDebugFlag(ac, av);
    const FileExtension file_type = getFileType(file_path);

    if (file_type == UNKNOWN || file_type == PNG)
    {
        std::cerr << AnsiColors::Red << "Error: File extension must be .steg or .stegasm" << AnsiColors::Reset << std::endl;
        return 1;
    }

    try
    {
        ByteBuffer buffer = compile_or_assemble_local(file_path, debug, file_type);
        return run_local(buffer);
    } catch (const std::exception &e)
    {
        std::cerr << AnsiColors::Red << "Error: " << e.what() << AnsiColors::Reset << std::endl;
        return 1;
    }
}


/*
 * Build
 */

int build(int ac, char** av)
{
    const std::string source_file = av[2];
    const std::string image_in = av[3];
    const std::string image_out = av[4];
    const bool debug = hasDebugFlag(ac, av);

    const FileExtension source_file_type = getFileType(source_file);
    if (source_file_type == UNKNOWN || source_file_type == PNG)
    {
        std::cerr << AnsiColors::Red << "Error: File extension must be .steg or .stegasm" << AnsiColors::Reset << std::endl;
        return 1;
    }

    if (getFileType(image_out) != PNG)
    {
        std::cerr << AnsiColors::Red << "Error: Output images must be .png" << AnsiColors::Reset << std::endl;
        return 1;
    }

    try
    {
        steganographer::Image img_in{image_in};
        const ByteBuffer buffer = compile_or_assemble_local(source_file, debug, source_file_type);

        auto handler = handler_factory(CODE);
        Steganographer::encode(img_in, buffer, CODE);
        img_in.save_png(image_out);
        return 0;
    } catch (const std::exception &e)
    {
        std::cerr << AnsiColors::Red << "Error: " << e.what() << AnsiColors::Reset << std::endl;
        return 1;
    }
}

/* Run Image */

int run_img(int ac, char** av)
{
    try
    {
        steganographer::Image img{av[2]};
        Steganographer::DecodeResult decoded = Steganographer::decode(img);

        auto data_type = static_cast<DataType>(decoded.header.data_type);
        if (data_type != CODE)
        {
            std::cerr << AnsiColors::Red << "Error: This image is not a code image !" << AnsiColors::Reset << std::endl;
            return 1;
        }

        auto handler = handler_factory(CODE);
        return run_local(decoded.data);
    } catch (const ReadSequenceError &)
    {
        std::cerr << AnsiColors::Red << "Error: This image do not contains data !" << AnsiColors::Reset << std::endl;
        return 1;
    } catch (const std::exception &e)
    {
        std::cerr << AnsiColors::Red << "Error: " << e.what() << AnsiColors::Reset << std::endl;
        return 1;
    }
}