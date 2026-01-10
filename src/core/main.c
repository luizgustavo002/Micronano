#include "bytes_manage.h"

#include <stdio.h>

int main(){
    FILE *file = fopen("teste", "wb");
    Bytes_Writer *write =  make_struct_bytes_write(file);
    write_byte_to_file('A', write);
    write_byte_to_file('B', write);
    write_byte_to_file('C', write);
    fclose(file);

    return 0;
}