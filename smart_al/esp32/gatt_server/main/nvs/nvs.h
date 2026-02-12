#pragma once

#include "esp_err.h"

void nvs_init(void);
esp_err_t nvs_load_max_value(float *value);
esp_err_t nvs_save_max_value(float value);
