#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

#include "adc/oneshot_read.h"
#include "ble/gatts_demo.h"
#include "buttons/buttons.h"

#define TAG "main"
#define BTN_GPIO GPIO_NUM_9

QueueHandle_t adc_queue;
static QueueHandle_t button_queue;

static buttons_task_params_t btn_params;

static void button_consumer_task(void *arg)
{
    (void)arg;

    while (1) {
        button_event_t evt;
        if (xQueueReceive(button_queue, &evt, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI(TAG, "Button event received: gpio=%d", (int)evt.gpio);
            // Later: update OLED / UI
        }
    }
}

void app_main(void)
{
    adc_queue = xQueueCreate(1, sizeof(float));
    button_queue = xQueueCreate(8, sizeof(button_event_t));

    ble_init(adc_queue);
    adc_init(adc_queue);

    if (!buttons_hw_init(BTN_GPIO)) {
        ESP_LOGE(TAG, "buttons_hw_init failed");
        return;
    }

    btn_params.gpio = BTN_GPIO;
    btn_params.out_queue = button_queue;

    xTaskCreate(buttons_task, "buttons_task", 2048, &btn_params, 10, NULL);
    xTaskCreate(button_consumer_task, "btn_consume", 3072, NULL, 9, NULL);
}
