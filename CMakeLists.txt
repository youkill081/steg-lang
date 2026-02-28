cmake_minimum_required(VERSION 4.0)

project(stegnocode)

set(APP_NAME stegnocode)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(libs/lsp-framework)
add_subdirectory(libs/stegcommon)
add_subdirectory(libs/raylib)
add_subdirectory(libs/json)

add_subdirectory(stegasm)
add_subdirectory(stegasmlsp)

add_subdirectory(test)

set(LIB_SOURCES
        steganographer/images/pixel.h
        steganographer/images/pixels.h
        steganographer/images/Image.cpp
        steganographer/images/Image.h
        steganographer/steganographer/Steganographer.cpp
        steganographer/steganographer/Steganographer.h
        steganographer/steganographer/Random32.h
        steganographer/steganographer/SequenceManager.cpp
        steganographer/steganographer/SequenceManager.h
        steganographer/steganographer/sequences/WriteSequence.cpp
        steganographer/steganographer/sequences/WriteSequence.h
        steganographer/steganographer/constant.h
        steganographer/steganographer/sequences/ReadSequence.cpp
        steganographer/steganographer/sequences/ReadSequence.h
        steganographer/steganographer/header.hpp
        steganographer/utils/Parameters.cpp
        steganographer/utils/Parameters.h
        steganographer/handlers/IHander.h
        steganographer/handlers/StringHandler.cpp
        steganographer/handlers/StringHandler.h
        steganographer/handlers/handlers.h
        steganographer/handlers/FileHandler.cpp
        steganographer/handlers/FileHandler.h
        steganographer/handlers/CodeHandler.cpp
        steganographer/handlers/CodeHandler.h
)

add_library(stegnocode_lib STATIC ${LIB_SOURCES})

target_link_libraries(stegnocode_lib PUBLIC stegcommon) # Link stegcommon
target_link_libraries(stegnocode_lib PUBLIC stegasm_lib) # Link stegasm

target_include_directories(stegnocode_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/steganographer
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/stb
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/stegcommon
)

add_executable(${APP_NAME} main.cpp)
target_link_libraries(${APP_NAME} PRIVATE stegnocode_lib)

