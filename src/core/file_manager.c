#include "file_manager.h"
#include "types_errors.h"
#include "huffman.h"
#include "huffman_internal.h"
#include "logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#define PATH_SEP '\\'
#else
#include <dirent.h>
#include <sys/stat.h>
#define PATH_SEP '/'
#endif

#define FILE_EXTENSION ".hlg"

struct File_Header
{
    int is_directory;
    uint16_t path_size;
    char *relative_path;
    uint64_t file_size;
};

struct File_List
{
    char **relative_paths;
    size_t current_path;
    char *base_path;
    size_t count;
    size_t capacity;
};

Status open_file(FILE **file, const char *path, const char *mode)
{
    *file = NULL;
    *file = fopen(path, mode);
    if ((*file) == NULL)
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

static Status make_relative_path(char **relative_path, const char *absolute_path, const char *base_path)
{
    char *aux_path = (char*) absolute_path + strlen(base_path) + 1;

    *relative_path = (char *)calloc(strlen(aux_path) + 1, sizeof(char));
    if (*relative_path == NULL)
    {
        log_message(LOG_ERROR, "Failed to allocate memory for relative_path.");
        return ERROR_MEMORY_ALLOCATION;
    }
    strcpy(*relative_path, aux_path);

    return STATUS_OK;
}

Status create_file_header(File_Header **file_header, const char *absolute_path, const char *base_path)
{
    Status status = STATUS_OK;

    log_message(LOG_DEBUG, "Creating File_Header.");
    *file_header = (File_Header *)calloc(1, sizeof(File_Header));
    if ((*file_header) == NULL)
    {
        log_message(LOG_ERROR, "Failed to allocate memory for File_Header.");
        return ERROR_MEMORY_ALLOCATION;
    }

    int is_file;
    status = is_regular_file(&is_file, absolute_path);
    ASSERT_STATUS_OK(status);
    (*file_header)->is_directory = !(is_file);

    status = make_relative_path(&(*file_header)->relative_path, absolute_path, base_path);
    ASSERT_STATUS_OK(status);

    size_t len = strlen((*file_header)->relative_path);
    (*file_header)->path_size = len;

    if (is_file)
    {
        status = calculate_file_size(&(*file_header)->file_size, absolute_path);
        ASSERT_STATUS_OK(status);
    }
    else
        (*file_header)->file_size = 0;

    return STATUS_OK;
}

void set_compress_output_file_path(char full_path[], const char *input_path, const char *output_path, File_List *list)
{
    char base_path[4096];
    strcpy(base_path, list->base_path);
    char file_output_name[4096];
    strcpy(file_output_name, input_path + strlen(base_path) + 1);
    char aux_path[8192];
    snprintf(aux_path, sizeof(aux_path), "%s%c%s%s", output_path, PATH_SEP, file_output_name, FILE_EXTENSION);
    strcpy(full_path, aux_path);
}

//--------------- File list ---------------
Status set_base_path(char **base_path, const char* path)
{
    *base_path = (char *)calloc(strlen(path) + 1, sizeof(char));
    if (*base_path == NULL)
    {
        log_message(LOG_ERROR, "Failed to allocate memory for base_path.");
        return ERROR_MEMORY_ALLOCATION;
    }
    strcpy(*base_path, path);

    char *aux_path = NULL;
    #ifdef _WIN32
        aux_path = strrchr(*base_path, '\\');
        if (aux_path == NULL || strlen(path) <= 3)
            return STATUS_OK;
    #else
        aux_path = strrchr(*base_path, '/');
        if (aux_path == NULL || strlen(path) <= 1)
            return STATUS_OK;
    #endif
    
    *aux_path = '\0';
    return STATUS_OK;
}

Status init_file_list(File_List **list, const char *path)
{
    *list = (File_List *)calloc(1, sizeof(File_List));
    if (*list == NULL)
    {
        log_message(LOG_ERROR, "Failed to allocate memory for File_List.");
        return ERROR_MEMORY_ALLOCATION;
    }
    (*list)->current_path = 0;
    (*list)->count = 0;
    (*list)->capacity = 20;
    (*list)->relative_paths = (char **)calloc((*list)->capacity, sizeof(char *));
    if ((*list)->relative_paths == NULL)
    {
        log_message(LOG_ERROR, "Failed to allocate memory for relative_paths in File_List.");
        return ERROR_MEMORY_ALLOCATION;
    }

    Status status = set_base_path(&(*list)->base_path, path);
    ASSERT_STATUS_OK(status);

    return STATUS_OK;
}

static Status add_to_file_list(File_List *list, const char *path)
{
    if (list->count >= list->capacity)
    {
        list->capacity *= 2;
        list->relative_paths = realloc(list->relative_paths, list->capacity * sizeof(char *));
        if (list->relative_paths == NULL)
        {
            log_message(LOG_ERROR, "Failed to allocate memory for relative_paths in realloc.");
            return ERROR_MEMORY_ALLOCATION;
        }
    }

    list->relative_paths[list->count] = (char *)calloc(strlen(path) + 1, sizeof(char));
    if (list->relative_paths[list->count] == NULL)
    {
        log_message(LOG_ERROR, "Failed to allocate memory for relative_paths in %zu.", list->count);
        return ERROR_MEMORY_ALLOCATION;
    }
    strcpy(list->relative_paths[list->count], path);
    list->count++;

    return STATUS_OK;
}

Status is_regular_file(int *result, const char *path)
{
    *result = 0;
#ifdef _WIN32
    DWORD dwAttrib = GetFileAttributesA(path);
    if (dwAttrib == INVALID_FILE_ATTRIBUTES)
    {
        log_message(LOG_ERROR, "Could not open file: %s", path);
        return ERROR_FILE_DOES_NOT_EXIST;
    }

    *result = !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
#else
    struct stat path_stat;
    if (stat(path, &path_stat) != 0)
    {
        log_message(LOG_ERROR, "Could not open file: %s", path);
        return ERROR_FILE_DOES_NOT_EXIST;
    }
    *result = S_ISREG(path_stat.st_mode);
#endif

    return STATUS_OK;
}

Status scan_directory(File_List *list, const char *path)
{
    Status status = STATUS_OK;

    int is_file;
    status = is_regular_file(&is_file, path);
    ASSERT_STATUS_OK(status);

    char *relative_path;
    status = make_relative_path(&relative_path, path, list->base_path);
    ASSERT_STATUS_OK(status);
    status = add_to_file_list(list, relative_path);
    ASSERT_STATUS_OK(status);
    free(relative_path);
    
    if (is_file)
        return STATUS_OK;

    char full_path[16384];
#ifdef _WIN32
    WIN32_FIND_DATA find_data;
    HANDLE h_find;
    char search_path[16384];

    snprintf(search_path, sizeof(search_path), "%s\\*", path);

    h_find = FindFirstFile(search_path, &find_data);

    if (h_find == INVALID_HANDLE_VALUE)
    {
        log_message(LOG_ERROR, "Could not open directory: %s", search_path);
        return ERROR_FILE_DOES_NOT_EXIST;
    }

    do
    {
        const char *name = find_data.cFileName;

        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
        {
            continue;
        }

        snprintf(full_path, sizeof(full_path), "%s\\%s", path, name);

        if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            status = scan_directory(list, full_path);
            ASSERT_STATUS_OK(status);
        }
        else
        {
            status = make_relative_path(&relative_path, path, list->base_path);
            ASSERT_STATUS_OK(status);
            status = add_to_file_list(list, name);
            ASSERT_STATUS_OK(status);
            free(relative_path);
        }

    } while (FindNextFile(h_find, &find_data) != 0);

    FindClose(h_find);
    return STATUS_OK;
#else
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        log_message(LOG_ERROR, "Could not open directory: %s", path);
        return ERROR_FILE_DOES_NOT_EXIST;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        char *name = entry->d_name;
        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
        {
            continue;
        }
        snprintf(full_path, sizeof(full_path), "%s/%s", path, name);

        struct stat st;
        if (stat(full_path, &st) == -1)
        {
            log_message(LOG_WARN, "Could not locate '%s'. File will be omitted from archive.", full_path);
            continue;
        }

        if (S_ISDIR(st.st_mode))
        {
            status = scan_directory(list, full_path);
            ASSERT_STATUS_OK(status);
        }
        else
        {
            status = make_relative_path(&relative_path, full_path, list->base_path);
            ASSERT_STATUS_OK(status);
            status = add_to_file_list(list, relative_path);
            ASSERT_STATUS_OK(status);
            free(relative_path);
        }
    }
    closedir(dir);
