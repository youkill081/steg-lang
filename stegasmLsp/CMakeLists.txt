cmake_minimum_required(VERSION 4.0)

project(stegasmlsp)

set(APP_NAME stegasmlsp)

set(CMAKE_CXX_STANDARD 23)

set(STEGASMLSP_SRC
    src/main.cpp
)

add_executable(stegasmlsp ${STEGASMLSP_SRC})

target_link_libraries(stegasmlsp lsp)
target_link_libraries(stegasmlsp stegasm_lib)