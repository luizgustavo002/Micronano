#ifndef FILE_MANAGE_H
#define FILE_MANAGE_H
#include "types_errors.h"
#include "huffman.h"
#include <stdio.h>

typedef struct File_Header File_Header;
typedef struct File_List File_List;

Status open_file(FILE** file, const char *path, const char *mode);
Status create_file_header(File_Header **file_header, const char *absolute_path, const char *base_path);

Status init_file_list(File_List **list, const char *base_path);
Status is_regular_file(int *result, const char *path);
Status scan_directory(File_List *list, const char *path);
Status set_base_path(char **base_path, const char* path);
void set_compress_output_file_path(char full_path[], const char *input_path, const char *output_path, File_List *list);

void free_file_header(File_Header **file_header);
void free_file_list(File_List **list);

Status write_file_header(File_Header *file_header, Bytes_Writer *writer);
Status read_file_header(Huffman_Decoder *decoder);

Status log_compressing_file(File_List *list);

uint16_t get_path_size(File_Header *file_header);
uint64_t get_file_size(File_Header *file_header);
char *get_base_path(File_List *list);
Status get_next_absolute_path(char **path, File_List *list);
int get_is_directory(File_Header *file_header);
void get_relative_path(char *path, File_Header *file_header);

#endif