#include "bytes_manage.h"

#include <stdio.h>
#include <stdlib.h>
#include <unity.h>

const char *test_file = "temporary_test_file.bin";
unsigned char bytes[] = {'A', 'B', 'C'};
int size_bytes = 3;
int bits[] = {1, 0, 1, 0, 1, 1, 1, 1};
int size_bits = 8;


void setUp(){
    remove(test_file);
}

void tearDown(){
    remove(test_file);
}

void test_write_and_read(){
    FILE *file = fopen(test_file, "wb");
    Bytes_Writer *writer = make_struct_bytes_write(file);
    
    write_multiple_bits_to_file(bits, size_bits, writer);
    write_multiple_bytes_to_file(bytes, size_bytes, writer);
    write_trash(writer);
    fclose(file);
    //-----------------------------------------------------------------------------
    file = fopen(test_file, "rb");
    Bytes_Reader *reader = make_struct_bytes_reader(file);
    
    int bits_read[size_bits];
    for (int i = 0; i < size_bits; i++)
    {
        bits_read[i] = read_bit_from_file(reader);
    }
    TEST_ASSERT_EQUAL_INT_ARRAY(bits, bits_read, size_bits);

    unsigned char bytes_read[size_bytes];
    for (int i = 0; i < size_bytes; i++)
    {
        bytes_read[i] = read_byte_from_file(reader);
    }
    TEST_ASSERT_EQUAL_UINT8_ARRAY(bytes, bytes_read, size_bytes);
}

int main(){
    UNITY_BEGIN();
    RUN_TEST(test_write_and_read);
    UNITY_END();
}