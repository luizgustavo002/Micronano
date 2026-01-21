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
    src/core/bytes_manage.c
    src/core/logger.c
)
target_include_directories(core_lib PUBLIC include/core)

add_executable(Micronano src/core/main.c)
add_executable(test_bytes_manage tests/test_bytes_manage.c)
add_executable(test_logger tests/test_logger.c)

target_link_libraries(Micronano PRIVATE core_lib)
target_link_libraries(test_bytes_manage PRIVATE core_lib unity)
target_link_libraries(test_logger PRIVATE core_lib unity)

add_test(NAME bytes_manage_test COMMAND test_bytes_manage)
add_test(NAME logger_test COMMAND test_logger)