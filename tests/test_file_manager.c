#include "logger.h"
#include "file_manager.h"
#include "types_errors.h"
#include "test_helpers.h"

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unity.h>
#include <stdint.h>

const char *input_test_file_path = "temporary_input_test_file.txt";
const char *output_test_file_path = "temporary_output_test_file.txt";

const uint16_t path_size_input = 29;
const uint16_t path_size_output = 30;

const unsigned char test_input_1[] = "AAAAACCCCEEEGGH";
const unsigned char test_input_empty[] = "";

const uint64_t size_test_input_1 = 15;
const uint64_t size_test_input_empty = 0;

void setUp()
{
    remove(input_test_file_path);
    remove(output_test_file_path);
}

void tearDown()
{
    remove(input_test_file_path);
    remove(output_test_file_path);
}

void validate_file_header(File_Header *file_header, const uint16_t expected_path_size, const uint64_t expected_file_size)
{
    uint16_t path_size = get_path_size(file_header);
    uint64_t file_size = get_file_size(file_header);

    TEST_ASSERT_EQUAL_INT16(expected_path_size, path_size);
    TEST_ASSERT_EQUAL_INT64(expected_file_size, file_size);
}

void test_file_header_1()
{
    logger_init("tests.log", 1);
    log_message(LOG_INFO, "Running file_header_1 test");

    Status status;
    status = create_temp_file(input_test_file_path, test_input_1, sizeof(test_input_1) - sizeof(unsigned char));
    ASSERT_STATUS_OK(status);

    File_Header *file_header;
    status = create_file_header(&file_header, input_test_file_path);

    validate_file_header(file_header, path_size_input, size_test_input_1);

    free_file_header(&file_header);
}

void test_file_header_empty()
{
    logger_init("tests.log", 1);
    log_message(LOG_INFO, "Running file_header_1 test");

    Status status;
    status = create_temp_file(input_test_file_path, test_input_empty, sizeof(test_input_empty) - sizeof(unsigned char));
    ASSERT_STATUS_OK(status);

    File_Header *file_header;
    status = create_file_header(&file_header, input_test_file_path);

    validate_file_header(file_header, path_size_input, size_test_input_empty);

    free_file_header(&file_header);
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_file_header_1);
    RUN_TEST(test_file_header_empty);
    return UNITY_END();
}