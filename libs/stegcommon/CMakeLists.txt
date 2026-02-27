cmake_minimum_required(VERSION 4.0)

project(stegcommon)

set(CMAKE_CXX_STANDARD 23)

add_library(stegcommon STATIC
        ByteBuffer.cpp
        ByteBuffer.h
        Logger.cpp
        Logger.h
        utils/utils.hpp
        utils.h
        Observable.h
        Colorrs.h
)

target_include_directories(stegcommon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
