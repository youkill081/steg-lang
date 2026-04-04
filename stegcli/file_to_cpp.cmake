file(READ ${INPUT_FILE} HEX_CONTENT HEX)

string(LENGTH "${HEX_CONTENT}" HEX_LEN)
math(EXPR F_SIZE "${HEX_LEN} / 2")

string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1, " CPP_CONTENT "${HEX_CONTENT}")

file(WRITE ${OUTPUT_FILE} "unsigned char ${VARIABLE_NAME}[] = { ${CPP_CONTENT} };\n")
file(APPEND ${OUTPUT_FILE} "unsigned int ${VARIABLE_NAME}_len = ${F_SIZE};\n")