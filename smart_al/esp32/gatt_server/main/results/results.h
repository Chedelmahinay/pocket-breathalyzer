#pragma once

#include <stdbool.h>

#define RESULTS_MAX_COUNT 3

typedef struct {
    float values[RESULTS_MAX_COUNT];
    int count;
} results_snapshot_t;

bool results_init(float initial_max_value);
bool results_note_max(float max_value);
bool results_get_snapshot(results_snapshot_t *snapshot);