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
#include "icons.h"
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

static int  g_batt_percent = 87;
static bool g_mq3_ready    = true;
static bool g_ble_ready    = false;

static void draw_text_at(ssd1306_handle_t oled, uint8_t page, uint8_t seg, const char *text, bool invert)
{
    uint8_t img[8];

    for (int i = 0; text[i] != '\0'; i++) {
        memcpy(img, font_latin_8x8_tr[(uint8_t)text[i]], 8);

        if (invert) {
            for (int k = 0; k < 8; k++) img[k] = (uint8_t)~img[k];
        }

        ssd1306_display_image(oled, page, seg, img, 8);
        seg += 8;
        if (seg >= 128) 
        break;
    }
}

static const uint8_t *select_batt_icon(int batt_percent)
{
    if (batt_percent < 0) batt_percent = 0;
    if (batt_percent > 100) batt_percent = 100;

    if (batt_percent >= 70) return icon_batt_full_16x8;
    if (batt_percent >= 30) return icon_batt_half_16x8;
    return icon_batt_empty_16x8;
}

static void draw_top_bar_static(ssd1306_handle_t oled)
{
    const uint8_t *batt_icon = select_batt_icon(g_batt_percent);

    ssd1306_display_bitmap(oled, 0, 0, batt_icon, ICON_W, ICON_H, false);
    ssd1306_display_bitmap(oled, 52, 0, icon_mq3_16x8, ICON_W, ICON_H, false);
    ssd1306_display_bitmap(oled, 92, 0, icon_ble_16x8, ICON_W, ICON_H, false);

    char btxt[8];
    snprintf(btxt, sizeof(btxt), "%d%%", g_batt_percent);
    draw_text_at(oled, 0, 18, btxt, false);

    draw_text_at(oled, 0, 70, g_mq3_ready ? "OK" : "X", false);
    draw_text_at(oled, 0, 110, g_ble_ready ? "OK" : "X", false);

    ssd1306_display_text(oled, 1, "----------------", false);
}

static void clear_content_area(ssd1306_handle_t oled)
{
    for (int row = 2; row <= 7; row++) {
        ssd1306_display_text(oled, row, "                ", false); 
    }
}

static void draw_menu_selection_only(ssd1306_handle_t oled, menu_sel_t sel)
{
    ssd1306_display_text_x2(oled, 3, "  START ",  (sel == SEL_START));
    ssd1306_display_text_x2(oled, 6, " RESULTS", (sel == SEL_RESULTS));
}

static void make_centered_x2(char *out, size_t out_sz, const char *txt)
{
    const int cols = 8;
    int len = (int)strlen(txt);
    if (len > cols) len = cols;

    int lead = (cols - len) / 2;
    int idx = 0;

    for (int i = 0; i < lead && idx < (int)out_sz - 1; i++) out[idx++] = ' ';
    for (int i = 0; i < len  && idx < (int)out_sz - 1; i++) out[idx++] = txt[i];
    while (idx < cols && idx < (int)out_sz - 1) out[idx++] = ' ';

    out[idx] = '\0';
}

static void draw_bac_centered_x2(ssd1306_handle_t oled, float bac)
{ 
    char num[OLED_TEXT_BUF_SZ_2];
    snprintf(num, sizeof(num), "%0.3f", (double)bac);

    char line[OLED_TEXT_BUF_SZ];
    make_centered_x2(line, sizeof(line), num);
    ssd1306_display_text_x2(oled, 4, line, false);
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

    ssd1306_display_text(oled, 2, "Max 3 Peaks:", false);

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
        ESP_LOGE(TAG, "Failed to create I2C master bus");
        vTaskDelete(NULL);
        return;
    }

    ssd1306_handle_t oled;
    ssd1306_config_t oled_cfg = I2C_SSD1306_128x64_CONFIG_DEFAULT;
    oled_cfg.i2c_address = OLED_I2C_ADDR;

    if (ssd1306_init(bus, &oled_cfg, &oled) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SSD1306");
        vTaskDelete(NULL);
        return;
    }

    ssd1306_clear_display(oled, false);
    draw_top_bar_static(oled);

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