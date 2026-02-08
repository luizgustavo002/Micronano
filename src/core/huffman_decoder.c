#include "huffman.h"
#include "huffman_internal.h"
#include "file_manager.h"
#include "types_errors.h"
#include "logger.h"
#include "bytes_manager.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#ifdef _WIN32
#include <dirent.h>
#define CREATE_DIR(path) _mkdir(path)
#define PATH_SEP '\\'
#else
#include <sys/stat.h>
#include <sys/types.h>
#define CREATE_DIR(path) mkdir(path, 0755)
#define PATH_SEP '/'
#endif

// Creates Huffman_Decoder instance and assigns it to provided pointer.
// - The caller is responsible for freeing it with free_huffman_decoder.
// - Returns STATUS_OK on success or a specific error code on failure.
Status create_huffman_decoder(Huffman_Decoder **decoder, const char *input_path, const char *output_path)
{
    // Validates that the argument is not NULL.
    if (decoder == NULL || input_path == NULL || output_path == NULL) {
        char msg[128] = {0};
        if (decoder == NULL) strcat(msg, "decoder ");
        if (input_path == NULL) strcat(msg, "input_path ");
        if (output_path == NULL) strcat(msg, "output_path ");
        log_message(LOG_ERROR, "create_huffman_decoder invalid args: %s", msg);
        return ERROR_INVALID_ARGUMENT;
    }

    *decoder = (Huffman_Decoder *)calloc(1, sizeof(Huffman_Decoder));
    if (*decoder == NULL)
    {
        log_message(LOG_ERROR, "Failed to allocate memory for Huffman_Decoder.");
        return ERROR_MEMORY_ALLOCATION;
    }

    Status status = STATUS_OK;

    status = create_bytes_reader(&(*decoder)->reader, input_path);
    ASSERT_STATUS_OK_AND_FREES(status, *decoder);

    size_t len = strlen(output_path) + 1;
    (*decoder)->output_path = calloc(len, sizeof(char));
    if ((*decoder)->output_path == NULL)
    {
        log_message(LOG_ERROR, "Failed to allocate memory for (*decoder)->output_path.");
        status = ERROR_MEMORY_ALLOCATION;
        free_bytes_reader(&(*decoder)->reader);
        ASSERT_STATUS_OK_AND_FREES(status, *decoder);
    }
    strcpy((*decoder)->output_path, output_path);

    return STATUS_OK;
}

Status read_huffman_tree(Huffman_Node **node, Huffman_Decoder *decoder)
{
    // Validates that the argument is not NULL.
    if (decoder == NULL) {
        log_message(LOG_ERROR, "read_huffman_tree invalid args: decoder");
        return ERROR_INVALID_ARGUMENT;
    }
    Status status = STATUS_OK;

    if (node == NULL)
    {
        if (decoder->huffman_tree == NULL)
        {
            decoder->huffman_tree = (Huffman_Tree *)calloc(1, sizeof(Huffman_Tree));
            if (decoder->huffman_tree == NULL)
            {
                log_message(LOG_ERROR, "Failed to allocate memory for Huffman_Tree.");
                return ERROR_MEMORY_ALLOCATION;
            }
        }
        read_huffman_tree(&decoder->huffman_tree->first_node, decoder);
        return STATUS_OK;
    }

    uint8_t bit = 0;

    status = read_bit_from_file(&bit, decoder->reader);
    ASSERT_STATUS_OK(status);

    if (bit == 1)
    {
        status = new_huffman_node(node, NULL, NULL);
        ASSERT_STATUS_OK(status);

        status = read_byte_from_file(&(*node)->symbol, decoder->reader);
        ASSERT_STATUS_OK(status);

        return STATUS_OK;
    }

    Huffman_Node *left_node, *right_node;
    status = read_huffman_tree(&left_node, decoder);
    ASSERT_STATUS_OK(status);
    status = read_huffman_tree(&right_node, decoder);
    ASSERT_STATUS_OK(status);

    status = new_huffman_node(node, left_node, right_node);
    ASSERT_STATUS_OK(status);

    return STATUS_OK;
}

