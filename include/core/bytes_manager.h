#ifndef BYTES_MANAGER_H
#define BYTES_MANAGER_H

#include "types_errors.h"
#include <stdio.h>
#include <stdint.h>

typedef struct Bytes_Writer Bytes_Writer;
typedef struct Bytes_Reader Bytes_Reader;

Status create_bytes_writer(Bytes_Writer **writer, const char *path);
Status create_bytes_reader(Bytes_Reader **reader, const char *path);
void free_bytes_writer(Bytes_Writer **writer);
void free_bytes_reader(Bytes_Reader **reader);

Status write_bit_to_file(uint8_t bit, Bytes_Writer *write);
Status write_multiple_bits_to_file(uint64_t bits, uint8_t size, Bytes_Writer *writer);
Status write_byte_to_file(unsigned char byte, Bytes_Writer *write);
Status write_multiple_bytes_to_file(unsigned char *bytes, int size, Bytes_Writer *writer);
Status write_padding(Bytes_Writer *writer);

Status read_bit_from_file(uint8_t *bit, Bytes_Reader *reader);
Status read_multiple_bits_from_file(uint64_t *bits, uint8_t size, Bytes_Reader *reader);
Status read_byte_from_file(unsigned char *byte, Bytes_Reader *reader);
Status read_multiple_bytes_from_file(unsigned char *bytes, int size, Bytes_Reader *reader);
Status skip_padding(Bytes_Reader *reader);

FILE* get_file_reader(Bytes_Reader *reader);

#endif