#include "file_manager.h"
#include "huffman.h"
#include "logger.h"
#include "test_helpers.h"
#include "types_errors.h"
#include "huffman_api.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unity.h>

const char *input_directory_name = "dir_input";
char input_full_path[4096];
char output_compresed_file_directory[4096];

char input_decompresed_file[8192];
const char *output_directory_name = "dir_output";
char output_full_path[4096];

const char *names_file[] = {"dir_input/Arquivo_1", "dir_input/Arquivo_2"};
const char *datas_file[] = {"AAAAACCCCEEEGGH", "GGGGGHHHJJJ"};
const int file_count = 2;
char full_path[10][4096] = {0};

const unsigned char test_input_1[] = "AAAAACCCCEEEGGH";
const unsigned char test_input_empty[] = "";

void setUp() {}

void tearDown() {}

void test_compress_file_simple_1() {
    logger_init("tests.log", 1);
    log_message(LOG_INFO, "Running generate_codes_1 test");

    Status status;

    get_current_working_directory(output_compresed_file_directory);

    make_directory(input_directory_name, input_full_path);
    status = populate_directory((unsigned char **)datas_file, (char **)names_file, file_count, full_path + 1);
    ASSERT_STATUS_OK_TEST(status);

    Huffman_Encoder *encoder;
    status = create_huffman_encoder(&encoder, full_path[1], output_compresed_file_directory);
    ASSERT_STATUS_OK_TEST(status);

    log_message(LOG_INFO, "Compressing file or directory: %s", full_path[1]);
    // status = compress_file_simple(encoder);
    ASSERT_STATUS_OK_TEST(status);

    snprintf(input_decompresed_file, sizeof(input_decompresed_file), "%s/%s%s", output_compresed_file_directory, "Arquivo_1", ".hlg");
    make_directory(output_directory_name, output_full_path);
    Huffman_Decoder *decoder;
    status = create_huffman_decoder(&decoder, input_decompresed_file, output_full_path);
    ASSERT_STATUS_OK_TEST(status);

    log_message(LOG_INFO, "Decompressing file or directory: %s", input_decompresed_file);
    // status = decompressed_file_simple(decoder);
    ASSERT_STATUS_OK_TEST(status);

    free_huffman_encoder(&encoder);
    free_huffman_decoder(&decoder);

    remove_directory(input_full_path);
    remove_directory(output_directory_name);
}

void test_compress_directory_simple_1() {
    logger_init("tests.log", 1);
    log_message(LOG_INFO, "Running generate_codes_1 test");

    Status status;

    get_current_working_directory(output_compresed_file_directory);

    make_directory(input_directory_name, input_full_path);
    status = populate_directory((unsigned char **)datas_file, (char **)names_file, file_count, full_path + 1);
    ASSERT_STATUS_OK_TEST(status);

    status = compress(input_full_path, output_compresed_file_directory, PROCESSING_SETTING_NON_SOLID_LOG_DEBUG);  
    ASSERT_STATUS_OK_TEST(status);

    //------------------------------------
    snprintf(input_decompresed_file, sizeof(input_decompresed_file), "%s/%s%s", output_compresed_file_directory, input_directory_name, ".hlg");
    make_directory(output_directory_name, output_full_path);
    Huffman_Decoder *decoder;
    status = create_huffman_decoder(&decoder, input_decompresed_file, output_full_path);
    ASSERT_STATUS_OK_TEST(status);

    log_message(LOG_INFO, "Decompressing file or directory: %s", input_decompresed_file);
    status = decompress(input_decompresed_file, output_full_path, PROCESSING_SETTING_NON_SOLID_LOG_DEBUG);
    ASSERT_STATUS_OK_TEST(status);

    free_huffman_decoder(&decoder);

    remove_directory(input_full_path);
    remove_directory(output_directory_name);
}

void test() {
    Status status = STATUS_OK;

    int bool_compress = 0;
    printf("Digite 1 para compactar e 0 descompactar: ");
    scanf("%d", &bool_compress);

    if (bool_compress) {
        char input_path[4096] = "/home/luizg/Games/aa";
        char output_path[4096] = "/home/luizg/Games";
        // printf("Digite o arquivo que serar compactado: ");
        // // scanf(" %s", input_path);
        // printf("Digite a pasta de saida do arquivo: ");
        // // scanf(" %s", output_path);

        Huffman_Encoder *encoder;
        status = create_huffman_encoder(&encoder, input_path, output_path);
        ASSERT_STATUS_OK_TEST(status);

        // status = compress_file_simple(encoder);
        ASSERT_STATUS_OK_TEST(status);

        free_huffman_encoder(&encoder);
    } else {
        char input_path[4096] = "/home/luizg/Games/aa.hlg";
        char output_path[4096] = "/home/luizg/Games/A/";
        // printf("Digite o arquivo que serar descompactado: ");
        // scanf(" %s", input_path);
        // printf("Digite a pasta de saida do arquivo: ");
        // scanf(" %s", output_path);

        Huffman_Decoder *decoder;
        status = create_huffman_decoder(&decoder, input_path, output_path);
        ASSERT_STATUS_OK_TEST(status);

        // status = decompressed_file_simple(decoder);
        ASSERT_STATUS_OK_TEST(status);

        free_huffman_decoder(&decoder);
    }
}

int main() {
    UNITY_BEGIN();
    // RUN_TEST(test_compress_file_simple_1);
    RUN_TEST(test_compress_directory_simple_1);
    // RUN_TEST(test);
    return UNITY_END();
}