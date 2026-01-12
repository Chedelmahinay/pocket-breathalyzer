#include <stdint.h>            
#include "freertos/FreeRTOS.h" 
#include "freertos/queue.h"    
#include "adc/oneshot_read.h"  
#include "ble/gatts_demo.h"    

QueueHandle_t adc_queue;

void app_main(void){

    adc_queue = xQueueCreate(1, sizeof(float));

    ble_init(adc_queue);
    adc_init(adc_queue);
}