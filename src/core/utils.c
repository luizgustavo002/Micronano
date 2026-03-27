#include "utils.h"
#include "file_manager.h"

#include <stdio.h>
#include <time.h>
#include <stdint.h>

static uint64_t size_bytes = 0;

void add_size_bytes(uint64_t size) {
    size_bytes += size;
}

static double calculate_time_in_seconds(struct timespec start, struct timespec end) {
    double seconds = (double)(end.tv_sec - start.tv_sec);
    double nanoseconds = (double)(end.tv_nsec - start.tv_nsec);
    
    return seconds + (nanoseconds / 1000000000.0);
}

void print_report(struct timespec start, struct timespec end, const char *path) {
    remove_trailing_separator((char *) path);
    double seconds = calculate_time_in_seconds(start, end);

    double size_mb = (double) size_bytes / (1024 * 1024);
    double mbps = size_mb / seconds;
    printf("============================================================\n");
    printf("Total processing time: %.2lfs\n", seconds);
    printf("Total size processed: %.2lfMB\n", size_mb);
    printf("Processing speed: %.2lfMB/s\n", mbps);
    printf("============================================================\n");
}
