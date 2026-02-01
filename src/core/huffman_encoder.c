#include "huffman.h"
#include "huffman_internal.h"
#include "file_manager.h"
#include "types_errors.h"
#include "logger.h"
#include "bytes_manager.h"

#include <stdlib.h>
#include <stdio.h>

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

static Status count_char_frequency(Huffman_Encoder *encoder)
{
    log_message(LOG_DEBUG, "Counting character frequencies");
    Bytes_Reader *reader = encoder->reader;
    unsigned char letter;

    Status status = STATUS_OK;
    while (status == STATUS_OK)
    {
        status = read_byte_from_file(&letter, reader);
        if (status != STATUS_OK && status != ERROR_END_OF_FILE)
            return status;
        if (status == ERROR_END_OF_FILE)
            break;
        encoder->char_frequency[letter]++;
    }
    return STATUS_OK;
}

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
    for (unsigned int i = 0; i < encoder->huffman_tree->size + 1; i++)
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
    log_message(LOG_DEBUG, "Creating frequency list.");
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

Status new_huffman_node(Huffman_Node **node, Huffman_Node *left_node, Huffman_Node *right_node){

    Huffman_Node *new_node = (Huffman_Node*) calloc(1, sizeof(Huffman_Node));
    if (new_node == NULL){
        log_message(LOG_ERROR, "Failed to allocate memory for Huffman_Node.");
        *node = NULL;
        return ERROR_MEMORY_ALLOCATION;
    }

    new_node->symbol = (unsigned char) 0;

    if (left_node != NULL && right_node != NULL)
        new_node->frequency = left_node->frequency + right_node->frequency;
    else
        new_node->frequency = 0;
    
    new_node->right = right_node;
    new_node->left = left_node;
    new_node->next = NULL;
    *node = new_node;
    return STATUS_OK;
}

Status build_huffman_tree(Huffman_Encoder *encoder)
{
    Status status;
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

Huffman_Node* get_first_node(Huffman_Encoder *encoder)
{
    return encoder->huffman_tree->first_node;
}

Huffman_Node* get_next_node(Huffman_Node *node)
{
    return node->next;
}

unsigned int get_node_frequency(Huffman_Node *node)
{
    return node->frequency;
}

void free_huffman_encoder(Huffman_Encoder **encoder)
{
    log_message(LOG_DEBUG, "Freeing Huffman_Encoder.");
    free_bytes_writer(&(*encoder)->writer);
    free_bytes_reader(&(*encoder)->reader);
    free_huffman_tree(&(*encoder)->huffman_tree);
    free((*encoder));
    *encoder = NULL;
}
