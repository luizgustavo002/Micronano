#include "file_manager.h"
#include "types_errors.h"
#include "huffman.h"
#include "huffman_internal.h"
#include "logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct File_Header
{
    uint8_t is_directory;
    uint16_t path_size;
    char *relative_path;
    uint64_t file_size;
};

Status open_file(FILE **file, const char *path, const char *mode)
{
    *file = NULL;
    *file = fopen(path, mode);
    if (!(*file))
    {
        log_message(LOG_ERROR, "Could not open file: %s", path);
        return ERROR_FILE_DOES_NOT_EXIST;
    }
    return STATUS_OK;
}

static Status calculate_file_size(uint64_t *size, const char *path_file)
{
    Status status = STATUS_OK;
    FILE *file;

    status = open_file(&file, path_file, "rb");
    if (status)
        return status;

    fseek(file, 0, SEEK_END);
    *size = ftell(file);

    fclose(file);
    return STATUS_OK;
}

Status create_file_header(File_Header **file_header, const char *path_file)
{
    Status status = STATUS_OK;
    FILE *file;

    log_message(LOG_DEBUG, "Creating Huffman_Encoder.");
    *file_header = (File_Header *)calloc(1, sizeof(File_Header));
    if (!(*file_header))
    {
        log_message(LOG_ERROR, "Failed to allocate memory for File_Header.");
        return ERROR_MEMORY_ALLOCATION;
    }

    status = open_file(&file, path_file, "rb");
    if (status)
        return status;

    (*file_header)->is_directory = 0;

    size_t len = strlen(path_file);
    (*file_header)->path_size = len;

    (*file_header)->relative_path = (char *)calloc(1, strlen(path_file) + 1);
    strcpy((*file_header)->relative_path, path_file);

    status = calculate_file_size(&(*file_header)->file_size, path_file);
    if (status)
        return status;

    return STATUS_OK;
}

void free_file_header(File_Header **file_header)
{
    if (*file_header == NULL)
        return;

    if ((*file_header)->relative_path != NULL)
        free((*file_header)->relative_path);
    (*file_header)->relative_path = NULL;

    free((*file_header));
    *file_header = NULL;
}

uint16_t get_path_size(File_Header *file_header)
{
    return file_header->path_size;
}

uint64_t get_file_size(File_Header *file_header)
{
    return file_header->file_size;
}

char *get_relative_path(File_Header *file_header)
{
    return file_header->relative_path;
}
