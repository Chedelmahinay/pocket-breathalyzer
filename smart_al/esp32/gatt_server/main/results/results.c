#include "results.h"
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "nvs.h"


#define RESULTS_TAG              "RESULTS"
#define RESULTS_NVS_KEY          "results"
#define RESULTS_MIN_VALID_VALUE  0.0f
#define RESULTS_MUTEX_TIMEOUT_MS 100

typedef struct {
    float values[RESULTS_MAX_COUNT];
    int count;
} results_data_t;

static results_data_t g_results;
static SemaphoreHandle_t g_results_mutex = NULL;

bool results_init(float initial_max_value)
{
    if (g_results_mutex == NULL) {
        g_results_mutex = xSemaphoreCreateMutex();
        if (g_results_mutex == NULL) {
            ESP_LOGE(RESULTS_TAG, "mutex create failed");
            return false;
        }
    }

    if (xSemaphoreTake(g_results_mutex, pdMS_TO_TICKS(RESULTS_MUTEX_TIMEOUT_MS)) != pdTRUE) {
        ESP_LOGE(RESULTS_TAG, "mutex lock failed");
        return false;
    }

    memset(&g_results, 0, sizeof(g_results));

    esp_err_t err = nvs_load_blob(RESULTS_NVS_KEY, &g_results, sizeof(g_results));
    if (err != ESP_OK) {
        g_results.count = 0;

        if (initial_max_value > RESULTS_MIN_VALID_VALUE) {
            g_results.values[0] = initial_max_value;
            g_results.count = 1;
            nvs_save_blob(RESULTS_NVS_KEY, &g_results, sizeof(g_results));
        }
    }

    if (g_results.count < 0) {
        g_results.count = 0;
    }

    if (g_results.count > RESULTS_MAX_COUNT) {
        g_results.count = RESULTS_MAX_COUNT;
    }

    xSemaphoreGive(g_results_mutex);
    return true;
}

bool results_note_max(float max_value)
{
    if (max_value <= RESULTS_MIN_VALID_VALUE) {
        return false;
    }

    if (xSemaphoreTake(g_results_mutex, pdMS_TO_TICKS(RESULTS_MUTEX_TIMEOUT_MS)) != pdTRUE) {
        ESP_LOGE(RESULTS_TAG, "mutex lock failed");
        return false;
    }

    if (g_results.count < RESULTS_MAX_COUNT) {
        g_results.values[g_results.count] = max_value;
        g_results.count++;
    } else {
        g_results.values[0] = g_results.values[1];
        g_results.values[1] = g_results.values[2];
        g_results.values[2] = max_value;
    }

    esp_err_t err = nvs_save_blob(RESULTS_NVS_KEY, &g_results, sizeof(g_results));

    xSemaphoreGive(g_results_mutex);

    if (err != ESP_OK) {
        ESP_LOGE(RESULTS_TAG, "save failed");
        return false;
    }

    return true;
}

bool results_get_snapshot(results_snapshot_t *snapshot)
{
    if (snapshot == NULL) {
        return false;
    }

    if (xSemaphoreTake(g_results_mutex, pdMS_TO_TICKS(RESULTS_MUTEX_TIMEOUT_MS)) != pdTRUE) {
        ESP_LOGE(RESULTS_TAG, "mutex lock failed");
        return false;
    }

    memset(snapshot, 0, sizeof(*snapshot));
    snapshot->count = g_results.count;

    for (int i = 0; i < g_results.count; i++) {
        snapshot->values[i] = g_results.values[i];
    }

    xSemaphoreGive(g_results_mutex);
    return true;
}