#endif

    return STATUS_OK;
}

//--------------- Free memory ---------------
void free_file_header(File_Header **file_header)
{
    log_message(LOG_DEBUG, "Freeing File_Header.");
    if (*file_header == NULL)
        return;

    if ((*file_header)->relative_path != NULL)
        free((*file_header)->relative_path);
    (*file_header)->relative_path = NULL;

    free((*file_header));
    *file_header = NULL;
}

void free_file_list(File_List **list)
{
    if (list == NULL || *list == NULL)
        return;
    
    for (char **p = (*list)->relative_paths; p < (*list)->relative_paths + (*list)->count; p++)
    {
        free(*p);
        *p = NULL;
    }
    free((*list)->relative_paths);
    (*list)->relative_paths = NULL;
    free((*list)->base_path);
    (*list)->base_path = NULL;
    free(*list);
    *list = NULL;
}

//--------------- File manager ---------------
Status write_file_header(File_Header *file_header, Bytes_Writer *writer)
{
    Status status = STATUS_OK;
    status = write_bit_to_file(file_header->is_directory, writer);
    ASSERT_STATUS_OK(status);

    status = write_multiple_bits_to_file(file_header->path_size, 15, writer);
    ASSERT_STATUS_OK(status);

    status = write_multiple_bytes_to_file((unsigned char *)file_header->relative_path, file_header->path_size, writer);
    ASSERT_STATUS_OK(status);

    if (!file_header->is_directory)
    {
        status = write_multiple_bits_to_file(file_header->file_size, 40, writer);
        ASSERT_STATUS_OK(status);
    }

    return STATUS_OK;
}

