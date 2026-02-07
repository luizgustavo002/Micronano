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
    
    status = init_file_list(&(*encoder)->file_list, input_path);
    ASSERT_STATUS_OK(status);
    
    FILE *output_file;
    char full_output_path[8192];
    set_compress_output_file_path(full_output_path, input_path, output_path, (*encoder)->file_list);
    status = open_file(&output_file, full_output_path, "wb");
    ASSERT_STATUS_OK(status);
    status = create_bytes_writer(&(*encoder)->writer, output_file);
    ASSERT_STATUS_OK(status);

    status = scan_directory((*encoder)->file_list, input_path);
    ASSERT_STATUS_OK(status);

    return STATUS_OK;
}

//--------------- Char frequency ---------------
static Status count_char_frequency(Huffman_Encoder *encoder)
{
    log_message(LOG_DEBUG, "Counting character frequencies.");
    Bytes_Reader *reader = encoder->reader;

    Status status = STATUS_OK;
    unsigned char letter;

    // while (fread(&letter, sizeof(letter), 1, get_file_reader(reader)) > 0);
    // {
    //     encoder->char_frequency[letter]++;
    // }

    while (status == STATUS_OK)
    {
        status = read_byte_from_file(&letter, reader);
        if (status != STATUS_OK && status != ERROR_END_OF_FILE)
            return status;
        if (status == ERROR_END_OF_FILE)
            break;
        encoder->char_frequency[letter]++;
    }

    fseek(get_file_reader(reader), 0, SEEK_SET);

    return STATUS_OK;
}

//--------------- Frequency list ---------------
static void insert_sorted_node(Huffman_Node *new_node, Huffman_Encoder *encoder)
{
    if (encoder->huffman_tree->first_node == NULL)
    {
        encoder->huffman_tree->first_node = new_node;
        encoder->huffman_tree->size++;
        return;
    }

    Huffman_Node *next_node = encoder->huffman_tree->first_node;
    Huffman_Node *prev_node = NULL;
    for (uint16_t i = 0; i < encoder->huffman_tree->size + 1; i++)
    {
        if (next_node->next == NULL && prev_node)
        {
            if (new_node->frequency >= next_node->frequency)
                next_node->next = new_node;
            else
            {
                prev_node->next = new_node;
                new_node->next = next_node;
            }
            break;
        }

        if (new_node->frequency <= next_node->frequency)
        {
            if (i == 0 || encoder->huffman_tree->first_node->next == NULL)
            {
                new_node->next = next_node;
                encoder->huffman_tree->first_node = new_node;
                break;
            }
            prev_node->next = new_node;
            new_node->next = next_node;
            break;
        }

        if (encoder->huffman_tree->first_node->next == NULL)
        {
            encoder->huffman_tree->first_node->next = new_node;
            break;
        }
        prev_node = next_node;
        next_node = next_node->next;
    }

    encoder->huffman_tree->size++;
}

Status create_frequency_list(Huffman_Encoder *encoder)
{
    log_message(LOG_DEBUG, "Creating Frequency List.");
    if (encoder->huffman_tree == NULL)
        encoder->huffman_tree = (Huffman_Tree *)calloc(1, sizeof(Huffman_Tree));

    if (encoder->huffman_tree == NULL)
    {
        log_message(LOG_ERROR, "Failed to allocate memory for Huffman_Tree.");
        return ERROR_MEMORY_ALLOCATION;
    }

    count_char_frequency(encoder);
    for (int i = 0; i < ASCII_SIZE; i++)
    {
        // Verifica se a letra aparece
        if (encoder->char_frequency[i] == 0)
            continue;

        // Cria o no
        Huffman_Node *new_node = (Huffman_Node *)calloc(1, sizeof(Huffman_Node));
        if (new_node == NULL)
        {
            log_message(LOG_ERROR, "Failed to allocate memory for Huffman_Node.");
            return ERROR_MEMORY_ALLOCATION;
        }

        new_node->symbol = (unsigned char)i;
        new_node->frequency = encoder->char_frequency[i];
        insert_sorted_node(new_node, encoder);
    }
    return STATUS_OK;
}

