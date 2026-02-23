cmake_minimum_required(VERSION 4.0)

project(stegasm)

set(CMAKE_CXX_STANDARD 23)

# On enlève src/main.cpp de cette liste !
set(STEGASM_LIB_SOURCES
        src/instructions.h
        src/interpreter/runtime/Runtime.h
        src/compiler/TextParser.cpp
        src/compiler/TextParser.h
        src/compiler/Compiler.cpp
        src/compiler/Compiler.h
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
)

add_library(stegasm_lib STATIC ${STEGASM_LIB_SOURCES})

target_include_directories(stegasm_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(stegasm_lib PUBLIC stegcommon)

add_executable(stegasm src/main.cpp)

target_link_libraries(stegasm PRIVATE stegasm_lib)