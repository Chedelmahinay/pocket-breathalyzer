#include "adc_fanout.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"

#define TAG "adc_fanout"

typedef struct {
    QueueHandle_t src_q;
    QueueHandle_t ble_q;
    QueueHandle_t oled_q;
} fanout_params_t;

static void adc_fanout_task(void *arg)
{
    fanout_params_t *p = (fanout_params_t *)arg;
    float value = 0.0f;

    ESP_LOGI(TAG, "fanout started (src=%p ble=%p oled=%p)",
             (void*)p->src_q, (void*)p->ble_q, (void*)p->oled_q);

    while (1) {
        if (xQueueReceive(p->src_q, &value, portMAX_DELAY) == pdTRUE) {

            if (p->ble_q) {
                xQueueOverwrite(p->ble_q, &value);
            }
            if (p->oled_q) {
                xQueueOverwrite(p->oled_q, &value);
            }
        }
    }
}

void adc_fanout_init(QueueHandle_t src_q, QueueHandle_t ble_q, QueueHandle_t oled_q)
{
    static fanout_params_t params;  

    params.src_q  = src_q;
    params.ble_q  = ble_q;
    params.oled_q = oled_q;

    xTaskCreate(adc_fanout_task,"adc_fanout", 2048, &params,tskIDLE_PRIORITY + 2, NULL);
}
