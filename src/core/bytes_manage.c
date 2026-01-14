#include "bytes_manage.h"
#include "types_errors.h"

#include <stdio.h>
#include <stdlib.h>

struct Bytes_Writer
{
    unsigned char buffer_byte;
    int bits_written;
    FILE *file;
};

struct Bytes_Reader
{
    unsigned char buffer_byte;
    int remaining_bits;
    FILE *file;
};

Status make_struct_bytes_write(Bytes_Writer **writer, FILE *file){
    *writer = (Bytes_Writer*) calloc(1, sizeof(Bytes_Writer));
    if (!(*writer))
        return ERROR_MEMORY_ALLOCATION;

    (*writer)->file = file;
    return STATUS_OK;
}

Status make_struct_bytes_reader(Bytes_Reader **reader, FILE *file){
    *reader = (Bytes_Reader*) calloc(1, sizeof(Bytes_Reader));

    if (!(*reader))
        return ERROR_MEMORY_ALLOCATION;

    (*reader)->file = file;
    return STATUS_OK;
}

void free_struct_bytes_write(Bytes_Writer *writer){
    if (writer) free(writer);
}

void free_struct_bytes_reader(Bytes_Reader *reader){
    if (reader) free(reader);
}

static Status write_to_file(Bytes_Writer *writer){
    size_t result = fwrite(&writer->buffer_byte, sizeof(unsigned char), 1, writer->file);
    if (!result){
        free_struct_bytes_write(writer);
        return ERROR_WRITING_FILE;
    }
    writer->bits_written = 0;
    writer->buffer_byte = 0;
    return STATUS_OK;
}

Status write_bit_to_file(int bit, Bytes_Writer *writer){
    unsigned char byte = writer->buffer_byte;
    writer->buffer_byte = (byte << 1) | bit;
    writer->bits_written += 1;

    if (writer->bits_written == 8){
        Status status = write_to_file(writer);
        if (status)
            return status;
    }
    return STATUS_OK;
}

Status write_multiple_bits_to_file(int bits, int size, Bytes_Writer *writer){
    for (int i = size; i > 0; i--)
    {
        int bit = (bits >> (i - 1)) & 1;
        Status status = write_bit_to_file(bit, writer);
        if (status)
            return status;
    }  
    return STATUS_OK; 
}

Status write_byte_to_file(unsigned char byte, Bytes_Writer *writer){
    for (int i = 7; i >= 0; i--)
    {
        int bit = (byte >> i) & 1;
        Status status = write_bit_to_file(bit, writer);
        if (status)
            return status;
    }   
    return STATUS_OK;
}

Status write_multiple_bytes_to_file(unsigned char *bytes, int size, Bytes_Writer *writer){
    for (int i = 0; i < size; i++)
    {
        Status status = write_byte_to_file(bytes[i], writer);
        if (status)
            return status;
    } 
    return STATUS_OK;
}

Status write_trash(Bytes_Writer *writer){
    while (writer->bits_written != 0)
    {
        Status status = write_bit_to_file(0, writer);
        if (status)
            return status;
    }
    return STATUS_OK;
}

static Status read_file(Bytes_Reader *reader){
    size_t result = fread(&reader->buffer_byte, sizeof(unsigned char), 1, reader->file);
    if (!result){
        if (feof(reader->file)){
            free_struct_bytes_reader(reader);
            return ERROR_END_OF_FILE;
        }
        return ERROR_READING_FILE;
    }
    reader->remaining_bits = 8;
    return STATUS_OK;
}

Status read_bit_from_file(int *bit, Bytes_Reader *reader){
    if (reader->remaining_bits == 0){
        Status status = read_file(reader);
        if (status)
            return status;
    }
 
    *bit = (reader->buffer_byte >> (reader->remaining_bits - 1)) & 1;
    reader->remaining_bits -= 1;
    
    return STATUS_OK; 
}

Status read_multiple_bits_from_file(int *bits, int size, Bytes_Reader *reader){
    *bits = 0;
    int bit = 0;
    for (int i = 0; i < size; i++)
    {
        Status status = read_bit_from_file(&bit, reader);
        if (status)
            return status;
        *bits = (*bits << 1) | bit;
    }  
    return STATUS_OK; 
}

Status read_byte_from_file(unsigned char *byte, Bytes_Reader *reader){
    *byte = 0;
    int bit;
    for (int i = 0; i < 8; i++){
        Status status = read_bit_from_file(&bit, reader);
        if (status)
            return status;
        *byte = (*byte << 1) | bit;
    }
    
    return STATUS_OK;
}

Status read_multiple_bytes_from_file(unsigned char *bytes, int size, Bytes_Reader *reader){
    unsigned char byte = 0;
    for (int i = 0; i < size; i++)
    {
        Status status = read_byte_from_file(&byte, reader);
        if (status)
            return status;
        bytes[i] = byte;
    }  
    return STATUS_OK; 
}
