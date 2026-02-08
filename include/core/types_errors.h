#ifndef TYPES_ERRORS_H
#define TYPES_ERRORS_H
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    STATUS_OK = 0,
    ERROR_GENERIC,
    ERROR_FILE_DOES_NOT_EXIST,
    ERROR_PERMISSION_DENIED,
    ERROR_OPENING_FILE,
    ERROR_MEMORY_ALLOCATION,
    ERROR_WRITING_FILE,
    ERROR_READING_FILE,
    ERROR_END_OF_FILE,
    ERROR_INVALID_ARGUMENT,
    ERROR_BUFFER_TOO_SMALL,
    STATUS_END_OF_PROCESS
} Status;

#define ASSERT_STATUS_OK(status)                                                                                       \
    do {                                                                                                               \
        if (status) {                                                                                                  \
            return status;                                                                                             \
        }                                                                                                              \
    } while (0)

#define ASSERT_STATUS_OK_AND_FREES(status, ...)                                                                        \
    do {                                                                                                               \
        if (status) {                                                                                                  \
            void *resources[] = {__VA_ARGS__};                                                                         \
            size_t num_ptrs = sizeof(resources) / sizeof(resources[0]);                                                \
            for (size_t i = 0; i < num_ptrs; i++) {                                                                    \
                free(resources[i]);                                                                                    \
            }                                                                                                          \
            return status;                                                                                             \
        }                                                                                                              \
    } while (0)

#endif