cmake_minimum_required(VERSION 4.0)

project(stegnocode VERSION 0.0.1)

option(ENABLE_INSTRUCTION_COUNTER "If enable, display in the console the number of operation computed each second" ON)

if(MINGW)
    add_compile_options("-Wa,-mbig-obj")
    set(CMAKE_AR "gcc-ar")
    set(CMAKE_RANLIB "gcc-ranlib")
endif()

if (WIN32 AND MINGW)
    add_link_options("-static-libgcc" "-static-libstdc++" "-static")
endif ()

set(APP_NAME stegnocode)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(JSON_BuildTests OFF CACHE INTERNAL "")

# Gen version header and create a lib
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/version.h.in"
        "${CMAKE_CURRENT_BINARY_DIR}/generated/version.h"
)
add_library(project_version INTERFACE)
target_include_directories(project_version INTERFACE "${CMAKE_CURRENT_BINARY_DIR}/generated")

# Steg common lib
add_subdirectory(libs/stegcommon)

# External libraries
add_subdirectory(libs/json)
add_subdirectory(libs/raylib)


# -------------------
# All Project of Steg
# -------------------

# Compiler optimisations
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_options(
            $<$<CONFIG:Release>:-O3>

            $<$<CONFIG:RelWithDebInfo>:-O3>
    )
    add_link_options(
            $<$<CONFIG:Release>:-s>
    )
endif()

# Compilation flag
if(ENABLE_INSTRUCTION_COUNTER)
    add_compile_definitions(ENABLE_INSTRUCTION_COUNTER)
endif()

add_subdirectory(steg)
add_subdirectory(stegasm)
add_subdirectory(steg/lsp)
add_subdirectory(steganographer)

add_subdirectory(test)
