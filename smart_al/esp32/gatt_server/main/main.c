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


#define TAG "main"

#define BTN_TOGGLE_GPIO  GPIO_NUM_9
#define BTN_CONFIRM_GPIO GPIO_NUM_4

#include <stdint.h>            
#include "freertos/FreeRTOS.h" 
#include "freertos/queue.h"    
#include "adc/oneshot_read.h"  
#include "ble/gatts_demo.h"    
#include "nvs.h"

QueueHandle_t adc_queue;
static QueueHandle_t button_queue;

static QueueHandle_t adc_src_q;
static QueueHandle_t ble_q;
static QueueHandle_t oled_q;



void app_main(void)
{
    nvs_init();
        
    adc_queue = xQueueCreate(1, sizeof(float));
    adc_src_q = xQueueCreate(1, sizeof(float));
    ble_q     = xQueueCreate(1, sizeof(float));
    oled_q    = xQueueCreate(1, sizeof(float));

    button_queue = xQueueCreate(8, sizeof(button_event_t));

    assert(adc_src_q);
    assert(ble_q);
    assert(oled_q);
    assert(button_queue);

    // just a debug
    ESP_LOGI(TAG, "Q: adc_src=%p ble=%p oled=%p btn=%p",
             (void*)adc_src_q, (void*)ble_q, (void*)oled_q, (void*)button_queue);

    ble_init(ble_q);          // changed to read from ble_q instead of adc_queue. Fanout copies adc_src -> (ble_q + oled_q)
    adc_init(adc_src_q);      // only changed main not adc/ble/nvs. changed this to write to adc_src_q instead of ble_q. it Fans out copies adc_src -> (ble_q + oled_q) to make not just the ble but also oled.c this time to read the adc output. basically just spreads the adc value to multiple queues w/o adc task needing to know about them. 
 
    adc_fanout_init(adc_src_q, ble_q, oled_q);

    buttons_init(button_queue, BTN_TOGGLE_GPIO, BTN_CONFIRM_GPIO);
    oled_init(button_queue, oled_q);

    // just a debug
    ESP_LOGI(TAG, "Buttons: TOGGLE=GPIO%d CONFIRM=GPIO%d",
             (int)BTN_TOGGLE_GPIO, (int)BTN_CONFIRM_GPIO);
}