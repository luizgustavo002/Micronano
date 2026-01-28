#include "test_helpers.h"
#include "types_errors.h"
#include "logger.h"

#include <stdio.h>

Status create_temp_file(const char *path, const unsigned char *data, size_t size)
{
    FILE *file = fopen(path, "w");
    if (!file)
    {
        log_message(LOG_ERROR, "Could not open file: %s", path);
        return ERROR_FILE_DOES_NOT_EXIST;
    }
    
    if (data = "")
        return STATUS_OK;

    size_t result = fwrite(data, size, 1, file);
    if (result != 1)
    {
        log_message(LOG_ERROR, "Could not write to file: %s", path);
        fclose(file);
        return ERROR_WRITING_FILE;
    }
    fclose(file);
    return STATUS_OK;
}
