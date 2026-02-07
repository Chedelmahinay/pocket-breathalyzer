#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

#include "adc/oneshot_read.h"
#include "ble/gatts_demo.h"
#include "buttons/buttons.h"
#include "display/oled.h"

#define TAG "main"

#define BTN_TOGGLE_GPIO  GPIO_NUM_9
#define BTN_CONFIRM_GPIO GPIO_NUM_4

QueueHandle_t adc_queue;
static QueueHandle_t button_queue;

static buttons_task_params_t btn_params;

void app_main(void)
{
    adc_queue = xQueueCreate(1, sizeof(float));
    button_queue = xQueueCreate(8, sizeof(button_event_t));

    ble_init(adc_queue);
    adc_init(adc_queue);

    // Init both buttons
    if (!buttons_hw_init_multi(BTN_TOGGLE_GPIO, BTN_CONFIRM_GPIO)) {
        ESP_LOGE(TAG, "buttons_hw_init_multi failed");
        return;
    }

    btn_params.out_queue = button_queue;

    xTaskCreate(buttons_task, "buttons_task", 2048, &btn_params, 10, NULL);

    oled_init(button_queue);

    ESP_LOGI(TAG, "Buttons: TOGGLE=GPIO%d CONFIRM=GPIO%d",
             (int)BTN_TOGGLE_GPIO, (int)BTN_CONFIRM_GPIO);
}
