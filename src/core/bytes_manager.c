#include "bytes_manager.h"
#include "file_manager.h"
#include "logger.h"
#include "types_errors.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

struct Bytes_Writer {
    uint8_t buffer_byte;
    uint8_t bits_written;
    FILE *file;
};

struct Bytes_Reader {
    uint8_t buffer_byte;
    uint8_t remaining_bits;
    FILE *file;
};

// Creates a Bytes_Writer instance, and assigns it to the provided pointer.
// - The function free_bytes_writer is responsible for freeing the instance and closing the file.
// - Returns STATUS_OK on success or specific error code on failure.
Status create_bytes_writer(Bytes_Writer **writer, const char *path) {
    // Validates that the argument is not NULL.
    if (writer == NULL || path == NULL) {
        char msg[128] = {0};
        if (writer == NULL) strcat(msg, "writer ");
        if (path == NULL) strcat(msg, "path ");
        log_message(LOG_ERROR, "create_bytes_writer invalid args: %s", msg);
        return ERROR_INVALID_ARGUMENT;
    }
    Status status = STATUS_OK;

    FILE *file;
    status = open_file(&file, path, "wb");
    ASSERT_STATUS_OK(status);

    *writer = (Bytes_Writer *)calloc(1, sizeof(Bytes_Writer));
    if ((*writer) == NULL) {
        log_message(LOG_ERROR, "Failed to allocate memory for Bytes_Writer.");
        fclose(file);
        return ERROR_MEMORY_ALLOCATION;
    }
    (*writer)->file = file;
    return STATUS_OK;
}

// Creates a Bytes_Reader instance, and assigns it to the provided pointer.
// - The caller is responsible for freeing the instace.
// - Returns STATUS_OK on success or the specific error code on failure.
Status create_bytes_reader(Bytes_Reader **reader, const char *path) {
    // Validates that the argument is not NULL.
    if (reader == NULL || path == NULL) {
        char msg[128] = {0};
        if (reader == NULL) strcat(msg, "reader ");
        if (path == NULL) strcat(msg, "path ");
        log_message(LOG_ERROR, "create_bytes_reader invalid args: %s", msg);
        return ERROR_INVALID_ARGUMENT;
    }
    Status status = STATUS_OK;

    FILE *file;
    status = open_file(&file, path, "rb");
    ASSERT_STATUS_OK(status);

    *reader = (Bytes_Reader *)calloc(1, sizeof(Bytes_Reader));
    if ((*reader) == NULL) {
        log_message(LOG_ERROR, "Failed to allocate memory for Bytes_Reader.");
        fclose(file);
        return ERROR_MEMORY_ALLOCATION;
    }
    (*reader)->file = file;
    return STATUS_OK;
}

// Writes to the file, updates the buffer, and resets helper variables.
static Status write_to_file(Bytes_Writer *writer) {
    if (writer == NULL) {
        char msg[128] = {0};
        if (writer == NULL) strcat(msg, "writer ");
        log_message(LOG_ERROR, "write_to_file invalid args: %s", msg);
        return ERROR_INVALID_ARGUMENT;
    }
    size_t result = fwrite(&writer->buffer_byte, sizeof(uint8_t), 1, writer->file);
    if (result != 1) {
        char *error_msg = strerror(errno);
        log_message(LOG_ERROR, "Could not write file: %s (errno: %d)", error_msg, errno);
        return ERROR_WRITING_FILE;
    }
    writer->bits_written = 0;
    writer->buffer_byte = 0;
    return STATUS_OK;
}

Status write_bit_to_file(uint8_t bit, Bytes_Writer *writer) {
    uint8_t byte = writer->buffer_byte;
    writer->buffer_byte = (byte << 1) | bit;
    writer->bits_written += 1;

    if (writer->bits_written == 8) {
        Status status = write_to_file(writer);
        if (status) return status;
    }
    return STATUS_OK;
}

