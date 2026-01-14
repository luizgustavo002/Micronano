#include "bytes_manage.h"
#include "types_errors.h"

#include <stdio.h>
#include <stdlib.h>
#include <unity.h>

const char *test_file = "temporary_test_file.bin";
unsigned char bytes[] = {'A', 'B', 'C'};
int size_bytes = 3;
int bits = 0xAF;
int size_bits = 8;

void setUp(){
    remove(test_file);
}

void tearDown(){
    remove(test_file);
}

#define ASSERT_STATUS_OK(status) do {                                       \
        if (status){                                                        \
        TEST_ASSERT_EQUAL_INT(STATUS_OK, status);                           \
        TEST_FAIL_MESSAGE("The function failed; please check the log.");    \
        } } while (0)

void test_write_and_read(){
    Status status;
    Bytes_Writer *writer = NULL;

    FILE *file = fopen(test_file, "wb");
    status = make_struct_bytes_write(&writer, file);
    ASSERT_STATUS_OK(status);
    
    status = write_multiple_bits_to_file(bits, size_bits, writer);
    ASSERT_STATUS_OK(status);
    status = write_multiple_bytes_to_file(bytes, size_bytes, writer);
    ASSERT_STATUS_OK(status);
    status = write_trash(writer);
    ASSERT_STATUS_OK(status);
    fclose(file);
    //-----------------------------------------------------------------------------
    file = fopen(test_file, "rb");
    Bytes_Reader *reader = NULL;
    status = make_struct_bytes_reader(&reader, file);
    ASSERT_STATUS_OK(status);

    int bits_read = 0;
    status = read_multiple_bits_from_file(&bits_read, size_bits, reader);
    ASSERT_STATUS_OK(status);
    TEST_ASSERT_EQUAL_INT(bits, bits_read);

    unsigned char bytes_read[size_bytes];
    status = read_multiple_bytes_from_file(bytes_read, size_bytes, reader);
    ASSERT_STATUS_OK(status);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(bytes, bytes_read, size_bytes);
}

int main(){
    UNITY_BEGIN();
    RUN_TEST(test_write_and_read);
    UNITY_END();
}