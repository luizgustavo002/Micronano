#ifndef HUFFMAN_INTERNAL_H
#define HUFFMAN_INTERNAL_H
#include "huffman.h"
#include "bytes_manager.h"
#include "file_manager.h"
#include <stdint.h>

#define ASCII_SIZE 256

struct Huffman_Encoder
{
    Bytes_Writer *writer;
    Bytes_Reader *reader;
    File_Header *file_header;
    File_List *file_list;
    uint64_t char_frequency[ASCII_SIZE];
    Huffman_Tree *huffman_tree;
    char *huffman_codes[ASCII_SIZE];
};

struct Huffman_Decoder
{
    Bytes_Writer *writer;
    Bytes_Reader *reader;
    char *output_path;
    File_Header *file_header;
    Huffman_Tree *huffman_tree;
};

struct Huffman_Tree
{
    Huffman_Node *first_node;
    uint16_t size;
    uint8_t height;
};

struct Huffman_Node
{
    unsigned char symbol;
    uint64_t frequency;
    Huffman_Node *next;
    Huffman_Node *right;
    Huffman_Node *left;
};

#endif