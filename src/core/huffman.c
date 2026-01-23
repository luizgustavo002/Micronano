#include "huffman.h"
#include "huffman_internal.h"
#include "file_manager.h"
#include "types_errors.h"
#include "logger.h"

#include <stdio.h>
#include <stdlib.h>

#define ASSERT_STATUS_OK(status) \
    do                           \
    {                            \
        if (status)              \
        {                        \
            return status;       \
        }                        \
    } while (0)

Status create_huffman_encoder(Huffman_Encoder **encoder, const char *input_path, const char *output_path)
{
    log_message(LOG_DEBUG, "Creating Huffman_Encoder.");
    *encoder = (Huffman_Encoder *)calloc(1, sizeof(Huffman_Encoder));
    if (!(*encoder))
    {
        log_message(LOG_ERROR, "Failed to allocate memory for Huffman_Encoder.");
        return ERROR_MEMORY_ALLOCATION;
    }

    Status status;

    FILE *input_file;
    status = open_file(&input_file, input_path, "rb");
    ASSERT_STATUS_OK(status);
    status = create_bytes_reader(&(*encoder)->reader, input_file);
    ASSERT_STATUS_OK(status);

    FILE *output_file;
    status = open_file(&output_file, output_path, "wb");
    ASSERT_STATUS_OK(status);
    status = create_bytes_writer(&(*encoder)->writer, output_file);
    ASSERT_STATUS_OK(status);

    return STATUS_OK;
}

Status create_huffman_decoder(Huffman_Decoder **decoder, const char *input_path, const char *output_path)
{
    log_message(LOG_DEBUG, "Creating Huffman_Decoder.");
    *decoder = (Huffman_Decoder *)calloc(1, sizeof(Huffman_Encoder));
    if (!(*decoder))
    {
        log_message(LOG_ERROR, "Failed to allocate memory for Huffman_Decoder.");
        return ERROR_MEMORY_ALLOCATION;
    }

    Status status;
    FILE *output_file;
    status = open_file(&output_file, output_path, "rb");
    ASSERT_STATUS_OK(status);
    status = create_bytes_writer(&(*decoder)->writer, output_file);
    ASSERT_STATUS_OK(status);

    FILE *input_file;
    status = open_file(&input_file, input_path, "wb");
    ASSERT_STATUS_OK(status);
    status = create_bytes_reader(&(*decoder)->reader, input_file);
    ASSERT_STATUS_OK(status);

    return STATUS_OK;
}



