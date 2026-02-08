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


void app_main(void)
{
    adc_queue = xQueueCreate(1, sizeof(float));
    button_queue = xQueueCreate(8, sizeof(button_event_t));

    ble_init(adc_queue);
    adc_init(adc_queue);
    buttons_init(button_queue, BTN_TOGGLE_GPIO, BTN_CONFIRM_GPIO);
    oled_init(button_queue);

    //just a debug
    ESP_LOGI(TAG, "Buttons: TOGGLE=GPIO%d CONFIRM=GPIO%d",
             (int)BTN_TOGGLE_GPIO, (int)BTN_CONFIRM_GPIO);
}