#include <stdint.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "lcd.h"
#include "decode_image.h"
#include "uart.h"
#include "gatt_server.h"

#define BTN 17

QueueHandle_t adc_queue;

void app_main(void)
{
    adc_queue = xQueueCreate(1, sizeof(float));

    lcd_init_driver();
    uart_init_bac(adc_queue);

    gpio_config_t io = {
        .pin_bit_mask = (1ULL << BTN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 1,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io);

    uint16_t *img1;
    uint16_t *img2;
    uint16_t *img3;

    decode_image(&img1);
    lcd_display_image(img1);
    vTaskDelay(pdMS_TO_TICKS(2000));

    decode_cat(&img2);
    lcd_display_image(img2);
    vTaskDelay(pdMS_TO_TICKS(2000));

    lcd_display_image(img1);
    vTaskDelay(pdMS_TO_TICKS(2000));

    decode_menu(&img3);
    lcd_display_image(img3);

    ble_init(adc_queue);

    esp_sleep_pd_config(ESP_PD_DOMAIN_VDDSDIO, ESP_PD_OPTION_ON);
    esp_sleep_enable_gpio_wakeup();
    gpio_wakeup_enable(BTN, GPIO_INTR_LOW_LEVEL);

    time_t last_activity;
    time(&last_activity);

    while (1)
    {
        time_t now;
        time(&now);

        if (difftime(now, last_activity) >= 10)
        {
            while (gpio_get_level(BTN) == 0)
                vTaskDelay(pdMS_TO_TICKS(1));

            lcd_display_black();
            esp_light_sleep_start();
            esp_restart();
        }

        if (gpio_get_level(BTN) == 0)
        {
            vTaskDelay(pdMS_TO_TICKS(50));

            if (gpio_get_level(BTN) == 0)
            {
                bac_start_measurement();
                time(&last_activity);

                while (gpio_get_level(BTN) == 0)
                    vTaskDelay(pdMS_TO_TICKS(10));
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
