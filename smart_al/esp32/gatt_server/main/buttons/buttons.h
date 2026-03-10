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

bool buttons_hw_init(gpio_num_t button_gpio);
bool buttons_hw_init_multi(gpio_num_t gpio_a, gpio_num_t gpio_b);
bool buttons_init(QueueHandle_t out_queue, gpio_num_t gpio_a, gpio_num_t gpio_b);

void buttons_task(void *param);

