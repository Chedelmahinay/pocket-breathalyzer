#pragma once
#include "freertos/queue.h"

#ifdef __cplusplus
extern "C" {
#endif

void adc_init(QueueHandle_t adc_queue);

#ifdef __cplusplus
}
#endif