//--------------- Huffman tree ---------------
Status build_huffman_tree(Huffman_Encoder *encoder)
{
    log_message(LOG_DEBUG, "Building Huffman tree.");
    Status status;

    if (encoder->huffman_tree == NULL || encoder->huffman_tree->first_node == NULL)
    {
        status = create_frequency_list(encoder);
        ASSERT_STATUS_OK(status);
    }

    while (encoder->huffman_tree->size >= 2)
    {
        Huffman_Node *left_child = encoder->huffman_tree->first_node;
        Huffman_Node *right_child = encoder->huffman_tree->first_node->next;
        Huffman_Node *next_child = encoder->huffman_tree->first_node->next->next;

        Huffman_Node *new_node;
        status = new_huffman_node(&new_node, left_child, right_child);
        if (status)
        {
            return status;
        }
        encoder->huffman_tree->first_node = next_child;
        encoder->huffman_tree->size -= 2;

        insert_sorted_node(new_node, encoder);
    }
    encoder->huffman_tree->height = get_height_tree(encoder->huffman_tree->first_node);

    return STATUS_OK;
}

//--------------- Huffman codes ---------------
static Status allocate_huffman_codes(char **huffman_codes, int height)
{
    for (int i = 0; i < ASCII_SIZE; i++)
    {
        (huffman_codes)[i] = (char *)calloc(height + 1, sizeof(char));
        if (!(huffman_codes)[i])
        {
            log_message(LOG_ERROR, "Failed to allocate memory for huffman_codes.");
            return ERROR_MEMORY_ALLOCATION;
        }
    }
    return STATUS_OK;
}

Status generate_codes(Huffman_Encoder *encoder, Huffman_Node *node, char *current_path)
{
    if (encoder->huffman_tree == NULL || encoder->huffman_tree->first_node == NULL)
        build_huffman_tree(encoder);

    Huffman_Node *first_node = encoder->huffman_tree->first_node;
    if (first_node == NULL)
        return STATUS_OK;
    if (node == NULL)
    {
        log_message(LOG_DEBUG, "Generating Huffman codes.");
        generate_codes(encoder, first_node, "");
    }

    uint8_t height = encoder->huffman_tree->height;

    Status status = STATUS_OK;
    if (!encoder->huffman_codes[0])
    {
        status = allocate_huffman_codes(encoder->huffman_codes, height);
        ASSERT_STATUS_OK(status);
    }

    if (node == NULL)
        return STATUS_OK;

    char left_current_path[height + 1], right_current_path[height + 1];

    if (node->left == NULL && node->right == NULL)
    {
        strcpy(encoder->huffman_codes[node->symbol], current_path);
        return STATUS_OK;
    }
    strcpy(left_current_path, current_path);
    strcpy(right_current_path, current_path);

    strcat(left_current_path, "0");
    strcat(right_current_path, "1");

    status = generate_codes(encoder, node->left, left_current_path);
    ASSERT_STATUS_OK(status);

    status = generate_codes(encoder, node->right, right_current_path);
    ASSERT_STATUS_OK(status);

    return STATUS_OK;
}

//--------------- Free memory ---------------
void free_huffman_codes(char **huffman_codes)
{
    if (!(*huffman_codes))
        return;

    for (char **p = huffman_codes; p < huffman_codes + 256; p++)
    {
        free(*p);
        *p = NULL;
    }
}

void free_huffman_encoder(Huffman_Encoder **encoder)
{
    log_message(LOG_DEBUG, "Freeing Huffman_Encoder.");
    free_bytes_writer(&(*encoder)->writer);
    free_bytes_reader(&(*encoder)->reader);
    free_file_header(&(*encoder)->file_header);
    free_file_list(&(*encoder)->file_list);
    free_huffman_tree(&(*encoder)->huffman_tree);
    free_huffman_codes((*encoder)->huffman_codes);

    free((*encoder));
    *encoder = NULL;
}

//--------------- Write to file  ---------------
Status write_huffman_tree(Huffman_Encoder *encoder, Huffman_Node *node)
{
    Status status = STATUS_OK;
    if (node->left == NULL && node->right == NULL)
    {
        status = write_multiple_bits_to_file(1, 1, encoder->writer);
        ASSERT_STATUS_OK(status);
        status = write_multiple_bits_to_file(node->symbol, 8, encoder->writer);
        ASSERT_STATUS_OK(status);
        return STATUS_OK;
    }

    write_multiple_bits_to_file(0, 1, encoder->writer);
    write_huffman_tree(encoder, node->left);
    write_huffman_tree(encoder, node->right);

    return STATUS_OK;
}

