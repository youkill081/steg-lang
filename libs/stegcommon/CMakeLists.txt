cmake_minimum_required(VERSION 4.0)

project(stegcommon)

set(CMAKE_CXX_STANDARD 20)

add_library(stegcommon STATIC
        ByteBuffer.cpp
        ByteBuffer.h
)

target_include_directories(stegcommon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
