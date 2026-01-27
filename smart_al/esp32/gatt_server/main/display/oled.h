#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#ifdef __cplusplus
extern "C" {
#endif

// OLED listens to button events from this queue
void oled_init(QueueHandle_t button_q);

#ifdef __cplusplus
}
#endif
