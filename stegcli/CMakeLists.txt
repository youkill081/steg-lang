cmake_minimum_required(VERSION 4.0)

project(stegcli)

set(CMAKE_CXX_STANDARD 23)

set(GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY ${GEN_DIR})

function(embed_file TARGET_NAME VARIABLE_NAME INPUT_TARGET OUTPUT_HEADER)
    add_custom_command(
            OUTPUT ${OUTPUT_HEADER}
            COMMAND ${CMAKE_COMMAND}
            -DINPUT_FILE=$<TARGET_FILE:${INPUT_TARGET}>
            -DOUTPUT_FILE=${OUTPUT_HEADER}
            -DVARIABLE_NAME=${VARIABLE_NAME}
            -P "${CMAKE_CURRENT_SOURCE_DIR}/file_to_cpp.cmake"
            DEPENDS ${INPUT_TARGET} "${CMAKE_CURRENT_SOURCE_DIR}/file_to_cpp.cmake"
            VERBATIM
    )
endfunction()

function(embed_plain_file VARIABLE_NAME INPUT_FILE OUTPUT_HEADER)
    # On calcule le chemin complet pour être sûr
    set(INPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${INPUT_FILE}")

    add_custom_command(
            OUTPUT ${OUTPUT_HEADER}
            COMMAND ${CMAKE_COMMAND}
            -DINPUT_FILE=${INPUT_PATH}
            -DOUTPUT_FILE=${OUTPUT_HEADER}
            -DVARIABLE_NAME=${VARIABLE_NAME}
            -P "${CMAKE_CURRENT_SOURCE_DIR}/file_to_cpp.cmake"
            DEPENDS "${INPUT_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/file_to_cpp.cmake"
            VERBATIM
    )
endfunction()

embed_file(stegcli data_steg_exe steg "${GEN_DIR}/steg_exe.h")
embed_file(stegcli data_lsp_exe steglsp "${GEN_DIR}/steglsp_exe.h")

embed_plain_file(mainsteg "sample_projects/main.steg" "${GEN_DIR}/mainsteg.h")
embed_plain_file(stegmd "sample_projects/steg.md" "${GEN_DIR}/stegmd.h")

add_executable(stegcli
        main.cpp
        "${GEN_DIR}/steg_exe.h"
        "${GEN_DIR}/steglsp_exe.h"
        "${GEN_DIR}/mainsteg.h"
        "${GEN_DIR}/stegmd.h"
)

target_include_directories(stegcli PRIVATE ${GEN_DIR})

add_dependencies(stegcli
        steg
        steglsp)
target_link_libraries(stegcli PRIVATE stegasm_lib project_version stegcommon)