#include "cli_args.h"
#include "logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <dirent.h>
#include <windows.h>
#define GetCurrentDir _getcwd
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

static void get_current_working_directory(char *path) {
    char cwd[8192];
    if (GetCurrentDir(cwd, sizeof(cwd)) == NULL) return;
    strcpy(path, cwd);
}

static void help_printf(const char *name) {
    printf("Usage: %s <command> <input path> [output path] [settings]\n\n", name);

    printf("Commands: \n");
    printf("\t-c, --compress         \t Compress a file or directory\n");
    printf("\t-d, --decompress       \t Decompress a .hlg file\n");
    printf("\t-h, --help             \t Show this help message\n");
    printf("\n");

    printf("Settings (Default --non-solid): \n");
    printf("\t-ns, --non-solid       \t Process files independently\n");
    printf("\t-nsd, --non-solid-debug\t Process files independently with debug logs\n");
}

static Status validate_command(int argc, char *argv[], Cli_Arguments *arguments) {
    const char *command = argv[1];
    if (strcmp(command, "-h") == 0 || strcmp(command, "--help") == 0) {
        help_printf(argv[0]);
        arguments->mode = MODE_HELP;
    } else if ((strcmp(command, "-c") == 0 || strcmp(command, "--compress") == 0) && argc >= 3) {
        arguments->mode = MODE_COMPRESS;
        strcpy(arguments->input_path, argv[2]);
    } else if ((strcmp(command, "-d") == 0 || strcmp(command, "--decompress") == 0) && argc >= 3) {
        arguments->mode = MODE_DECOMPRESS;
        strcpy(arguments->input_path, argv[2]);
    } else {
        printf("Unknown command: '%s'. Use --help for more information.\n", command);
        arguments->mode = MODE_UNKNOWN;
        free(arguments);
        return ERROR_INVALID_ARGUMENT;
    }
    return STATUS_OK;
}

static Status validate_optional_args(int current_argc, char *argv[], Cli_Arguments *arguments) {
    const char *optional = argv[current_argc];
    if (strcmp(optional, "-ns") == 0 || strcmp(optional, "--non-solid") == 0) {
        arguments->setting = PROCESSING_SETTING_NON_SOLID;
        if (arguments->output_path[0] == '\0') get_current_working_directory(arguments->output_path);
    } else if (strcmp(optional, "-nsd") == 0 || strcmp(optional, "--non-solid-debug") == 0) {
        arguments->setting = PROCESSING_SETTING_NON_SOLID_DEBUG_LOG;
        if (arguments->output_path[0] == '\0') get_current_working_directory(arguments->output_path);
    } else {
        if (arguments->output_path[0] == '\0') {
            strcpy(arguments->output_path, optional);
            return STATUS_OK;
        }else {
            printf("Invalid arguments. Use -h or --help for more information.\n");
            free(arguments);
            return ERROR_INVALID_ARGUMENT;
        }
    }
    return STATUS_OK;
}

Status parse_arguments(int argc, char *argv[], Cli_Arguments **arguments) {
    Status status = STATUS_OK;
    *arguments = (Cli_Arguments *)calloc(1, sizeof(Cli_Arguments));
    if ((*arguments) == NULL) {
        printf("Failed to allocate memory for parse_arguments.\n");
        return ERROR_MEMORY_ALLOCATION;
    }

    if (argc < 2) {
        printf("Invalid arguments. Use -h or --help for more information.\n");
        free(*arguments);
        return ERROR_INVALID_ARGUMENT;
    }

    (*arguments)->setting = PROCESSING_SETTING_NON_SOLID;
    status = validate_command(argc, argv, *arguments);
    ASSERT_STATUS_OK(status);

    if (argc < 4) {
        get_current_working_directory((*arguments)->output_path);
        return STATUS_OK;
    }

    status = validate_optional_args(3, argv, *arguments);
    ASSERT_STATUS_OK(status);

    if (argc < 5) return STATUS_OK;

    status = validate_optional_args(4, argv, *arguments);
    ASSERT_STATUS_OK(status);

    return STATUS_OK;
}