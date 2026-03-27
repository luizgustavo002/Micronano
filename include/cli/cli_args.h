#ifndef CLI_ARGS_H
#define CLI_ARGS_H
#include "huffman_api.h"
#include "types_errors.h"

typedef enum {
    MODE_UNKNOWN = 0,
    MODE_COMPRESS,
    MODE_DECOMPRESS,
    MODE_HELP
} Operation_Mode;

typedef struct {
    Operation_Mode mode;
    Processing_Settings setting;
    char input_path[8192];
    char output_path[8192];
} Cli_Arguments;

Status parse_arguments(int argc, char *argv[], Cli_Arguments **arguments);

#endif