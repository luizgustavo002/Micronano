#include "cli_args.h"
#include "huffman_api.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) { 
    Cli_Arguments *arguments = NULL;
    struct timespec start, end;

    if (parse_arguments(argc, argv, &arguments)) return 1;
    if (arguments->mode == MODE_UNKNOWN) return 1;

    clock_gettime(CLOCK_MONOTONIC, &start);
    if (arguments->mode == MODE_COMPRESS) {
        if (compress(arguments->input_path, arguments->output_path, arguments->setting) != 0) {
            printf("Process failed. See log for more information.\n");
            return 1;
        }
    } else if (arguments->mode == MODE_DECOMPRESS) {
        if (decompress(arguments->input_path, arguments->output_path, arguments->setting) != 0) {
            printf("Process failed. See log for more information.\n");
            return 1;
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    print_report(start, end, arguments->input_path);
    free(arguments);
    return 0;
 }