#ifndef HUFFMAN_API_H
#define HUFFMAN_API_H

#include "types_errors.h"

typedef enum {
    // Compresses the file or directory independently of other entries.
    PROCESSING_SETTING_NON_SOLID,
    PROCESSING_SETTING_NON_SOLID_DEBUG_LOG
} Processing_Settings;

Status compress(const char *input_path, const char *output_path, Processing_Settings setting);
Status decompress(const char *input_path, const char *output_path, Processing_Settings setting);

#endif