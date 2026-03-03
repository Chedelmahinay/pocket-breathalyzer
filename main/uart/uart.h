#pragma once
#include "freertos/queue.h"

#ifdef __cplusplus
extern "C" {
#endif

void uart_init_bac(QueueHandle_t adc_queue);
void bac_start_measurement(void);

#ifdef __cplusplus
}
#endif
