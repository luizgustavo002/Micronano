#include "file_manager.h"
#include "types_errors.h"
#include "huffman.h"
#include "logger.h"

#include <stdio.h>

Status open_file(FILE** file, const char *path, const char *mode)
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


Status read_text_file(unsigned char *text, Huffman_Encoder *encoder)
{
    
}
