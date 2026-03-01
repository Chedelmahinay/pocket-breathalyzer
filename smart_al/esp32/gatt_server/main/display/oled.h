#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#ifdef __cplusplus
extern "C" {
#endif

void oled_init(QueueHandle_t button_q, QueueHandle_t adc_q);

#ifdef __cplusplus
}
#endif
