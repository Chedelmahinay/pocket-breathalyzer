#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"

static const char *TAG = "NVS";

void nvs_init(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
        err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }
}

esp_err_t nvs_load_max_value(float *value)
{
    nvs_handle_t nvs;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &nvs);
    if (err != ESP_OK) {
        return err;
    }

    size_t size = sizeof(float);
    err = nvs_get_blob(nvs, "maxValue", value, &size);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Loaded maxValue: %f", *value);
    }

    nvs_close(nvs);
    return err;
}

esp_err_t nvs_save_max_value(float value)
{
    nvs_handle_t nvs;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs);
    if (err != ESP_OK) {
        return err;
    }

    err = nvs_set_blob(nvs, "maxValue", &value, sizeof(value));
    if (err == ESP_OK) {
        err = nvs_commit(nvs);
    }

    nvs_close(nvs);
    return err;
}
