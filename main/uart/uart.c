#include <stdio.h>
#include <string.h>
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lcd.h"
#include "esp_heap_caps.h"

#define UART_NUM UART_NUM_1
#define UART_TX  1
#define UART_RX  3

#define SAMPLE_COUNT 100

static float samples[SAMPLE_COUNT];
static int sample_index = 0;
static int collecting = 0;

static uint16_t *fb;
static QueueHandle_t adc_queue;

static void uart_task(void *arg)
{
    uint8_t buf[64];
    fb = heap_caps_malloc(LCD_WIDTH * LCD_HEIGHT * 2, MALLOC_CAP_DMA);

    while (1) {
        int len = uart_read_bytes(UART_NUM, buf, sizeof(buf), pdMS_TO_TICKS(100));

        if (len >= 9 && buf[0] == 0xFF && buf[1] == 0x17) {
            uint16_t raw = ((uint16_t)buf[4] << 8) | buf[5];
            float bac = raw / 10.0f;

            if (collecting && sample_index < SAMPLE_COUNT) {
                samples[sample_index++] = bac;
            }
        }
    }
}

void bac_start_measurement(void)
{
    sample_index = 0;
    collecting = 1;

    //for (int i = 0; i < LCD_WIDTH * LCD_HEIGHT; i++) fb[i] = 0x0000;
    //draw_string_buf(fb, 30, 60, "Blow Me", 0x07E0);

    for (int y = 0; y < LCD_HEIGHT; y += PARALLEL_LINES) {
        send_lines(y, &fb[y * LCD_WIDTH]);
        send_line_finish();
    }

    for (int i = 7; i > 0; i--) {
        char s[8];
        snprintf(s, sizeof(s), "%d", i);

        for (int j = 0; j < LCD_WIDTH * LCD_HEIGHT; j++) fb[j] = 0x0000;
        draw_string_buf(fb, 30, 60, "BLOW ME", 0x07E0);
        draw_string_buf(fb, 60, 80, s, 0x07E0);

        for (int y = 0; y < LCD_HEIGHT; y += PARALLEL_LINES) {
            send_lines(y, &fb[y * LCD_WIDTH]);
            send_line_finish();
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    collecting = 0;

    float sum = 0;
    for (int i = 0; i < sample_index; i++) sum += samples[i];
    float avg = (sample_index > 0) ? sum / sample_index : 0;

    char out[32];
    snprintf(out, sizeof(out), "AVG %.2f", avg);
    xQueueOverwrite(adc_queue, &avg);

    for (int i = 0; i < LCD_WIDTH * LCD_HEIGHT; i++) fb[i] = 0x0000;
    draw_string_buf(fb, 20, 80, out, 0x07E0);

    for (int y = 0; y < LCD_HEIGHT; y += PARALLEL_LINES) {
        send_lines(y, &fb[y * LCD_WIDTH]);
        send_line_finish();
    }
}

void uart_init_bac(QueueHandle_t q)
{
    adc_queue = q;

    uart_config_t cfg = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_driver_install(UART_NUM, 256, 0, 0, NULL, 0);
    uart_param_config(UART_NUM, &cfg);
    uart_set_pin(UART_NUM, UART_TX, UART_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    xTaskCreate(uart_task, "uart_task", 4096, NULL, 5, NULL);
}
