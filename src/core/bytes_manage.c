#include <bytes_manage.h>

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


Bytes_Writer* make_struct_bytes_write(FILE *file){
    Bytes_Writer *writer;
    writer = (Bytes_Writer*) calloc(1, sizeof(Bytes_Writer));
    if (writer == NULL){
        printf("Falha ao alocar memória.");
        return NULL;
    }

    writer->file = file;
    return writer;
}

Bytes_Reader* make_struct_bytes_reader(FILE *file){
    Bytes_Reader *reader;
    reader = (Bytes_Reader*) calloc(1, sizeof(Bytes_Reader));

    if (reader == NULL){
        printf("Falha ao alocar memória.");
        return NULL;
    }

    reader->file = file;
    return reader;
}

static void write_to_file(Bytes_Writer *writer){
    fwrite(&writer->buffer_byte, sizeof(unsigned char), 1, writer->file);
    writer->bits_written = 0;
    writer->buffer_byte = 0;
}

void write_bit_to_file(int bit, Bytes_Writer *writer){
    unsigned char byte = writer->buffer_byte;
    writer->buffer_byte = (byte << 1) | bit;
    writer->bits_written += 1;

    if (writer->bits_written == 8) 
        write_to_file(writer);
}

void write_multiple_bits_to_file(int *bits, int size, Bytes_Writer *writer){
    for (int i = 0; i < size; i++)
    {
        write_bit_to_file(bits[i], writer);
    }   
}

void write_byte_to_file(unsigned char byte, Bytes_Writer *writer){
    for (int i = 7; i >= 0; i--)
    {
        int bit = (byte >> i) & 1;
        write_bit_to_file(bit, writer);
    }   
}

void write_multiple_bytes_to_file(unsigned char *bytes, int size, Bytes_Writer *writer){
    for (int i = 0; i < size; i++)
    {
        write_byte_to_file(bytes[i], writer);
    } 
}

void write_trash(Bytes_Writer *writer){
    while (writer->bits_written != 0)
    {
        write_bit_to_file(0, writer);
    }
}

static void read_file(Bytes_Reader *reader){
    fread(&reader->buffer_byte, sizeof(unsigned char), 1, reader->file);
    reader->remaining_bits = 8;
}

int read_bit_from_file(Bytes_Reader *reader){
    if (reader->remaining_bits == 0)
        read_file(reader);
 
    int bit = (reader->buffer_byte >> (reader->remaining_bits - 1)) & 1;
    reader->remaining_bits -= 1;
    
    return bit; 
}

unsigned char read_byte_from_file(Bytes_Reader *reader){
    unsigned char byte = 0;
    for (int i = 0; i < 8; i++){
        int bit = read_bit_from_file(reader);
        byte = (byte << 1) | bit;
    }
    
    return byte;
}