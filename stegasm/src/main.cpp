//
// stegasm - Steganography Assembly
//

#include <iostream>
#include "ByteBuffer.h"
#include "Logger.h"
#include "assembler/Assembler.h"
#include "assembler/assembler_exception.h"
#include "interpreter/Vm.h"

int main(int argc, char* argv[])
{
    try
    {
        assembler::Linter linter;
        auto binary = assembler::Assembler::assemble(argv[1], linter);
        std::cout << "Successfully compiled " << binary.size() << " bytes" << std::endl;
        Vm::run(binary);
    } catch (assembler::AssemblerError &) { return 1; }
    catch (std::exception &e)
    {
        std::cerr << "Error -> " << e.what() << std::endl;
    }
    return 0;
}
