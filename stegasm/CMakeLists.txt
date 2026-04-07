cmake_minimum_required(VERSION 4.0)

project(stegasm)

set(CMAKE_CXX_STANDARD 23)

set(STEGASM_LIB_SOURCES
        src/instructions.h
        src/interpreter/runtime/Runtime.h
        src/assembler/TextParser.cpp
        src/assembler/TextParser.h
        src/assembler/Assembler.cpp
        src/assembler/Assembler.h
        src/instruction_utils.hpp
        src/interpreter/Loader.cpp
        src/interpreter/Loader.h
        src/interpreter/runtime/Memory.h
        src/interpreter/Vm.cpp
        src/interpreter/Vm.h
        src/interpreter/runtime/Memory.cpp
        src/interpreter/exceptions.h
        src/interpreter/runtime/Registries.cpp
        src/interpreter/runtime/Registries.h
        src/instructions.cpp
        src/interpreter/runtime/GraphicalBackend.h
        src/interpreter/runtime/GraphicalBackend.cpp
        src/interpreter/runtime/Utils.cpp
        src/interpreter/runtime/Utils.h
        src/interpreter/runtime/files/File.cpp
        src/interpreter/runtime/files/File.h
        src/assembler/utils/VariableSet.cpp
        src/assembler/utils/VariableSet.h
        src/assembler/assembler_exception.h
        src/assembler/utils/InstructionSet.cpp
        src/assembler/utils/InstructionSet.h
        src/assembler/utils/FileSet.cpp
        src/assembler/utils/FileSet.h
        src/assembler/utils/utils.h
        src/assembler/utils/utils.cpp
        src/assembler/utils/SubtexturesSet.cpp
        src/assembler/utils/SubtexturesSet.h
        src/assembler/Symbol.h
        src/assembler/ISymbolSource.h
        src/assembler/Symbol.cpp
        src/assembler/utils/LabelSet.cpp
        src/assembler/utils/LabelSet.h
        src/assembler/Linter.cpp
        src/assembler/Linter.h
        src/interpreter/runtime/files/FileBase.h
        src/interpreter/runtime/files/Subtexture.h
        src/interpreter/runtime/files/Files.cpp
        src/interpreter/runtime/files/Files.h
        src/interpreter/runtime/Clocks.cpp
        src/interpreter/runtime/Clocks.h
        src/interpreter/runtime/Stack.h
        src/interpreter/runtime/InstructionView.h
        src/interpreter/runtime/InstructionViewImpl.h
        src/interpreter/runtime/TimeDebugger.h
)

add_library(stegasm_lib STATIC ${STEGASM_LIB_SOURCES})

target_include_directories(stegasm_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(stegasm_lib PUBLIC stegcommon)
target_link_libraries(stegasm_lib PUBLIC raylib)
