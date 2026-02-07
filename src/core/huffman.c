#include "huffman.h"
#include "huffman_internal.h"
#include "bytes_manager.h"
#include "file_manager.h"
#include "types_errors.h"
#include "logger.h"

#include <stdio.h>
#include <stdlib.h>

Status new_huffman_node(Huffman_Node **node, Huffman_Node *left_node, Huffman_Node *right_node)
{

    Huffman_Node *new_node = (Huffman_Node *)calloc(1, sizeof(Huffman_Node));
    if (new_node == NULL)
    {
        log_message(LOG_ERROR, "Failed to allocate memory for Huffman_Node.");
        *node = NULL;
        return ERROR_MEMORY_ALLOCATION;
    }

    new_node->symbol = (unsigned char)0;

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
    *node = NULL;
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
        if ((*tree)->first_node->left != NULL || (*tree)->first_node->right != NULL)
            free_huffman_node(&(*tree)->first_node);
        else
            free_frequency_list(&(*tree)->first_node);
    }

    free((*tree));
    *tree = NULL;
}
