#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "types_errors.h"
#include "bytes_manager.h"
#include <stdio.h>
#include <stdint.h>

typedef struct Huffman_Encoder Huffman_Encoder;
typedef struct Huffman_Decoder Huffman_Decoder;
typedef struct Huffman_Tree Huffman_Tree;
typedef struct Huffman_Node Huffman_Node;

//--------------- Structs ---------------
Status create_huffman_encoder(Huffman_Encoder **encoder, const char *input_path, const char *output_path);
Status create_huffman_decoder(Huffman_Decoder **decoder, const char *input_path, const char *output_path);
void free_huffman_encoder(Huffman_Encoder **encoder);
void free_huffman_decoder(Huffman_Decoder **decoder);

//--------------- Frequency list ---------------
Status new_huffman_node(Huffman_Node **node, Huffman_Node *left_node, Huffman_Node *right_node);
Status create_frequency_list(Huffman_Encoder *encoder);

//--------------- Huffman tree ---------------
Status build_huffman_tree(Huffman_Encoder *encoder);
uint8_t get_height_tree(Huffman_Node *root);
void free_huffman_tree(Huffman_Tree **tree);

//--------------- Huffman codes ---------------
Status generate_codes(Huffman_Encoder *encoder, Huffman_Node *node, char *current_path);

Status compress_file_non_solid(Huffman_Encoder *encoder);
Status decompressed_file_non_solid(Huffman_Decoder *decoder);

//--------------- Get data ---------------
Huffman_Node* get_first_node(Huffman_Encoder *encoder);
Huffman_Node* get_next_node(Huffman_Node *node);
uint64_t get_node_frequency(Huffman_Node *node);
Huffman_Node* get_left_node(Huffman_Node *node);
Huffman_Node* get_right_node(Huffman_Node *node);
unsigned char get_symbol_node(Huffman_Node *node);
char** get_huffman_codes(Huffman_Encoder *encoder);
Bytes_Writer *get_writer_encoder(Huffman_Encoder *encoder);

#endif