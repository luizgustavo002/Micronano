#include "logger.h"

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unity.h>

const char *log_file_name = "test_log.log";
const char *message[] = {"Test info", "Test debug", "Test warn", "Test error"};
FILE *log_file = NULL;

void setUp(void)
{
    char file_path[256];
    snprintf(file_path, sizeof(file_path), "logs/%s", log_file_name);
    remove(file_path);
}

void tearDown(void)
{
    char file_path[256];
    snprintf(file_path, sizeof(file_path), "logs/%s", log_file_name);
    remove(file_path);
}

void test_logger()
{
    logger_init(log_file_name, 1);
    log_message(LOG_INFO, message[0]);
    log_message(LOG_DEBUG, message[1]);
    log_message(LOG_WARN, message[2]);
    log_message(LOG_ERROR, message[3]);

    char file_path[256];
    snprintf(file_path, sizeof(file_path), "logs/%s", log_file_name);
    log_file = fopen(file_path, "r");
    TEST_ASSERT_NOT_NULL(log_file);

    char line[512];
    fgets(line, sizeof(line), log_file);

    TEST_ASSERT_EQUAL_STRING_LEN("[INFO]", line + 22, 6);
    TEST_ASSERT_EQUAL_STRING_LEN(message[0], line + 29, strlen(message[0]) - 1);

    fgets(line, sizeof(line), log_file);
    TEST_ASSERT_EQUAL_STRING_LEN("[DEBUG]", line + 22, 7);
    TEST_ASSERT_EQUAL_STRING_LEN(message[1], line + 30, strlen(message[1]) - 1);

    fgets(line, sizeof(line), log_file);
    TEST_ASSERT_EQUAL_STRING_LEN("[WARN]", line + 22, 6);
    TEST_ASSERT_EQUAL_STRING_LEN(message[2], line + 29, strlen(message[2]) - 1);

    fgets(line, sizeof(line), log_file);
    TEST_ASSERT_EQUAL_STRING_LEN("[ERROR]", line + 22, 7);
    TEST_ASSERT_EQUAL_STRING_LEN(message[3], line + 30, strlen(message[3]) - 1);

    logger_close();
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_logger);
    UNITY_END();
    return 0;
}