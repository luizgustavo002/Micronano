#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include "types_errors.h"
#include <stdio.h>

#define ASSERT_STATUS_OK_TEST(status)                                        \
    do                                                                       \
    {                                                                        \
        if (status)                                                          \
        {                                                                    \
            TEST_ASSERT_EQUAL_INT(STATUS_OK, status);                        \
            TEST_FAIL_MESSAGE("The function failed; please check the log."); \
        }                                                                    \
    } while (0)

Status create_temp_file(const char *name, const unsigned char *data, size_t size, char *full_path);
void make_directory(const char *name, char *full_path);
Status populate_directory(unsigned char **datas, char **names, int file_count, char (*full_path)[4096]);
void remove_directory(const char *root);
void get_current_working_directory(char *path);

#endif