Status read_file_header(Huffman_Decoder *decoder)
{
    Status status = STATUS_OK;

    log_message(LOG_DEBUG, "Readeing File_Header.");
    decoder->file_header = (File_Header *)calloc(1, sizeof(File_Header));
    if (decoder->file_header == NULL)
    {
        log_message(LOG_ERROR, "Failed to allocate memory for File_Header.");
        return ERROR_MEMORY_ALLOCATION;
    }
    File_Header *file_header = decoder->file_header;

    status = read_bit_from_file((uint8_t *) &file_header->is_directory, decoder->reader);
    ASSERT_STATUS_OK(status);

    uint64_t result;
    status = read_multiple_bits_from_file(&result, 15, decoder->reader);
    ASSERT_STATUS_OK(status);
    file_header->path_size = result;

    char *path_file = (char *)calloc(file_header->path_size + 1, sizeof(char));
    file_header->relative_path = (char *)calloc(file_header->path_size + 1, sizeof(char));
    status = read_multiple_bytes_from_file((unsigned char *)path_file, file_header->path_size, decoder->reader);
    ASSERT_STATUS_OK(status);
    strcpy(file_header->relative_path, path_file);

    char absolute_path[8192];
    snprintf(absolute_path, sizeof(absolute_path), "%s%c%s", decoder->output_path, PATH_SEP, file_header->relative_path);


    if (file_header->is_directory == 0)
    {
        status = read_multiple_bits_from_file(&file_header->file_size, 40, decoder->reader);
        ASSERT_STATUS_OK(status);
        FILE *file;
        status = open_file(&file, absolute_path, "wb");
        ASSERT_STATUS_OK(status);
        status = create_bytes_writer(&decoder->writer, file);
        ASSERT_STATUS_OK(status);
    }

    free(path_file);

    return STATUS_OK;
}

//--------------- Get data ---------------
uint16_t get_path_size(File_Header *file_header)
{
    return file_header->path_size;
}

uint64_t get_file_size(File_Header *file_header)
{
    return file_header->file_size;
}

char *get_base_path(File_List *list)
{
    return list->base_path;
}

Status get_next_absolute_path(char **path, File_List *list)
{
    if (list->current_path >= list->count)
    {
        *path = NULL;
        return STATUS_OK;
    }

    char aux_path[16384];
    char *relative_path = list->relative_paths[list->current_path];
    list->current_path++;
    snprintf(aux_path, sizeof(aux_path), "%s%c%s", list->base_path, PATH_SEP, relative_path);
    
    *path = (char *)calloc(strlen(aux_path) + 1, sizeof(char));
    if (*path == NULL)
    {
        log_message(LOG_ERROR, "Failed to allocate memory for get_next_absolute_path");
        return ERROR_MEMORY_ALLOCATION;
    }
    strcpy(*path, aux_path);

    return STATUS_OK;
}

int get_is_directory(File_Header *file_header)
{
    return file_header->is_directory;
}

void get_relative_path(char *path, File_Header *file_header)
{
    strcpy(path, file_header->relative_path);
}
