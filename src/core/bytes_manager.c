#include "bytes_manager.h"
#include "types_errors.h"
#include "logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct Bytes_Writer
{
    uint8_t buffer_byte;
    uint8_t bits_written;
    FILE *file;
};

struct Bytes_Reader
{
    uint8_t buffer_byte;
    uint8_t remaining_bits;
    FILE *file;
};

Status create_bytes_writer(Bytes_Writer **writer, FILE *file)
{
    log_message(LOG_DEBUG, "Creating Bytes_Writer.");
    *writer = (Bytes_Writer *)calloc(1, sizeof(Bytes_Writer));
    if (!(*writer)){
        log_message(LOG_ERROR, "Failed to allocate memory for Bytes_Writer.");
        return ERROR_MEMORY_ALLOCATION;
    }
    
    (*writer)->file = file;
    return STATUS_OK;
}

Status create_bytes_reader(Bytes_Reader **reader, FILE *file)
{
    log_message(LOG_DEBUG, "Creating Bytes_Reader.");
    *reader = (Bytes_Reader *)calloc(1, sizeof(Bytes_Reader));
    if (!(*reader))
    {
        log_message(LOG_ERROR, "Failed to allocate memory for Bytes_Reader.");
        return ERROR_MEMORY_ALLOCATION;
    }

    (*reader)->file = file;
    return STATUS_OK;
}

//--------------- Writer ---------------
static Status write_to_file(Bytes_Writer *writer)
{
    //log_message(LOG_DEBUG, "Reading byte from file.");
    size_t result = fwrite(&writer->buffer_byte, sizeof(uint8_t), 1, writer->file);
    if (result != 1)
    {
        log_message(LOG_ERROR, "Could not write to file");
        return ERROR_WRITING_FILE;
    }
    writer->bits_written = 0;
    writer->buffer_byte = 0;
    return STATUS_OK;
}

Status write_bit_to_file(uint8_t bit, Bytes_Writer *writer)
{
    uint8_t byte = writer->buffer_byte;
    writer->buffer_byte = (byte << 1) | bit;
    writer->bits_written += 1;

    if (writer->bits_written == 8)
    {
        Status status = write_to_file(writer);
        if (status)
            return status;
    }
    return STATUS_OK;
}

Status write_multiple_bits_to_file(uint64_t bits, uint8_t size, Bytes_Writer *writer)
{
    for (int i = size; i > 0; i--)
    {
        uint8_t bit = (bits >> (i - 1)) & 1;
        Status status = write_bit_to_file(bit, writer);
        if (status)
            return status;
    }
    return STATUS_OK;
}

Status write_byte_to_file(uint8_t byte, Bytes_Writer *writer)
{
    Status status = write_multiple_bits_to_file(byte, 8, writer);
    if (status)
        return status;
    return STATUS_OK;
}

Status write_multiple_bytes_to_file(uint8_t *bytes, int size, Bytes_Writer *writer)
{
    for (uint8_t *p = bytes; p < bytes + size; p++)
    {
        Status status = write_byte_to_file(*p, writer);
        if (status)
            return status;
    }
    return STATUS_OK;
}

Status write_padding(Bytes_Writer *writer)
{
    log_message(LOG_DEBUG, "Writing padding bits.");
    while (writer->bits_written != 0)
    {
        Status status = write_bit_to_file(0, writer);
        if (status)
            return status;
    }
    fflush(writer->file);
    return STATUS_OK;
}

//--------------- Reader ---------------
static Status read_file(Bytes_Reader *reader)
{
    size_t result = fread(&reader->buffer_byte, sizeof(uint8_t), 1, reader->file);
    if (!result)
    {
        if (feof(reader->file))
        {
            return ERROR_END_OF_FILE;
        }
        return ERROR_READING_FILE;
    }
    reader->remaining_bits = 8;
    return STATUS_OK;
}

Status read_bit_from_file(uint8_t *bit, Bytes_Reader *reader)
{
    *bit = 0;
    if (reader->remaining_bits == 0)
    {
        Status status = read_file(reader);
        if (status)
            return status;
    }

    *bit = (reader->buffer_byte >> (reader->remaining_bits - 1)) & 1;
    reader->remaining_bits -= 1;

    return STATUS_OK;
}

Status read_multiple_bits_from_file(uint64_t *bits, uint8_t size, Bytes_Reader *reader)
{
    *bits = 0;
    uint8_t bit = 0;
    for (int i = 0; i < size; i++)
    {
        Status status = read_bit_from_file(&bit, reader);
        if (status)
            return status;
        *bits = (*bits << 1) | bit;
    }
    return STATUS_OK;
}

Status read_byte_from_file(uint8_t *byte, Bytes_Reader *reader)
{
    *byte = 0;
    uint64_t temp = 0;
    Status status = read_multiple_bits_from_file(&temp, 8, reader);
    if (status)
        return status;

    *byte = temp;
    return STATUS_OK;
}

Status read_multiple_bytes_from_file(uint8_t *bytes, int size, Bytes_Reader *reader)
{
    uint8_t byte = 0;
    for (int i = 0; i < size; i++)
    {
        Status status = read_byte_from_file(&byte, reader);
        if (status)
            return status;
        bytes[i] = byte;
    }
    return STATUS_OK;
}

Status skip_padding(Bytes_Reader *reader)
{
    Status status;
    uint8_t bit;
    while (reader->remaining_bits != 0)
    {
        status = read_bit_from_file(&bit, reader);
        if (status)
            return status;
    }
    return STATUS_OK;
}
//--------------- Free memory ---------------
void free_bytes_writer(Bytes_Writer **writer)
{
    log_message(LOG_DEBUG, "Freeing Bytes_Writer.");
    if (*writer == NULL)
        return;
    fclose((*writer)->file);
    if (*writer)
        free(*writer);
    *writer = NULL;
}

void free_bytes_reader(Bytes_Reader **reader)
{
    log_message(LOG_DEBUG, "Freeing Bytes_Reader.");
    if (*reader == NULL)
        return;
    fclose((*reader)->file);
    if (*reader)
        free(*reader);
    *reader = NULL;
}

//--------------- Get data ---------------
FILE* get_file_reader(Bytes_Reader *reader)
{
    return reader->file;
}