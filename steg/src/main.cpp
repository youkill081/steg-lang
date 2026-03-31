//
// Created by Roumite on 14/03/2026.
//

#include <iostream>

#include "compiler.h"
#include "assembler/Assembler.h"
#include "interpreter/Vm.h"
#include "linter/Linter.h"
#include "parser/parser_program.h"


int main()
{
    auto result = compiler::compile("C:/Users/Roumite/CLionProjects/stegnocode/steg/examples/test.steg");
    if (!compiler::Linter::instance().has_errors())
    {
        std::cout << "Compilation successful" << std::endl;
        std::cout << result->asm_output << std::endl;

        assembler::Linter linter;
        auto buffer = assembler::Assembler::assemble_from_text(result->asm_output, linter, false);

        if (linter.has_errors())
        {
            std::cerr << "Errors during assembly" << std::endl;
            for (const auto &error : linter.get_errors())
            {
                std::cerr << error.message << std::endl;
            }

            std::cerr << "Generated assembly :" << std::endl;
            std::cout << result->asm_output << std::endl;
        } else
        {
            try
            {
                return Vm::run(buffer);
            } catch (const std::exception &e)
            {
                std::cerr << e.what() << std::endl;
            }
        }

        return 1;
    }
    std::cerr << "Errors during compilation" << std::endl;
    return 1;
}
