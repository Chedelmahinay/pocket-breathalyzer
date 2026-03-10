#include "buttons.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <string.h>
#define TAG "buttons"
#define DEBOUNCE_MS 60

static QueueHandle_t s_isr_q;
static int64_t s_last_ms[40];


static void IRAM_ATTR button_isr(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;

    BaseType_t hp = pdFALSE;
    if (s_isr_q) {
        xQueueSendFromISR(s_isr_q, &gpio_num, &hp);
    }
    if (hp) portYIELD_FROM_ISR();
}

static bool config_one_button(gpio_num_t gpio)
{
    gpio_config_t cfg = {
        .pin_bit_mask = 1ULL << gpio,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE
    };

    if (gpio_config(&cfg) != ESP_OK) return false;

    if (gpio_isr_handler_add(gpio, button_isr, (void *)gpio) != ESP_OK) return false;

    ESP_LOGI(TAG, "configured gpio=%d", (int)gpio);
    return true;
}

bool buttons_hw_init_multi(gpio_num_t gpio_a, gpio_num_t gpio_b)
{
    if (!s_isr_q) {
        s_isr_q = xQueueCreate(16, sizeof(uint32_t));
        if (!s_isr_q) return false;
        memset(s_last_ms, 0, sizeof(s_last_ms));
    }

    esp_err_t err = gpio_install_isr_service(0);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) return false;

    if (gpio_a != GPIO_NUM_NC) {
        if (!config_one_button(gpio_a)) return false;
    }
    if (gpio_b != GPIO_NUM_NC) {
        if (!config_one_button(gpio_b)) return false;
    }

    ESP_LOGI(TAG, "buttons_hw_init_multi ok (A=%d B=%d)", (int)gpio_a, (int)gpio_b);
    return true;
}


void buttons_task(void *param)
{
    buttons_task_params_t *p = (buttons_task_params_t *)param;

    const int STABLE_DELAY_PRESSED = 20;
    const int RELEASE_CHECK_INTERVAL= 10;
    const int RELEASE_DEBOUNCE_MS = 30;
    const int MICROSECONDS_IN_MS = 1000;
    const int ACTIVE_LOW = 0;
    
    while (1) {
        uint32_t gpio_u32 = 0;

        if (xQueueReceive(s_isr_q, &gpio_u32, portMAX_DELAY) == pdTRUE) {
            gpio_num_t gpio = (gpio_num_t)gpio_u32;

            if (((int)gpio >= 0) &&
                ((int)gpio < (int)(sizeof(s_last_ms) / sizeof(s_last_ms[0])))) {

                int64_t now_ms = esp_timer_get_time() / MICROSECONDS_IN_MS;

                if ((now_ms - s_last_ms[gpio]) >= DEBOUNCE_MS) {
                    vTaskDelay(pdMS_TO_TICKS(STABLE_DELAY_PRESSED));

                    if (gpio_get_level(gpio) == ACTIVE_LOW) {
                        button_event_t evt = {
                            .gpio = gpio,
                            .type = BUTTON_EVENT_PRESSED,
                            .level = ACTIVE_LOW
                        };

                        (void)xQueueSend(p->out_queue, &evt, portMAX_DELAY);
                        ESP_LOGI(TAG, "PRESSED gpio=%d", (int)gpio);

                        s_last_ms[gpio] = esp_timer_get_time() / MICROSECONDS_IN_MS;

                        while (gpio_get_level(gpio) == ACTIVE_LOW) {
                            vTaskDelay(pdMS_TO_TICKS(RELEASE_CHECK_INTERVAL));
                        }

                        vTaskDelay(pdMS_TO_TICKS(RELEASE_DEBOUNCE_MS));
                        s_last_ms[gpio] = esp_timer_get_time() / MICROSECONDS_IN_MS;
                    }
                }
            }
        }
    }
}

bool buttons_init(QueueHandle_t out_queue, gpio_num_t gpio_a, gpio_num_t gpio_b)
{
    if (!buttons_hw_init_multi(gpio_a, gpio_b)) {
        ESP_LOGE(TAG, "buttons_hw_init_multi failed");
        return false;
    }

    static buttons_task_params_t params;
    params.out_queue = out_queue;

    xTaskCreate(buttons_task, "buttons_task", 2048, &params, 10, NULL);

    return true;
}
