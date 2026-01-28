#include "logger.h"
#include "huffman.h"
#include "test_helpers.h"
#include "types_errors.h"

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unity.h>

const char *input_test_file_txt = "temporary_input_test_file.txt";
const char *output_test_file_txt = "temporary_output_test_file.txt";

const unsigned char test_input_1[] = "AAAAACCCCEEEGGH";
const unsigned char test_input_empty[] = "";

void setUp()
{
    remove(input_test_file_txt);
}

void tearDown()
{
    remove(input_test_file_txt);
}

void assert_list_order(Huffman_Encoder *encoder)
{
    Huffman_Node *prev_node = NULL;
    Huffman_Node *curr_node = NULL;
    Huffman_Node *frist_node = get_first_node(encoder);
    if (frist_node)
    {
        curr_node = frist_node;
        prev_node = frist_node;
    }
    
    while (1)
    {
        if (!curr_node)
            break;
        
        unsigned int prev_freq = get_node_frequency(prev_node);
        unsigned int curr_freq = get_node_frequency(curr_node);
        TEST_ASSERT_GREATER_OR_EQUAL_INT(prev_freq, curr_freq);
        prev_node = curr_node;
        curr_node = get_next_node(curr_node);
    }
}

void test_create_frequency_list_1()
{
    logger_init("tests.log", 1);
    log_message(LOG_INFO, "Running huffman_encoder test");

    Status status;
    status = create_temp_file(input_test_file_txt, test_input_1, sizeof(test_input_1) - sizeof(unsigned char));
    ASSERT_STATUS_OK(status);

    Huffman_Encoder *encoder;
    status = create_huffman_encoder(&encoder, input_test_file_txt, output_test_file_txt);
    ASSERT_STATUS_OK(status);

    status = create_frequency_list(encoder);
    ASSERT_STATUS_OK(status);
    assert_list_order(encoder);
}

void test_create_frequency_list_empty()
{
    logger_init("tests.log", 1);
    log_message(LOG_INFO, "Running huffman_encoder test");

    Status status;
    status = create_temp_file(input_test_file_txt, test_input_empty, sizeof(test_input_empty) - sizeof(unsigned char));
    ASSERT_STATUS_OK(status);

    Huffman_Encoder *encoder;
    status = create_huffman_encoder(&encoder, input_test_file_txt, output_test_file_txt);
    ASSERT_STATUS_OK(status);

    status = create_frequency_list(encoder);
    ASSERT_STATUS_OK(status);
    assert_list_order(encoder);
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_create_frequency_list_1);
    RUN_TEST(test_create_frequency_list_empty);
    UNITY_END();
    return 0;
}