static Status write_decompressed_data(Huffman_Decoder *decoder)
{
    // Validates that the argument is not NULL.
    if (decoder == NULL) {
        log_message(LOG_ERROR, "write_decompressed_data invalid args: decoder");
        return ERROR_INVALID_ARGUMENT;
    }
    Status status = STATUS_OK;
    uint64_t file_size = get_file_size(decoder->file_header);

    Huffman_Node *aux_node = decoder->huffman_tree->first_node;

    for (uint64_t i = 0; i < file_size; i++)
    {
        while (1)
        {
            uint8_t bit;
            status = read_bit_from_file(&bit, decoder->reader);
            ASSERT_STATUS_OK(status);
            if (bit == 0)
                aux_node = aux_node->left;
            else
                aux_node = aux_node->right;

            if (aux_node->left == NULL && aux_node->right == NULL)
            {
                status = write_byte_to_file(aux_node->symbol, decoder->writer);
                ASSERT_STATUS_OK(status);
                aux_node = decoder->huffman_tree->first_node;
                break;
            }
        }
    }

    status = write_padding(decoder->writer);
    ASSERT_STATUS_OK(status);

    return STATUS_OK;
}

static Status setup_decompressed_context(Huffman_Decoder *decoder)
{
    // Validates that the argument is not NULL.
    if (decoder == NULL) {
        log_message(LOG_ERROR, "setup_decompressed_context invalid args: decoder");
        return ERROR_INVALID_ARGUMENT;
    }
    Status status = STATUS_OK;

    free_bytes_writer(&decoder->writer);
    free_huffman_tree(&decoder->huffman_tree);
    free_file_header(&decoder->file_header);
    status = read_file_header(decoder);
    ASSERT_STATUS_OK(status);

    char relative_path[4096];
    get_relative_path(relative_path, decoder->file_header);
    log_message(LOG_DEBUG, "Decompressing file or directory: %s", relative_path);

    return STATUS_OK;
}

Status decompressed_file_non_solid(Huffman_Decoder *decoder)
{
    // Validates that the argument is not NULL.
    if (decoder == NULL) {
        log_message(LOG_ERROR, "decompressed_file_simple invalid args: decoder");
        return ERROR_INVALID_ARGUMENT;
    }
    Status status = STATUS_OK;

    status = setup_decompressed_context(decoder);
    if (status == ERROR_END_OF_FILE)
        return STATUS_END_OF_PROCESS;
    ASSERT_STATUS_OK(status);

    if (get_is_directory(decoder->file_header))
    {
        char relative_path[4096];
        char dir_name[8192];
        get_relative_path(relative_path, decoder->file_header);
        snprintf(dir_name, sizeof(dir_name), "%s%c%s", decoder->output_path, PATH_SEP, relative_path);
        if (CREATE_DIR(dir_name) != 0)
        {
            log_message(LOG_ERROR, "mkdir failed: %s (errno=%d)", strerror(errno), errno);
        }
        return STATUS_OK;
    }

    status = read_huffman_tree(NULL, decoder);
    ASSERT_STATUS_OK(status);

    status = write_decompressed_data(decoder);
    ASSERT_STATUS_OK(status);

    status = skip_padding(decoder->reader);
    ASSERT_STATUS_OK(status);

    return STATUS_OK;
}

void free_huffman_decoder(Huffman_Decoder **decoder)
{
    // Validates that the argument is not NULL.
    if (decoder == NULL) {
        log_message(LOG_ERROR, "free_huffman_decoder invalid args: decoder");
        return;
    }
    free((*decoder)->output_path);
    free_bytes_writer(&(*decoder)->writer);
    free_bytes_reader(&(*decoder)->reader);
    free_file_header(&(*decoder)->file_header);
    free_huffman_tree(&(*decoder)->huffman_tree);

    free((*decoder));
    *decoder = NULL;
}
