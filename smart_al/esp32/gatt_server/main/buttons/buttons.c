#include "buttons.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/task.h"

#define TAG "buttons"
#define DEBOUNCE_MS 60

static QueueHandle_t s_isr_q;
static gpio_num_t s_gpio;

static void IRAM_ATTR button_isr(void *arg)
{
    (void)arg;
    uint32_t gpio_num = (uint32_t)s_gpio;

    BaseType_t hp = pdFALSE;
    xQueueSendFromISR(s_isr_q, &gpio_num, &hp);
    if (hp) portYIELD_FROM_ISR();
}

bool buttons_hw_init(gpio_num_t button_gpio)
{
    s_gpio = button_gpio;
    s_isr_q = xQueueCreate(8, sizeof(uint32_t));
    if (!s_isr_q) return false;

    gpio_config_t cfg = {
        .pin_bit_mask = 1ULL << button_gpio,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE
    };
    if (gpio_config(&cfg) != ESP_OK) return false;

    esp_err_t err = gpio_install_isr_service(0);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) return false;

    if (gpio_isr_handler_add(button_gpio, button_isr, NULL) != ESP_OK) return false;

    ESP_LOGI(TAG, "buttons_hw_init ok (gpio=%d)", (int)button_gpio);
    return true;
}

void buttons_task(void *param)
{
    buttons_task_params_t *p = (buttons_task_params_t *)param;

    int64_t last_ms = 0;

    while (1) {
        uint32_t gpio_num;
        if (xQueueReceive(s_isr_q, &gpio_num, portMAX_DELAY) == pdTRUE) {

            int64_t now_ms = esp_timer_get_time() / 1000;
            if ((now_ms - last_ms) < DEBOUNCE_MS) continue;
            last_ms = now_ms;

            int level = gpio_get_level((gpio_num_t)gpio_num);
            if (level == 0) {
                button_event_t evt = {
                    .gpio = (gpio_num_t)gpio_num,
                    .type = BUTTON_EVENT_PRESSED,
                    .level = level
                };
                (void)xQueueSend(p->out_queue, &evt, 0);
                ESP_LOGI(TAG, "PRESSED gpio=%d", (int)gpio_num);
            }
        }
    }
}
