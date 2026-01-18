#include "oled.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "ssd1306.h"


/* CHANGE THESE for each member preference 
"OLED_I2C_PORT   I2C_NUM_0" is not changed */
#define OLED_I2C_PORT   I2C_NUM_0
#define OLED_SDA_GPIO   GPIO_NUM_19
#define OLED_SCL_GPIO   GPIO_NUM_20
#define OLED_I2C_ADDR   0x3C   


static void oled_task(void *arg)
{
    (void)arg;

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

    /* Static only*/
    ssd1306_clear_display(oled, false);

    /*The "BATT   MQ3   BLE" is a concept text holder only where sensor data will be displayed in future*/
    ssd1306_display_text(oled, 0, "BATT   MQ3   BLe", false);
    ssd1306_display_text(oled, 1, "----------------", false);
    ssd1306_display_text_x2(oled, 3, "  START ", false);
    ssd1306_display_text_x2(oled, 6, " RESULTS", false);
    

    vTaskDelete(NULL);
}

void oled_init(void)
{
    xTaskCreate(oled_task, "oled_task", 4096, NULL, 1, NULL);
}
