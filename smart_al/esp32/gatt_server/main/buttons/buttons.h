#pragma once
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

typedef enum {
    BUTTON_EVENT_PRESSED = 0
} button_event_type_t;

typedef struct {
    gpio_num_t gpio;
    button_event_type_t type;
    int level;
} button_event_t;

typedef struct {
    QueueHandle_t out_queue;
} buttons_task_params_t;

// Old single-button init (kept for compatibility)
bool buttons_hw_init(gpio_num_t button_gpio);

// New: init 2 buttons (toggle + confirm)
bool buttons_hw_init_multi(gpio_num_t gpio_a, gpio_num_t gpio_b);

// Task body: debounces and sends button_event_t to out_queue
void buttons_task(void *param);
