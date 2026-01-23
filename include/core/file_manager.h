#ifndef FILE_MANAGE_H
#define FILE_MANAGE_H
#include "types_errors.h"
#include "huffman.h"
#include <stdio.h>

Status open_file(FILE** file, const char *path, const char *mode);
Status read_text_file(unsigned char *text, Huffman_Encoder *encoder);

#endif