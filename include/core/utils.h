#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdio.h>
#include <time.h>

void add_size_bytes(uint64_t size);
void print_report(struct timespec start, struct timespec end, const char *path);

#endif