Status write_encoded_data(Huffman_Encoder *encoder)
{
    Bytes_Reader *reader = encoder->reader;
    Bytes_Writer *writer = encoder->writer;
    Status status = STATUS_OK;
    unsigned char letter;
    char *code_string;

    while (status == STATUS_OK)
    {
        status = read_byte_from_file(&letter, reader);
        if (status != STATUS_OK && status != ERROR_END_OF_FILE)
            return status;
        if (status == ERROR_END_OF_FILE)
            break;

        code_string = encoder->huffman_codes[letter];
        if (code_string == NULL)
        {
            log_message(LOG_ERROR, "Code not found for byte %d", letter);
            return ERROR_GENERIC;
        }

        for (char *p = code_string; *p; p++)
        {
            if (*p == '1')
            {
                status = write_bit_to_file(1, writer);
                ASSERT_STATUS_OK(status);
            }
            else
            {
                status = write_bit_to_file(0, writer);
                ASSERT_STATUS_OK(status);
            }
        }
    }

    return STATUS_OK;
}

//--------------- Compress file ---------------
static Status setup_compression_context(Huffman_Encoder *encoder, const char *new_input_path)
{
    Status status;

    free_bytes_reader(&encoder->reader);
    for (uint64_t *p = encoder->char_frequency; p < encoder->char_frequency + ASCII_SIZE; p++)
    {
        *p = 0;
    }
    free_huffman_tree(&encoder->huffman_tree);
    free_huffman_codes(encoder->huffman_codes);

    FILE *input_file;
    status = open_file(&input_file, new_input_path, "rb");
    ASSERT_STATUS_OK(status);
    status = create_bytes_reader(&encoder->reader, input_file);
    ASSERT_STATUS_OK(status);

    free_file_header(&encoder->file_header);
    char *base_path = get_base_path(encoder->file_list);
    status = create_file_header(&encoder->file_header, new_input_path, base_path);
    ASSERT_STATUS_OK(status);

    return STATUS_OK;
}

Status compress_file_simple(Huffman_Encoder *encoder)
{
    Status status = STATUS_OK;

    char *absolute_path;
    status = get_next_absolute_path(&absolute_path, encoder->file_list);
    ASSERT_STATUS_OK(status);
    if (absolute_path == NULL)
        return STATUS_OK;

    status = setup_compression_context(encoder, absolute_path);
    ASSERT_STATUS_OK(status);
    free(absolute_path);

    if (get_is_directory(encoder->file_header))
    {
        status = write_file_header(encoder->file_header, encoder->writer);
        ASSERT_STATUS_OK(status);
        write_padding(get_writer_encoder(encoder));
        compress_file_simple(encoder);
        return STATUS_OK;
    }

    status = generate_codes(encoder, NULL, "");
    ASSERT_STATUS_OK(status);
    
    status = write_file_header(encoder->file_header, encoder->writer);
    ASSERT_STATUS_OK(status);

    status = write_huffman_tree(encoder, encoder->huffman_tree->first_node);
    ASSERT_STATUS_OK(status);

    status = write_encoded_data(encoder);
    ASSERT_STATUS_OK(status);

    write_padding(get_writer_encoder(encoder));

    status = compress_file_simple(encoder);
    ASSERT_STATUS_OK(status);


    return STATUS_OK;
}

//--------------- Get data ---------------
Huffman_Node *get_first_node(Huffman_Encoder *encoder)
{
    return encoder->huffman_tree->first_node;
}

Huffman_Node *get_next_node(Huffman_Node *node)
{
    return node->next;
}

uint64_t get_node_frequency(Huffman_Node *node)
{
    return node->frequency;
}

Huffman_Node *get_left_node(Huffman_Node *node)
{
    return node->left;
}

Huffman_Node *get_right_node(Huffman_Node *node)
{
    return node->right;
}

unsigned char get_symbol_node(Huffman_Node *node)
{
    return node->symbol;
}

char **get_huffman_codes(Huffman_Encoder *encoder)
{
    return encoder->huffman_codes;
}

Bytes_Writer *get_writer_encoder(Huffman_Encoder *encoder)
{
    return encoder->writer;
}
