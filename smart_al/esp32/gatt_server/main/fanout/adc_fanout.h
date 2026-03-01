#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

void adc_fanout_init(QueueHandle_t src_q, QueueHandle_t ble_q, QueueHandle_t oled_q);