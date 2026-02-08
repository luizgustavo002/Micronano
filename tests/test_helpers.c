#include "test_helpers.h"
#include "types_errors.h"
#include "logger.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
#include <dirent.h>
#include <windows.h>
#define CREATE_DIR(path) _mkdir(path)
#define RMDIR_CMD "rmdir /s /q"
#define PATH_SEP '\\'
#define GetCurrentDir _getcwd
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define CREATE_DIR(path) mkdir(path, 0755)
#define RMDIR_CMD "rm -rf"
#define PATH_SEP '/'
#define GetCurrentDir getcwd
#endif

Status create_temp_file(const char *name, const unsigned char *data, size_t size, char *full_path)
{
    char cwd[4096];
    if (GetCurrentDir(cwd, sizeof(cwd)) == NULL)
        return ERROR_GENERIC;
    
    char aux_path[16384];
    snprintf(aux_path, sizeof(aux_path), "%s%c%s", cwd, PATH_SEP, name);

    FILE *file = fopen(aux_path, "w");
    if (!file)
    {
        log_message(LOG_ERROR, "Could not open file: %s", aux_path);
        return ERROR_FILE_DOES_NOT_EXIST;
    }
    
    if (size == 0 || data == NULL)
    {
        fclose(file);
        return STATUS_OK;
    }

    size_t result = fwrite(data, size, 1, file);
    if (result != 1)
    {
        log_message(LOG_ERROR, "Could not write to file: %s", aux_path);
        fclose(file);
        return ERROR_WRITING_FILE;
    }
    fclose(file);
    if (full_path != NULL)
        strcpy(full_path, aux_path);
    
    return STATUS_OK;
}

void make_directory(const char *name, char *full_path)
{
    char cwd[4096];
    if (GetCurrentDir(cwd, sizeof(cwd)) == NULL)
        return;
    
    char aux_path[16384];
    snprintf(aux_path, sizeof(aux_path), "%s%c%s", cwd, PATH_SEP, name);

    CREATE_DIR(aux_path);
    strcpy(full_path, aux_path);
}

Status populate_directory(unsigned char **datas, char **names, int file_count, char (*full_path)[4096])
{
    for (int i = 0; i < file_count; i++)
    {
        create_temp_file(names[i], datas[i], strlen((char *)datas[i]), full_path[i]);
    }
    return STATUS_OK;
}

void remove_directory(const char *root)
{
    char aux_path[4096];
    snprintf(aux_path, sizeof(aux_path), "%s %s", RMDIR_CMD, root);
    system(aux_path);
}

void get_current_working_directory(char *path){
    char cwd[4096];
    if (GetCurrentDir(cwd, sizeof(cwd)) == NULL)
        return;
    strcpy(path, cwd);
}
