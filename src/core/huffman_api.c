#include "huffman_api.h"
#include "huffman.h"
#include "logger.h"
#include "types_errors.h"

#include <stdio.h>
#include <stdlib.h>

// --------------- Compress ---------------
static Status compress_non_solid(const char *input_path, const char *output_path, int log_debug) {
    Status status = STATUS_OK;
    logger_init("Non_solid.log", log_debug);
    log_message(LOG_INFO, "Compressing: '%s'...", input_path);

    Huffman_Encoder *encoder = NULL;
    status = create_huffman_encoder(&encoder, input_path, output_path);
    if (status != STATUS_OK) {
        logger_close();
        return status;
    }

    while (1) {
        status = compress_file_non_solid(encoder);
        if (status == STATUS_END_OF_PROCESS) break;
        if (status != STATUS_OK) {
            free_huffman_encoder(&encoder);
            logger_close();
            return status;
        }
    }

    free_huffman_encoder(&encoder);
    log_message(LOG_INFO, "All files compressed successfully.");
    logger_close();
    return STATUS_OK;
}

Status compress(const char *input_path, const char *output_path, Processing_Settings setting) {
    Status status = STATUS_OK;
    if (setting == PROCESSING_SETTING_NON_SOLID) {
        status = compress_non_solid(input_path, output_path, 0);
        ASSERT_STATUS_OK(status);
    }
    if (setting == PROCESSING_SETTING_NON_SOLID_LOG_DEBUG) {
        status = compress_non_solid(input_path, output_path, 1);
        ASSERT_STATUS_OK(status);
    }
    return STATUS_OK;
}

// --------------- Decompress ---------------
static Status decompress_non_solid(const char *input_path, const char *output_path, int log_debug) {
    Status status = STATUS_OK;
    logger_init("Non_solid.log", log_debug);
    log_message(LOG_INFO, "Decompressing: '%s'...", input_path);

    Huffman_Decoder *decoder = NULL;
    status = create_huffman_decoder(&decoder, input_path, output_path);
    if (status != STATUS_OK) {
        logger_close();
        return status;
    }

    while (1) {
        status = decompressed_file_non_solid(decoder);
        if (status == STATUS_END_OF_PROCESS) break;
        if (status != STATUS_OK) {
            free_huffman_decoder(&decoder);
            logger_close();
            return status;
        }
    }

    free_huffman_decoder(&decoder);
    log_message(LOG_INFO, "All files compressed successfully.");
    logger_close();
    return STATUS_OK;
}

Status decompress(const char *input_path, const char *output_path, Processing_Settings setting) {
    Status status = STATUS_OK;
    if (setting == PROCESSING_SETTING_NON_SOLID) {
        status = decompress_non_solid(input_path, output_path, 0);
        ASSERT_STATUS_OK(status);
    }
    if (setting == PROCESSING_SETTING_NON_SOLID_LOG_DEBUG) {
        status = decompress_non_solid(input_path, output_path, 1);
        ASSERT_STATUS_OK(status);
    }
    return STATUS_OK;
}
