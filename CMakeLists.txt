cmake_minimum_required(VERSION 3.10)
project(Micronano C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
enable_testing()

include(FetchContent)
FetchContent_Declare(
    unity
    GIT_REPOSITORY  https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG         v2.6.1
)
FetchContent_MakeAvailable(unity)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)

add_library(core_lib STATIC
    src/core/bytes_manager.c
    src/core/logger.c
    src/core/file_manager.c
    src/core/huffman_encoder.c
    src/core/huffman.c
)
target_include_directories(core_lib PUBLIC include/core)

add_executable(Micronano src/core/main.c)

target_link_libraries(Micronano PRIVATE core_lib)

if (BUILD_TESTING)
    add_library(test_helpers STATIC tests/test_helpers.c)
    target_include_directories(test_helpers PUBLIC tests include/core)
    target_link_libraries(test_helpers PRIVATE core_lib)

    add_executable(test_bytes_manager tests/test_bytes_manager.c)
    add_executable(test_logger tests/test_logger.c)
    add_executable(test_huffman tests/test_huffman_encoder.c)

    target_link_libraries(test_bytes_manager PRIVATE core_lib unity test_helpers)
    target_link_libraries(test_logger PRIVATE core_lib unity test_helpers)
    target_link_libraries(test_huffman PRIVATE core_lib unity test_helpers)

    add_test(NAME bytes_manage_test COMMAND test_bytes_manager)
    add_test(NAME logger_test COMMAND test_logger)
    add_test(NAME huffman_test COMMAND test_huffman)
endif()