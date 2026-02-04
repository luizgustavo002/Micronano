#include "logger.h"
#include "huffman.h"
#include "test_helpers.h"
#include "types_errors.h"

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unity.h>
#include <stdint.h>

const char *input_test_file_txt = "temporary_input_test_file.txt";
const char *output_test_file_txt = "temporary_output_test_file.txt";

const unsigned char test_input_1[] = "AAAAACCCCEEEGGH";
const unsigned char test_input_empty[] = "";

void setUp()
{
    remove(input_test_file_txt);
    remove(output_test_file_txt);
}

void tearDown()
{
    remove(input_test_file_txt);
    remove(output_test_file_txt);
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
        
        uint64_t prev_freq = get_node_frequency(prev_node);
        uint64_t curr_freq = get_node_frequency(curr_node);
        TEST_ASSERT_GREATER_OR_EQUAL_INT(prev_freq, curr_freq);
        prev_node = curr_node;
        curr_node = get_next_node(curr_node);
    }
}

void test_create_frequency_list_1()
{
    logger_init("tests.log", 1);
    log_message(LOG_INFO, "Running create_frequency_list_1 test");

    Status status;
    status = create_temp_file(input_test_file_txt, test_input_1, sizeof(test_input_1) - sizeof(unsigned char));
    ASSERT_STATUS_OK(status);

    Huffman_Encoder *encoder;
    status = create_huffman_encoder(&encoder, input_test_file_txt, output_test_file_txt);
    ASSERT_STATUS_OK(status);

    status = create_frequency_list(encoder);
    ASSERT_STATUS_OK(status);
    assert_list_order(encoder);

    free_huffman_encoder(&encoder);
}

void test_create_frequency_list_empty()
{
    logger_init("tests.log", 1);
    log_message(LOG_INFO, "Running create_frequency_list_empty test");

    Status status;
    status = create_temp_file(input_test_file_txt, test_input_empty, sizeof(test_input_empty) - sizeof(unsigned char));
    ASSERT_STATUS_OK(status);

    Huffman_Encoder *encoder;
    status = create_huffman_encoder(&encoder, input_test_file_txt, output_test_file_txt);
    ASSERT_STATUS_OK(status);

    status = create_frequency_list(encoder);
    ASSERT_STATUS_OK(status);
    assert_list_order(encoder);
    
    free_huffman_encoder(&encoder);
}

void validate_huffman_tree(Huffman_Node *node)
{
    if (node == NULL)
        return;
    
    Huffman_Node *left_node = get_left_node(node);
    Huffman_Node *right_node = get_right_node(node);
    unsigned char symbol_node = get_symbol_node(node);

    if (left_node == NULL && right_node == NULL)
    {
        TEST_ASSERT(symbol_node != 0);
        return;
    }

    uint64_t current_frequency = get_node_frequency(node);
    uint64_t left_frequency = get_node_frequency(left_node);
    uint64_t right_frequency = get_node_frequency(right_node);
    TEST_ASSERT_EQUAL_INT(current_frequency, left_frequency + right_frequency);

    TEST_ASSERT(left_node != NULL && right_node != NULL);
    validate_huffman_tree(left_node);
    validate_huffman_tree(right_node);
}

void test_build_huffman_tree_1()
{
    logger_init("tests.log", 1);
    log_message(LOG_INFO, "Running build_huffman_tree_1 test");

    Status status;
    status = create_temp_file(input_test_file_txt, test_input_1, sizeof(test_input_1) - sizeof(unsigned char));
    ASSERT_STATUS_OK(status);

    Huffman_Encoder *encoder;
    status = create_huffman_encoder(&encoder, input_test_file_txt, output_test_file_txt);
    ASSERT_STATUS_OK(status);

    status = build_huffman_tree(encoder);
    ASSERT_STATUS_OK(status);

    validate_huffman_tree(get_first_node(encoder));

    free_huffman_encoder(&encoder);
}

void test_build_huffman_tree_empty()
{
    logger_init("tests.log", 1);
    log_message(LOG_INFO, "Running build_huffman_tree_empty test");

    Status status;
    status = create_temp_file(input_test_file_txt, test_input_empty, sizeof(test_input_empty) - sizeof(unsigned char));
    ASSERT_STATUS_OK(status);

    Huffman_Encoder *encoder;
    status = create_huffman_encoder(&encoder, input_test_file_txt, output_test_file_txt);
    ASSERT_STATUS_OK(status);

    status = build_huffman_tree(encoder);
    ASSERT_STATUS_OK(status);

    validate_huffman_tree(get_first_node(encoder));

    free_huffman_encoder(&encoder);
}

void validate_huffman_codes(char **huffman_codes)
{
    if (huffman_codes == NULL || huffman_codes[0] == NULL)
        return;

    for (char **ptr = huffman_codes; ptr < huffman_codes + 256; ptr++)
    {
        char *current_code = *ptr;
        if (*current_code != '\0')
            for (char *c = current_code; *c; c++)
            {
                TEST_ASSERT(*c == '0' || *c == '1');
            }
            
    } 
}

void test_generate_codes_1()
{
    logger_init("tests.log", 1);
    log_message(LOG_INFO, "Running generate_codes_1 test");

    Status status;
    status = create_temp_file(input_test_file_txt, test_input_1, sizeof(test_input_1) - sizeof(unsigned char));
    ASSERT_STATUS_OK(status);

    Huffman_Encoder *encoder;
    status = create_huffman_encoder(&encoder, input_test_file_txt, output_test_file_txt);
    ASSERT_STATUS_OK(status);
    
    status = generate_codes(encoder, NULL, "");
    ASSERT_STATUS_OK(status);
    validate_huffman_codes(get_huffman_codes(encoder));
    
    reset_encoder_for_next_file(encoder, input_test_file_txt);

    status = generate_codes(encoder, NULL, "");
    ASSERT_STATUS_OK(status);
    validate_huffman_codes(get_huffman_codes(encoder));

    free_huffman_encoder(&encoder);
}

void test_generate_codes_empty()
{
    logger_init("tests.log", 1);
    log_message(LOG_INFO, "Running generate_codes_empty test");

    Status status;
    status = create_temp_file(input_test_file_txt, test_input_empty, sizeof(test_input_empty) - sizeof(unsigned char));
    ASSERT_STATUS_OK(status);

    Huffman_Encoder *encoder;
    status = create_huffman_encoder(&encoder, input_test_file_txt, output_test_file_txt);
    ASSERT_STATUS_OK(status);

    status = generate_codes(encoder, NULL, "");
    ASSERT_STATUS_OK(status);
    validate_huffman_codes(get_huffman_codes(encoder));

    reset_encoder_for_next_file(encoder, input_test_file_txt);

    status = generate_codes(encoder, NULL, "");
    ASSERT_STATUS_OK(status);
    validate_huffman_codes(get_huffman_codes(encoder));
    
    free_huffman_encoder(&encoder);
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_create_frequency_list_1);
    RUN_TEST(test_create_frequency_list_empty);
    RUN_TEST(test_build_huffman_tree_1);
    RUN_TEST(test_build_huffman_tree_empty);
    RUN_TEST(test_generate_codes_1);
    RUN_TEST(test_generate_codes_empty);
    return UNITY_END();
}
