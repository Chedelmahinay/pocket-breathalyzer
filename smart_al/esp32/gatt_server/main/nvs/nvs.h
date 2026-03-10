#pragma once

#include <stddef.h>
#include "esp_err.h"

void nvs_init(void);
esp_err_t nvs_load_max_value(float *value);
esp_err_t nvs_save_max_value(float value);

esp_err_t nvs_save_blob(const char *key, const void *data, size_t size);
esp_err_t nvs_load_blob(const char *key, void *data, size_t size);