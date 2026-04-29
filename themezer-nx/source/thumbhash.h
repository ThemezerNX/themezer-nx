#pragma once

#include <stdint.h>
#include <stddef.h>

int ThumbHashToRGBA(const uint8_t *hash, size_t hashLen, int maxSize, uint8_t **rgba, int *width, int *height);