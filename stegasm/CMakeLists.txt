cmake_minimum_required(VERSION 4.0)

project(stegasm)

set(CMAKE_CXX_STANDARD 20)

set(SOURCES
        src/main.cpp
)

add_executable(stegasm ${SOURCES})

target_link_libraries(stegasm PRIVATE stegcommon)
target_include_directories(stegasm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../libs/stegcommon)
