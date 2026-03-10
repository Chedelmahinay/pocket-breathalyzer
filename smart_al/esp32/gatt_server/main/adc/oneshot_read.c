#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "nvs.h"
#include "results/results.h"

#define LOG_LOCAL_LEVEL ESP_LOG_INFO

#define ADC_SAMPLE_PERIOD_MS      1000
#define ADC_CALIBRATION_SAMPLES   10
#define BAC_SCALE_FACTOR          0.0001f

static float maxValue = 0.0f;
static float sessionMaxValue = 0.0f;

const static char *ADC_TAG = "ADC";
const static char *NVS_TAG = "NVS";

#if CONFIG_IDF_TARGET_ESP32
#define EXAMPLE_ADC1_CHAN0          ADC_CHANNEL_4
#define EXAMPLE_ADC1_CHAN1          ADC_CHANNEL_5
#else
#define EXAMPLE_ADC1_CHAN0          ADC_CHANNEL_2
#define EXAMPLE_ADC1_CHAN1          ADC_CHANNEL_3
#endif

#define EXAMPLE_ADC_ATTEN           ADC_ATTEN_DB_12

static int adc_raw[2][10];

static QueueHandle_t adc_queue;

static int baseline = 0;
static bool calibrated = false;
static int calibration_samples = 0;
static int calibration_sum = 0;

static void adc_task(void *arg)
{
    adc_oneshot_unit_handle_t adc1_handle = (adc_oneshot_unit_handle_t)arg;

    esp_err_t err = nvs_load_max_value(&maxValue);
    if (err == ESP_OK) {
        ESP_LOGI(NVS_TAG, "Restored maxValue from NVS: %f", maxValue);
    } else {
        ESP_LOGI(NVS_TAG, "No saved maxValue, starting fresh");
        maxValue = 0.0f;
    }

    results_init(maxValue);
    sessionMaxValue = 0.0f;

    while (1) {
        ESP_ERROR_CHECK(
            adc_oneshot_read(adc1_handle, EXAMPLE_ADC1_CHAN0, &adc_raw[0][0])
        );

        float bac_value;

        if (!calibrated) {
            calibration_sum += adc_raw[0][0];
            calibration_samples++;

            if (calibration_samples >= ADC_CALIBRATION_SAMPLES) {
                baseline = calibration_sum / ADC_CALIBRATION_SAMPLES;
                calibrated = true;
            }

            bac_value = 0.0f;
        } else {
            int diff = adc_raw[0][0] - baseline;
            if (diff < 0) diff = 0;
            bac_value = (float)diff * BAC_SCALE_FACTOR;
        }

        if (bac_value > maxValue) {
            maxValue = bac_value;
            nvs_save_max_value(maxValue);
        }

        if (bac_value > sessionMaxValue) {
            sessionMaxValue = bac_value;
        }

        ESP_LOGI(ADC_TAG, "bac: %f",  bac_value);

        xQueueOverwrite(adc_queue, &bac_value);

        vTaskDelay(pdMS_TO_TICKS(ADC_SAMPLE_PERIOD_MS));
    }
}

void adc_init(QueueHandle_t q)
{
    adc_queue = q;

    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    adc_oneshot_chan_cfg_t config = {
        .atten = EXAMPLE_ADC_ATTEN,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, EXAMPLE_ADC1_CHAN0, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, EXAMPLE_ADC1_CHAN1, &config));

    xTaskCreate(adc_task, "adc_task", 2048, adc1_handle, tskIDLE_PRIORITY, NULL);
}

void adc_reset_session_max(void)
{
    sessionMaxValue = 0.0f;
}

void adc_commit_session_max(void)
{
    if (sessionMaxValue > 0.0f) {
        results_note_max(sessionMaxValue);
    }
}