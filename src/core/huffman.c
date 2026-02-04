#include "huffman.h"
#include "huffman_internal.h"
#include "bytes_manager.h"
#include "file_manager.h"
#include "types_errors.h"
#include "logger.h"

#include <stdio.h>
#include <stdlib.h>

Status create_huffman_decoder(Huffman_Decoder **decoder, const char *input_path, const char *output_path)
{
    log_message(LOG_DEBUG, "Creating Huffman_Decoder.");
    *decoder = (Huffman_Decoder *)calloc(1, sizeof(Huffman_Decoder));
    if (!(*decoder))
    {
        log_message(LOG_ERROR, "Failed to allocate memory for Huffman_Decoder.");
        return ERROR_MEMORY_ALLOCATION;
    }

    Status status;
    FILE *input_file;
    status = open_file(&input_file, input_path, "rb");
    ASSERT_STATUS_OK(status);
    status = create_bytes_reader(&(*decoder)->reader, input_file);
    ASSERT_STATUS_OK(status);

    FILE *output_file;
    status = open_file(&output_file, output_path, "wb");
    ASSERT_STATUS_OK(status);
    status = create_bytes_writer(&(*decoder)->writer, input_file);
    ASSERT_STATUS_OK(status);

    return STATUS_OK;
}

uint8_t get_height_tree(Huffman_Node *root)
{
    int left = 0, right = 0;

    if (!root)
        return 0;

    if (root->left)
        left = get_height_tree(root->left) + 1;
    if (root->right)
        right = get_height_tree(root->right) + 1;

    if (left > right)
        return left;

    return right;
}

static void free_huffman_node(Huffman_Node **node)
{
    if (*node == NULL || node == NULL)
        return;

    free_huffman_node(&(*node)->left);
    free_huffman_node(&(*node)->right);
    free(*node);
}

void free_frequency_list(Huffman_Node **node)
{
    Huffman_Node *curret = *node;
    while (curret != NULL)
    {
        Huffman_Node *temp = curret;
        curret = curret->next;
        free(temp);
    }
    *node = NULL;
}

void free_huffman_tree(Huffman_Tree **tree)
{
    log_message(LOG_DEBUG, "Freeing Huffman_Tree.");
    if (*tree == NULL)
        return;

    if ((*tree)->first_node)
    {
        if ((*tree)->height != 0)
            free_huffman_node(&(*tree)->first_node);
        else
            free_frequency_list(&(*tree)->first_node);
    }

    free((*tree));
    *tree = NULL;
}
