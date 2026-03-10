#include "oled.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "ssd1306.h"
#include "font_latin_8x8.h"
#include "buttons/buttons.h"
#include "results/results.h"
#include "adc/oneshot_read.h"

#define BTN_TOGGLE_GPIO  GPIO_NUM_9
#define BTN_CONFIRM_GPIO GPIO_NUM_4
#define OLED_I2C_PORT   I2C_NUM_0
#define OLED_SDA_GPIO   GPIO_NUM_19
#define OLED_SCL_GPIO   GPIO_NUM_20
#define OLED_I2C_ADDR   0x3C
#define TAG "oled"
#define UI_TICK_MS         250
#define BAC_CHANGE_EPSILON 0.0003f
#define OLED_TEXT_BUF_SZ 16
#define OLED_TEXT_BUF_SZ_2 12
#define OLED_LINE_BUF_SZ 21

typedef enum {
    SEL_START = 0,
    SEL_RESULTS
} menu_sel_t;

typedef enum {
    SCREEN_MENU = 0,
    SCREEN_START,
    SCREEN_RESULTS
} screen_t;

typedef struct {
    QueueHandle_t button_q;
    QueueHandle_t adc_q;   
} oled_task_params_t;

static void clear_content_area(ssd1306_handle_t oled)
{
    for (int row = 0; row <= 7; row++) {
        ssd1306_display_text(oled, row, "                ", false); 
    }
}

static void draw_menu_selection_only(ssd1306_handle_t oled, menu_sel_t sel)
{
    ssd1306_display_text_x2(oled, 2, "  START ",  (sel == SEL_START));
    ssd1306_display_text_x2(oled, 5, " RESULTS", (sel == SEL_RESULTS));
}

static void draw_bac_centered_x2(ssd1306_handle_t oled, float bac)
{
    char num[12];
    snprintf(num, sizeof(num), "  %0.3f", (double)bac);
    int len = strlen(num);
    ssd1306_display_text_x2(oled, 3, num, false);
}

static void draw_start_screen_content(ssd1306_handle_t oled)
{
    clear_content_area(oled);
    draw_bac_centered_x2(oled, 0.0f);
}

static void draw_results_screen_content(ssd1306_handle_t oled)
{
    clear_content_area(oled);
    results_snapshot_t snapshot;
    
    if (!results_get_snapshot(&snapshot) || snapshot.count <= 0) {
        return;
    }

    ssd1306_display_text(oled, 1, "Max 3 Peaks:", false);

    char line[OLED_LINE_BUF_SZ];
    int max_rows = (snapshot.count < RESULTS_MAX_COUNT) ? snapshot.count : RESULTS_MAX_COUNT;

    for (int i = 0; i < max_rows && i < 3; i++) {
        snprintf(line, sizeof(line), "%.3f", (double)snapshot.values[i]);
        ssd1306_display_text(oled, 3 + i, line, false);
    }
}

static void oled_task(void *arg)
{
    oled_task_params_t *p = (oled_task_params_t *)arg;

    i2c_master_bus_handle_t bus;
    i2c_master_bus_config_t bus_cfg = {
        .i2c_port = OLED_I2C_PORT,
        .sda_io_num = OLED_SDA_GPIO,
        .scl_io_num = OLED_SCL_GPIO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .flags.enable_internal_pullup = true,
    };

    if (i2c_new_master_bus(&bus_cfg, &bus) != ESP_OK) {
        vTaskDelete(NULL);
        return;
    }

    ssd1306_handle_t oled;
    ssd1306_config_t oled_cfg = I2C_SSD1306_128x64_CONFIG_DEFAULT;
    oled_cfg.i2c_address = OLED_I2C_ADDR;

    if (ssd1306_init(bus, &oled_cfg, &oled) != ESP_OK) {
        vTaskDelete(NULL);
        return;
    }

    ssd1306_clear_display(oled, false);

    screen_t screen = SCREEN_MENU;
    menu_sel_t sel = SEL_START;

    clear_content_area(oled);
    draw_menu_selection_only(oled, sel);

    const TickType_t tick = pdMS_TO_TICKS(UI_TICK_MS);

    float last_bac = -999.0f;
    bool have_bac = false;

    while (1) {
        button_event_t ev;

        if (xQueueReceive(p->button_q, &ev, tick) == pdTRUE) {
            switch (screen) {
                case SCREEN_MENU:
                    if (ev.gpio == BTN_TOGGLE_GPIO) {
                        sel = (sel == SEL_START) ? SEL_RESULTS : SEL_START;
                        draw_menu_selection_only(oled, sel);
                    } else if (ev.gpio == BTN_CONFIRM_GPIO) {
                        if (sel == SEL_START) {
                            adc_reset_session_max();
                            screen = SCREEN_START;
                            draw_start_screen_content(oled);
                            last_bac = -999.0f;
                            have_bac = false;
                        } else {
                            screen = SCREEN_RESULTS;
                            draw_results_screen_content(oled);
                        }
                    }
                    break;

                case SCREEN_START:
                    if (ev.gpio == BTN_CONFIRM_GPIO) {
                        adc_commit_session_max();
                        screen = SCREEN_MENU;
                        clear_content_area(oled);
                        draw_menu_selection_only(oled, sel);
                    }
                    break;

                case SCREEN_RESULTS:
                    if (ev.gpio == BTN_CONFIRM_GPIO) {
                        screen = SCREEN_MENU;
                        clear_content_area(oled);
                        draw_menu_selection_only(oled, sel);
                    }
                    break;

                default:
                    screen = SCREEN_MENU;
                    clear_content_area(oled);
                    draw_menu_selection_only(oled, sel);
                    break;
            }
        } else {
            switch (screen) {
                case SCREEN_START:
                    if (p->adc_q) {
                        float bac_now;
                        if (xQueuePeek(p->adc_q, &bac_now, 0) == pdTRUE) {
                            if (!have_bac || fabsf(bac_now - last_bac) > BAC_CHANGE_EPSILON) {
                                draw_bac_centered_x2(oled, bac_now);
                                last_bac = bac_now;
                                have_bac = true;
                            }
                        }
                    }
                    break;

                case SCREEN_RESULTS:
                    break;
                case SCREEN_MENU:
                    break;
            }
        }
    }
}

void oled_init(QueueHandle_t button_q, QueueHandle_t adc_q)
{
    static oled_task_params_t params;
    params.button_q = button_q;
    params.adc_q    = adc_q;
    xTaskCreate(oled_task, "oled_task", 4096, &params, 5, NULL);
}