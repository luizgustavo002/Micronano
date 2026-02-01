#ifndef HUFFMAN_INTERNAL_H
#define HUFFMAN_INTERNAL_H
#include "huffman.h"
#include "bytes_manager.h"

#define ASCII_SIZE 256
#define ASSERT_STATUS_OK(status) \
    do                           \
    {                            \
        if (status)              \
        {                        \
            return status;       \
        }                        \
    } while (0)


struct Huffman_Encoder
{
    Bytes_Writer *writer;
    Bytes_Reader *reader;
    unsigned int char_frequency[ASCII_SIZE];
    Huffman_Tree *huffman_tree;
};

struct Huffman_Decoder
{
    Bytes_Writer *writer;
    Bytes_Reader *reader;
};

struct Huffman_Tree
{
    Huffman_Node *first_node;
    unsigned int size;
    unsigned int height;
};

struct Huffman_Node
{
    unsigned char symbol;
    unsigned int frequency;
    Huffman_Node *next;
    Huffman_Node *right;
    Huffman_Node *left;
};

#endif