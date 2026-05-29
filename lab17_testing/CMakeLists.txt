cmake_minimum_required(VERSION 3.10)
project(MatrixMultTests)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

enable_testing()

find_library(CMOCKA_LIB cmocka)
find_path(CMOCKA_INCLUDE cmocka.h)

add_executable(matrix_test tests.c original.c)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${CMOCKA_INCLUDE})
target_link_libraries(matrix_test ${CMOCKA_LIB} pthread)

add_test(NAME Multiply2x2 COMMAND matrix_test multiply_2x2)
add_test(NAME Multiply3x3 COMMAND matrix_test multiply_3x3)
add_test(NAME FilledOnes2x2 COMMAND matrix_test filled_ones_2x2)
add_test(NAME ZeroMatrix COMMAND matrix_test zero_matrix)
add_test(NAME Identity3x3 COMMAND matrix_test identity_3x3)
add_test(NAME EmulateMultithread COMMAND matrix_test emulate_multithread)