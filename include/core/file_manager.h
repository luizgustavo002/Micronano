#ifndef FILE_MANAGE_H
#define FILE_MANAGE_H
#include "types_errors.h"
#include "huffman.h"
#include <stdio.h>

typedef struct File_Header File_Header;

Status open_file(FILE** file, const char *path, const char *mode);
Status create_file_header(File_Header **file_header, const char *path_file);
void free_file_header(File_Header **file_header);

uint16_t get_path_size(File_Header *file_header);
uint64_t get_file_size(File_Header *file_header);
char* get_relative_path(File_Header *file_header);

#endif