#ifndef BYTES_MANAGE_H
#define BYTES_MANAGE_H
#include <stdio.h>

typedef struct Bytes_Writer Bytes_Writer;
typedef struct Bytes_Reader Bytes_Reader;

Bytes_Writer* make_struct_bytes_write(FILE *arquivo);
Bytes_Reader* make_struct_bytes_reader(FILE *file);

void write_bit_to_file(int bit, Bytes_Writer *write);
void write_multiple_bits_to_file(int *bits, int size, Bytes_Writer *writer);
void write_byte_to_file(unsigned char byte, Bytes_Writer *write);
void write_multiple_bytes_to_file(unsigned char *bytes, int size, Bytes_Writer *writer);
void write_trash(Bytes_Writer *writer);

int read_bit_from_file(Bytes_Reader *reader);
unsigned char read_byte_from_file(Bytes_Reader *reader);

#endif