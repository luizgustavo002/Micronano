#include "logger.h"
#include "huffman.h"
#include "test_helpers.h"
#include "types_errors.h"
#include "file_manager.h"

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unity.h>
#include <stdint.h>

const char *input_file_name = "temporary_input_file";
const char *compressed_file_name = "temporary_compressed_file";
const char *decompressed_file_name = "temporary_decompressed_file";

const char *input_directory_name = "dir_input";
char input_full_path[4096];
const char *output_directory_name = "dir_output";
char output_full_path[4096];

const char *names_file[] = {
    "dir_input/Arquivo_1",
    "dir_input/Arquivo_2"
};
const char *datas_file[] = {
    "AAAAACCCCEEEGGH",
    "GGGGGHHHJJJ"
};
const int file_count = 2;
char full_path[10][4096] = {0};


const unsigned char test_input_1[] = "AAAAACCCCEEEGGH";
const unsigned char test_input_empty[] = "";

void setUp()
{
    remove(input_file_name);
    remove(compressed_file_name);
    remove(decompressed_file_name);
}

void tearDown()
{
    remove(input_file_name);
    remove(compressed_file_name);
    remove(decompressed_file_name);
}

void test_compress_file_simple_1()
{
    logger_init("tests.log", 0);
    log_message(LOG_INFO, "Running generate_codes_1 test");

    Status status;

    make_directory(input_directory_name, input_full_path);
    status = populate_directory((unsigned char**)datas_file, (char **)names_file, file_count, full_path + 1);
    ASSERT_STATUS_OK(status);

    Huffman_Encoder *encoder;
    status = create_huffman_encoder(&encoder, input_full_path, compressed_file_name);
    ASSERT_STATUS_OK(status);
    
    status = compress_file_simple(encoder);
    ASSERT_STATUS_OK(status);  

    make_directory(output_directory_name, output_full_path);
    Huffman_Decoder *decoder;
    status = create_huffman_decoder(&decoder, compressed_file_name, output_full_path);
    ASSERT_STATUS_OK(status);

    status = decompressed_file_simple(decoder);
    ASSERT_STATUS_OK(status);

    free_huffman_encoder(&encoder);
    free_huffman_decoder(&decoder);

    remove_directory(input_full_path);
    remove_directory(output_directory_name);
}

void test()
{
    Status status = STATUS_OK;

    int bool_compress = 0;
    printf("Digite 1 para compactar e 0 descompactar: ");
    scanf("%d", &bool_compress);

    if (bool_compress)
    {
        char input_path[4096] = "/home/luizg/Games/aa";
        char output_path[4096] = "/home/luizg/Games"; 
        // printf("Digite o arquivo que serar compactado: ");
        // // scanf(" %s", input_path);
        // printf("Digite a pasta de saida do arquivo: ");
        // // scanf(" %s", output_path);

        Huffman_Encoder *encoder;
        status = create_huffman_encoder(&encoder, input_path, output_path);
        ASSERT_STATUS_OK(status);
        
        status = compress_file_simple(encoder);
        ASSERT_STATUS_OK(status); 

        free_huffman_encoder(&encoder);
    }
    else
    {
        char input_path[4096] = "/home/luizg/Games/aa.hlg";
        char output_path[4096] = "/home/luizg/Games/A/";
        // printf("Digite o arquivo que serar descompactado: ");
        // scanf(" %s", input_path);
        // printf("Digite a pasta de saida do arquivo: ");
        // scanf(" %s", output_path);

        Huffman_Decoder *decoder;
        status = create_huffman_decoder(&decoder, input_path, output_path);
        ASSERT_STATUS_OK(status);
    
        status = decompressed_file_simple(decoder);
        ASSERT_STATUS_OK(status);
    
        free_huffman_decoder(&decoder);
    }
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_compress_file_simple_1);
    RUN_TEST(test);
    return UNITY_END();
}