#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "types_errors.h"
#include "bytes_manager.h"
#include <stdio.h>

typedef struct Huffman_Encoder Huffman_Encoder;
typedef struct Huffman_Decoder Huffman_Decoder;
typedef struct Frequency_List Frequency_List;
typedef struct Huffman_Tree Huffman_Tree;
typedef struct Huffman_Node Huffman_Node;

Status create_huffman_encoder(Huffman_Encoder **encoder, const char *input_path, const char *output_path);
Status create_huffman_decoder(Huffman_Decoder **decoder, const char *input_path, const char *output_path);
void free_huffman_encoder(Huffman_Encoder **encoder);
void free_huffman_decoder(Huffman_Decoder *decoder);

Status create_frequency_list(Huffman_Encoder *encoder);
Status build_huffman_tree(Huffman_Encoder *encoder);
unsigned int get_height_tree(Huffman_Node *root);
void free_huffman_tree(Huffman_Tree **tree);

Huffman_Node* get_first_node(Huffman_Encoder *encoder);
Huffman_Node* get_next_node(Huffman_Node *node);
unsigned int get_node_frequency(Huffman_Node *node);

#endif