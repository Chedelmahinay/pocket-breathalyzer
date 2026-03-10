#pragma once
#include "freertos/queue.h"

#ifdef __cplusplus
extern "C" {
#endif

void adc_init(QueueHandle_t adc_queue);
void adc_reset_session_max(void);
void adc_commit_session_max(void);

#ifdef __cplusplus
}
#endif

