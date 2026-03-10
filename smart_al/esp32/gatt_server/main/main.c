#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_assert.h"
#include "adc/oneshot_read.h"
#include "ble/gatts_demo.h"
#include "buttons/buttons.h"
#include "display/oled.h"
#include "fanout/adc_fanout.h"  
#include "nvs.h"

#define TAG "main"
#define BTN_TOGGLE_GPIO  GPIO_NUM_9
#define BTN_CONFIRM_GPIO GPIO_NUM_4

QueueHandle_t adc_queue;
static QueueHandle_t button_queue;
static QueueHandle_t adc_src_q;
static QueueHandle_t ble_q;
static QueueHandle_t oled_q;

void app_main(void)
{
    nvs_init();
        
    adc_queue    = xQueueCreate(1, sizeof(float));
    adc_src_q    = xQueueCreate(1, sizeof(float));
    ble_q        = xQueueCreate(1, sizeof(float));
    oled_q       = xQueueCreate(1, sizeof(float));
    button_queue = xQueueCreate(8, sizeof(button_event_t));

    assert(adc_src_q);
    assert(ble_q);
    assert(oled_q);
    assert(button_queue);

    ble_init(ble_q);          
    adc_init(adc_src_q);       
    adc_fanout_init(adc_src_q, ble_q, oled_q);
    buttons_init(button_queue, BTN_TOGGLE_GPIO, BTN_CONFIRM_GPIO);
    oled_init(button_queue, oled_q);

}