#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include "types_errors.h"
#include <stdio.h>

#define ASSERT_STATUS_OK(status)                                             \
    do                                                                       \
    {                                                                        \
        if (status)                                                          \
        {                                                                    \
            TEST_ASSERT_EQUAL_INT(STATUS_OK, status);                        \
            TEST_FAIL_MESSAGE("The function failed; please check the log."); \
        }                                                                    \
    } while (0)

Status create_temp_file(const char *path, const unsigned char *data, size_t size);

#endif