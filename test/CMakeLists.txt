cmake_minimum_required(VERSION 4.0)

project(stegnocode_tests)

set(CMAKE_CXX_STANDARD 20)

# Google Test
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../libs/googletest ${CMAKE_CURRENT_BINARY_DIR}/googletest)

# Tests
add_executable(stegnocode_tests test_steganographer.cpp test_cli_parameters.cpp)

target_link_libraries(stegnocode_tests
        PRIVATE
        gtest
        gtest_main
        stegnocode_lib
)

# Définir le chemin vers les données de test
target_compile_definitions(stegnocode_tests PRIVATE
        TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
)

# Enable testing
enable_testing()
add_test(NAME stegnocode_tests COMMAND stegnocode_tests)
add_test(NAME steg_audio_pcm COMMAND steg run "${CMAKE_CURRENT_SOURCE_DIR}/audio_pcm.steg")
