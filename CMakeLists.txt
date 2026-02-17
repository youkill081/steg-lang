cmake_minimum_required(VERSION 4.0)

project(stegnocode)

set(APP_NAME stegnocode)

set(CMAKE_CXX_STANDARD 20)

set(SOURCES
        main.cpp
)

add_executable(${APP_NAME} ${SOURCES})

