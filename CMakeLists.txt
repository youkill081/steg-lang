cmake_minimum_required(VERSION 4.0)

project(stegnocode)

set(APP_NAME stegnocode)

set(CMAKE_CXX_STANDARD 20)

# Add subdirectories
add_subdirectory(libs/stegcommon)
add_subdirectory(stegasm)

set(SOURCES
        main.cpp
        src/images/pixel.h
        src/images/pixels.h
        src/images/Image.cpp
        src/images/Image.h
        src/utils/Logger.cpp
        src/utils/Logger.h
        src/steganographer/Steganographer.cpp
        src/steganographer/Steganographer.h
        src/steganographer/Random32.h
        src/steganographer/SequenceManager.cpp
        src/steganographer/SequenceManager.h
        src/steganographer/sequences/WriteSequence.cpp
        src/steganographer/sequences/WriteSequence.h
        src/steganographer/constant.h
        src/steganographer/sequences/ReadSequence.cpp
        src/steganographer/sequences/ReadSequence.h
        src/steganographer/header.hpp
        src/utils/Parameters.cpp
        src/utils/Parameters.h
        src/handlers/IHander.h
        src/handlers/StringHandler.cpp
        src/handlers/StringHandler.h
        src/handlers/handlers.h
        src/handlers/FileHandler.cpp
        src/handlers/FileHandler.h
)

add_executable(${APP_NAME} ${SOURCES})

target_link_libraries(${APP_NAME} PRIVATE stegcommon)
target_include_directories(${APP_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/stb
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/stegcommon
)

