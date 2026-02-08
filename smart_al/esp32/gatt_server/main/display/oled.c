#include "oled.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/i2c_master.h"
#include "esp_log.h"

#include "ssd1306.h"
#include "font_latin_8x8.h"

#include "buttons/buttons.h"
#include "icons.h"

#define OLED_I2C_PORT   I2C_NUM_0
#define OLED_SDA_GPIO   GPIO_NUM_19
#define OLED_SCL_GPIO   GPIO_NUM_20
#define OLED_I2C_ADDR   0x3C

#define BTN_TOGGLE_GPIO  GPIO_NUM_9
#define BTN_CONFIRM_GPIO GPIO_NUM_4

#define TAG "oled"

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
} oled_task_params_t;

// Top bar Values
static int  g_batt_percent = 87;
static bool g_mq3_ready    = true;
static bool g_ble_ready    = false;

// Draw 8x8 font
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
        if (seg >= 128) break;
    }
}

// battery icon logic // placeholder to be replaced
static const uint8_t *select_batt_icon(int batt_percent)
{
    if (batt_percent < 0) batt_percent = 0;
    if (batt_percent > 100) batt_percent = 100;

    if (batt_percent >= 70) return icon_batt_full_16x8;
    if (batt_percent >= 30) return icon_batt_half_16x8;
    return icon_batt_empty_16x8;
}

// Top bar icons and value placeholders draw
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

// Clear content area but Top bar stays
static void clear_content_area(ssd1306_handle_t oled)
{

    for (int row = 2; row <= 7; row++) {
        ssd1306_display_text(oled, row, "                ", false); 
    }
}

static void draw_menu_static(ssd1306_handle_t oled)
{
    clear_content_area(oled);
}

static void draw_menu_selection_only(ssd1306_handle_t oled, menu_sel_t sel)
{
    ssd1306_display_text_x2(oled, 3, "  START ",  (sel == SEL_START));
    ssd1306_display_text_x2(oled, 6, " RESULTS", (sel == SEL_RESULTS));
}

// Content screen below divider
static void draw_start_screen_content(ssd1306_handle_t oled)
{
    clear_content_area(oled);
    ssd1306_display_text(oled, 3, "START SCREEN", false);
    ssd1306_display_text(oled, 5, "GPIO4 = back", false);
}

static void draw_results_screen_content(ssd1306_handle_t oled)
{
    clear_content_area(oled);
    ssd1306_display_text(oled, 3, "RESULTS SCREEN", false);
    ssd1306_display_text(oled, 5, "GPIO4 = back", false);
}

// OLED
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
    i2c_new_master_bus(&bus_cfg, &bus);

    ssd1306_handle_t oled;
    ssd1306_config_t oled_cfg = I2C_SSD1306_128x64_CONFIG_DEFAULT;
    oled_cfg.i2c_address = OLED_I2C_ADDR;
    ssd1306_init(bus, &oled_cfg, &oled);

    // static draw
    ssd1306_clear_display(oled, false);
    draw_top_bar_static(oled);

    screen_t screen = SCREEN_MENU;
    menu_sel_t sel = SEL_START;

    // Menu init (below divider only)
    draw_menu_static(oled);
    draw_menu_selection_only(oled, sel);

    ESP_LOGI(TAG, "UI ready. GPIO9 toggles (menu), GPIO4 confirms/back.");

    while (1) {
        button_event_t ev;

        if (xQueueReceive(p->button_q, &ev, portMAX_DELAY) == pdTRUE) {
            if (ev.type != BUTTON_EVENT_PRESSED) continue;

            if (screen == SCREEN_MENU) {
                if (ev.gpio == BTN_TOGGLE_GPIO) {
                    sel = (sel == SEL_START) ? SEL_RESULTS : SEL_START;
                    draw_menu_selection_only(oled, sel);
                } else if (ev.gpio == BTN_CONFIRM_GPIO) {
                    // keep static
                    if (sel == SEL_START) {
                        screen = SCREEN_START;
                        draw_start_screen_content(oled);
                    } else {
                        screen = SCREEN_RESULTS;
                        draw_results_screen_content(oled);
                    }
                }
            } else {
                // GPIO4 back to menu
                if (ev.gpio == BTN_CONFIRM_GPIO) {
                    screen = SCREEN_MENU;
                    draw_menu_static(oled);
                    draw_menu_selection_only(oled, sel);
                }
            }
        }
    }
}

void oled_init(QueueHandle_t button_q)
{
    static oled_task_params_t params;
    params.button_q = button_q;

    xTaskCreate(oled_task, "oled_task", 4096, &params, 5, NULL);
}