#include "oled.h"
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/i2c_master.h"
#include "ssd1306.h"

#include "buttons/buttons.h"   // button_event_t

#define OLED_I2C_PORT   I2C_NUM_0
#define OLED_SDA_GPIO   GPIO_NUM_19
#define OLED_SCL_GPIO   GPIO_NUM_20
#define OLED_I2C_ADDR   0x3C

typedef enum {
    SEL_START = 0,
    SEL_RESULTS
} menu_sel_t;

typedef struct {
    QueueHandle_t button_q;
} oled_task_params_t;

/* ----------- DRAWING FUNCTIONS ----------- */

static void draw_static(ssd1306_handle_t oled)
{
    // Clear only ONCE at startup
    ssd1306_clear_display(oled, false);

    ssd1306_display_text(oled, 0, "BATT   MQ3   BLe", false);
    ssd1306_display_text(oled, 1, "----------------", false);
}

static void draw_selection(ssd1306_handle_t oled, menu_sel_t sel)
{
    // Redraw ONLY the selectable options
    ssd1306_display_text_x2(oled, 3, "  START ",  (sel == SEL_START));
    ssd1306_display_text_x2(oled, 6, " RESULTS",  (sel == SEL_RESULTS));
}

/* ----------- OLED TASK ----------- */

static void oled_task(void *arg)
{
    oled_task_params_t *p = (oled_task_params_t *)arg;

    /* I2C init */
    i2c_master_bus_handle_t bus;
    i2c_master_bus_config_t bus_cfg = {
        .i2c_port = OLED_I2C_PORT,
        .sda_io_num = OLED_SDA_GPIO,
        .scl_io_num = OLED_SCL_GPIO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .flags.enable_internal_pullup = true,
    };
    i2c_new_master_bus(&bus_cfg, &bus);

    /* OLED init */
    ssd1306_handle_t oled;
    ssd1306_config_t oled_cfg = I2C_SSD1306_128x64_CONFIG_DEFAULT;
    oled_cfg.i2c_address = OLED_I2C_ADDR;
    ssd1306_init(bus, &oled_cfg, &oled);

    /* UI state */
    menu_sel_t sel = SEL_START;

    /* Initial draw */
    draw_static(oled);
    draw_selection(oled, sel);

    /* Event loop */
    while (1) {
        button_event_t ev;

        if (xQueueReceive(p->button_q, &ev, portMAX_DELAY) == pdTRUE) {
            if (ev.type == BUTTON_EVENT_PRESSED) {

                // Toggle selection
                sel = (sel == SEL_START) ? SEL_RESULTS : SEL_START;

                // Update ONLY the two lines
                draw_selection(oled, sel);
            }
        }
    }
}

/* ----------- PUBLIC INIT ----------- */

void oled_init(QueueHandle_t button_q)
{
    static oled_task_params_t params;
    params.button_q = button_q;

    xTaskCreate(oled_task, "oled_task", 4096, &params, 5, NULL);
}
