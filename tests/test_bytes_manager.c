#include "bytes_manager.h"
#include "types_errors.h"
#include "logger.h"
#include "test_helpers.h"

#include <stdio.h>
#include <stdlib.h>
#include <unity.h>
#include <stdint.h>

const char *test_file = "temporary_test_file.bin";
unsigned char bytes[] = {'A', 'B', 'C'};
int size_bytes = 3;
uint64_t bits = 0xAF;
uint8_t size_bits = 8;

void setUp()
{
    remove(test_file);
}

void tearDown()
{
    remove(test_file);
}

void test_write_and_read()
{
    logger_init("tests.log", 1);
    log_message(LOG_INFO, "Running bytes_manage test");

    Status status;
    Bytes_Writer *writer = NULL;

    status = create_bytes_writer(&writer, test_file);
    ASSERT_STATUS_OK_TEST(status);

    status = write_multiple_bits_to_file(bits, size_bits, writer);
    ASSERT_STATUS_OK_TEST(status);
    status = write_multiple_bytes_to_file(bytes, size_bytes, writer);
    ASSERT_STATUS_OK_TEST(status);
    status = write_padding(writer);
    ASSERT_STATUS_OK_TEST(status);
    free_bytes_writer(&writer);
    //-----------------------------------------------------------------------------
    Bytes_Reader *reader = NULL;
    status = create_bytes_reader(&reader, test_file);
    ASSERT_STATUS_OK_TEST(status);

    uint64_t bits_read = 0;
    status = read_multiple_bits_from_file(&bits_read, size_bits, reader);
    ASSERT_STATUS_OK_TEST(status);
    TEST_ASSERT_EQUAL_INT64(bits, bits_read);

    unsigned char bytes_read[size_bytes];
    status = read_multiple_bytes_from_file(bytes_read, size_bytes, reader);
    ASSERT_STATUS_OK_TEST(status);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(bytes, bytes_read, size_bytes);
    free_bytes_reader(&reader);

    logger_close();
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_write_and_read);
    return UNITY_END();
}