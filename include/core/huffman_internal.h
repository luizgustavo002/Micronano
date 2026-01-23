#ifndef HUFFMAN_INTERNAL_H
#define HUFFMAN_INTERNAL_H
#include "huffman.h"
#include "bytes_manager.h"

#define ASCII_SIZE 256

struct Huffman_Encoder
{
    Bytes_Writer *writer;
    Bytes_Reader *reader;
    unsigned int char_frequency[ASCII_SIZE];
    Frequency_List *frequency_list;
};

struct Huffman_Decoder
{
    Bytes_Writer *writer;
    Bytes_Reader *reader;
};


struct Frequency_List
{
    Frequency_Node *first_node;
    int size;
};

struct Frequency_Node
{
    unsigned char symbol;
    int frequency;
    Frequency_Node *next;
};

#endif