#ifndef BYTES_MANAGE_H
#define BYTES_MANAGE_H
#include "types_errors.h"
#include <stdio.h>

typedef struct Bytes_Writer Bytes_Writer;
typedef struct Bytes_Reader Bytes_Reader;

Status make_struct_bytes_write(Bytes_Writer **writer, FILE *file);
Status make_struct_bytes_reader(Bytes_Reader **reader, FILE *file);

Status write_bit_to_file(int bit, Bytes_Writer *write);
Status write_multiple_bits_to_file(int bits, int size, Bytes_Writer *writer);
Status write_byte_to_file(unsigned char byte, Bytes_Writer *write);
Status write_multiple_bytes_to_file(unsigned char *bytes, int size, Bytes_Writer *writer);
Status write_trash(Bytes_Writer *writer);

Status read_bit_from_file(int *bit, Bytes_Reader *reader);
Status read_byte_from_file(unsigned char *byte, Bytes_Reader *reader);

#endif