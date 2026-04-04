cmake_minimum_required(VERSION 4.0)

project(stegnocode VERSION 0.0.1)

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

# All subprojects
add_subdirectory(steg)
add_subdirectory(stegcli)
add_subdirectory(stegasm)
add_subdirectory(steg/lsp)
add_subdirectory(steganographer)

add_subdirectory(test)