Status write_multiple_bits_to_file(uint64_t bits, uint8_t size, Bytes_Writer *writer) {
    for (int i = size; i > 0; i--) {
        uint8_t bit = (bits >> (i - 1)) & 1;
        Status status = write_bit_to_file(bit, writer);
        if (status) return status;
    }
    return STATUS_OK;
}

Status write_byte_to_file(uint8_t byte, Bytes_Writer *writer) {
    Status status = write_multiple_bits_to_file(byte, 8, writer);
    if (status) return status;
    return STATUS_OK;
}

Status write_multiple_bytes_to_file(uint8_t *bytes, int size, Bytes_Writer *writer) {
    for (uint8_t *p = bytes; p < bytes + size; p++) {
        Status status = write_byte_to_file(*p, writer);
        if (status) return status;
    }
    return STATUS_OK;
}

Status write_padding(Bytes_Writer *writer) {
    while (writer->bits_written != 0) {
        Status status = write_bit_to_file(0, writer);
        if (status) return status;
    }
    fflush(writer->file);
    return STATUS_OK;
}

// Reads from the file, updates the buffer, and resets helper variables.
static Status read_file(Bytes_Reader *reader) {
    if (reader == NULL) {
        char msg[128] = {0};
        if (reader == NULL) strcat(msg, "reader ");
        log_message(LOG_ERROR, "read_file invalid args: %s", msg);
        return ERROR_INVALID_ARGUMENT;
    }
    size_t result = fread(&reader->buffer_byte, sizeof(uint8_t), 1, reader->file);
    if (result != 1) {
        if (feof(reader->file)) return ERROR_END_OF_FILE;
        if (ferror(reader->file)){
            char *error_msg = strerror(errno);
            log_message(LOG_ERROR, "Could not read file: %s (errno: %d)", error_msg, errno);
            return ERROR_READING_FILE;
        }
        return ERROR_GENERIC;
    }
    reader->remaining_bits = 8;
    return STATUS_OK;
}

Status read_bit_from_file(uint8_t *bit, Bytes_Reader *reader) {
    *bit = 0;
    if (reader->remaining_bits == 0) {
        Status status = read_file(reader);
        if (status) return status;
    }

    *bit = (reader->buffer_byte >> (reader->remaining_bits - 1)) & 1;
    reader->remaining_bits -= 1;

    return STATUS_OK;
}

Status read_multiple_bits_from_file(uint64_t *bits, uint8_t size, Bytes_Reader *reader) {
    *bits = 0;
    uint8_t bit = 0;
    for (int i = 0; i < size; i++) {
        Status status = read_bit_from_file(&bit, reader);
        if (status) return status;
        *bits = (*bits << 1) | bit;
    }
    return STATUS_OK;
}

Status read_byte_from_file(uint8_t *byte, Bytes_Reader *reader) {
    *byte = 0;
    uint64_t temp = 0;
    Status status = read_multiple_bits_from_file(&temp, 8, reader);
    if (status) return status;

    *byte = temp;
    return STATUS_OK;
}

Status read_multiple_bytes_from_file(uint8_t *bytes, int size, Bytes_Reader *reader) {
    uint8_t byte = 0;
    for (int i = 0; i < size; i++) {
        Status status = read_byte_from_file(&byte, reader);
        if (status) return status;
        bytes[i] = byte;
    }
    return STATUS_OK;
}

Status skip_padding(Bytes_Reader *reader) {
    Status status;
    uint8_t bit;
    while (reader->remaining_bits != 0) {
        status = read_bit_from_file(&bit, reader);
        if (status) return status;
    }
    return STATUS_OK;
}
//--------------- Free memory ---------------
void free_bytes_writer(Bytes_Writer **writer) {
    if (writer == NULL || *writer == NULL) return;
    fclose((*writer)->file);
    free(*writer);
    *writer = NULL;
}

void free_bytes_reader(Bytes_Reader **reader) {
    if (reader == NULL || *reader == NULL) return;
    fclose((*reader)->file);
    free(*reader);
    *reader = NULL;
}

//--------------- Get data ---------------
FILE *get_file_reader(Bytes_Reader *reader) { return reader->file; }