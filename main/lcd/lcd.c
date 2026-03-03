#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "decode_image.h"
#include "lcd.h"

#define LCD_HOST    VSPI_HOST

#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  18
#define PIN_NUM_CS   5
#define PIN_NUM_DC   2
#define PIN_NUM_RST  4

// #define LCD_WIDTH  128
// #define LCD_HEIGHT 160
// #define PARALLEL_LINES 8

typedef struct {
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes;
} lcd_init_cmd_t;

typedef enum {
    LCD_TYPE_ILI = 1,
    LCD_TYPE_ST,
} type_lcd_t;

DRAM_ATTR static const lcd_init_cmd_t st_init_cmds[] = {
    {0x36, {0xC8}, 1},
    {0x3A, {0x55}, 1},
    {0x2A, {0x00, 0x00, 0x00, 0x7F}, 4},
    {0x2B, {0x00, 0x00, 0x00, 0x9F}, 4},
    {0xB2, {0x0c, 0x0c, 0x00, 0x33, 0x33}, 5},
    {0xB2, {0x0c, 0x0c, 0x00, 0x33, 0x33}, 5},
    {0xB7, {0x45}, 1},
    {0xBB, {0x2B}, 1},
    {0xC0, {0x2C}, 1},
    {0xC2, {0x01, 0xff}, 2},
    {0xC3, {0x11}, 1},
    {0xC4, {0x20}, 1},
    {0xC6, {0x0f}, 1},
    {0xD0, {0xA4, 0xA1}, 2},
    {0xE0, {0xD0, 0x00, 0x05, 0x0E, 0x15, 0x0D, 0x37, 0x43, 0x47, 0x09, 0x15, 0x12, 0x16, 0x19}, 14},
    {0xE1, {0xD0, 0x00, 0x05, 0x0D, 0x0C, 0x06, 0x2D, 0x44, 0x40, 0x0E, 0x1C, 0x18, 0x16, 0x19}, 14},
    {0x11, {0}, 0x80},
    {0x29, {0}, 0x80},
    {0, {0}, 0xff}
};

static spi_device_handle_t spi_handle;

static void lcd_cmd(uint8_t cmd)
{
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.tx_buffer = &cmd;
    t.user = (void*)0;
    spi_device_polling_transmit(spi_handle, &t);
}

static void lcd_data(const uint8_t *data, int len)
{
    if (len == 0) return;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = len * 8;
    t.tx_buffer = data;
    t.user = (void*)1;
    spi_device_polling_transmit(spi_handle, &t);
}

static void lcd_spi_pre_transfer_callback(spi_transaction_t *t)
{
    gpio_set_level(PIN_NUM_DC, (int)t->user);
}

static uint32_t lcd_get_id()
{
    spi_device_acquire_bus(spi_handle, portMAX_DELAY);
    lcd_cmd(0x04);

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8 * 3;
    t.flags = SPI_TRANS_USE_RXDATA;
    t.user = (void*)1;
    spi_device_polling_transmit(spi_handle, &t);

    spi_device_release_bus(spi_handle);
    return *(uint32_t*)t.rx_data;
}

static void lcd_init_internal()
{
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = ((1ULL << PIN_NUM_DC) | (1ULL << PIN_NUM_RST));
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = true;
    gpio_config(&io_conf);

    gpio_set_level(PIN_NUM_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(PIN_NUM_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(100));

    uint32_t lcd_id = lcd_get_id();
    const lcd_init_cmd_t *lcd_init_cmds = st_init_cmds;

    int cmd = 0;
    while (lcd_init_cmds[cmd].databytes != 0xff) {
        lcd_cmd(lcd_init_cmds[cmd].cmd);
        lcd_data(lcd_init_cmds[cmd].data, lcd_init_cmds[cmd].databytes & 0x1F);
        if (lcd_init_cmds[cmd].databytes & 0x80) {
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        cmd++;
    }
}

void send_lines(int ypos, uint16_t *linedata)
{
    static spi_transaction_t trans[6];
    for (int x = 0; x < 6; x++) {
        memset(&trans[x], 0, sizeof(spi_transaction_t));
        if ((x & 1) == 0) {
            trans[x].length = 8;
            trans[x].user = (void*)0;
        } else {
            trans[x].length = 8 * 4;
            trans[x].user = (void*)1;
        }
        trans[x].flags = SPI_TRANS_USE_TXDATA;
    }

    trans[0].tx_data[0] = 0x2A;
    trans[1].tx_data[0] = 0;
    trans[1].tx_data[1] = 0;
    trans[1].tx_data[2] = (LCD_WIDTH - 1) >> 8;
    trans[1].tx_data[3] = (LCD_WIDTH - 1) & 0xFF;

    trans[2].tx_data[0] = 0x2B;
    int y0 = ypos;
    int y1 = ypos + PARALLEL_LINES - 1;
    trans[3].tx_data[0] = y0 >> 8;
    trans[3].tx_data[1] = y0 & 0xFF;
    trans[3].tx_data[2] = y1 >> 8;
    trans[3].tx_data[3] = y1 & 0xFF;

    trans[4].tx_data[0] = 0x2C;
    trans[5].tx_buffer = linedata;
    trans[5].length = LCD_WIDTH * 2 * 8 * PARALLEL_LINES;
    trans[5].flags = 0;

    for (int x = 0; x < 6; x++) {
        spi_device_queue_trans(spi_handle, &trans[x], portMAX_DELAY);
    }
}

void send_line_finish()
{
    spi_transaction_t *rtrans;
    for (int x = 0; x < 6; x++) {
        spi_device_get_trans_result(spi_handle, &rtrans, portMAX_DELAY);
    }
}

void lcd_display_black(void)
{
    uint16_t *linebuf = spi_bus_dma_memory_alloc(LCD_HOST, LCD_WIDTH * PARALLEL_LINES * 2, 0);
    for (int y = 0; y < LCD_HEIGHT; y += PARALLEL_LINES) {
        for (int i = 0; i < LCD_WIDTH * PARALLEL_LINES; i++) {
            linebuf[i] = 0x0000;
        }
        send_lines(y, linebuf);
        send_line_finish();
    }
}

void lcd_display_image(uint16_t *img)
{
    uint16_t *linebuf = spi_bus_dma_memory_alloc(LCD_HOST, LCD_WIDTH * PARALLEL_LINES * 2, 0);

    for (int y = 0; y < LCD_HEIGHT; y += PARALLEL_LINES) {
        for (int yy = 0; yy < PARALLEL_LINES; yy++) {
            for (int x = 0; x < LCD_WIDTH; x++) {
                int src_y = y + yy;
                if (src_y < IMAGE_H && x < IMAGE_W)
                    linebuf[yy * LCD_WIDTH + x] = img[src_y * IMAGE_W + x];
                else
                    linebuf[yy * LCD_WIDTH + x] = 0x0000;
            }
        }
        send_lines(y, linebuf);
        send_line_finish();
    }
}

void lcd_init_driver(void)
{
    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = PARALLEL_LINES * 160 * 2 + 8
    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 10 * 1000 * 1000,
        .mode = 0,
        .spics_io_num = PIN_NUM_CS,
        .queue_size = 7,
        .pre_cb = lcd_spi_pre_transfer_callback,
    };

    spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO);
    spi_bus_add_device(LCD_HOST, &devcfg, &spi_handle);

    lcd_init_internal();
}

static const uint8_t font5x7[95][5] = {
{0x00,0x00,0x00,0x00,0x00}, // 32 space
{0x00,0x00,0x5F,0x00,0x00}, // !
{0x00,0x07,0x00,0x07,0x00}, // "
{0x14,0x7F,0x14,0x7F,0x14}, // #
{0x24,0x2A,0x7F,0x2A,0x12}, // $
{0x23,0x13,0x08,0x64,0x62}, // %
{0x36,0x49,0x55,0x22,0x50}, // &
{0x00,0x05,0x03,0x00,0x00}, // '
{0x00,0x1C,0x22,0x41,0x00}, // (
{0x00,0x41,0x22,0x1C,0x00}, // )
{0x14,0x08,0x3E,0x08,0x14}, // *
{0x08,0x08,0x3E,0x08,0x08}, // +
{0x00,0x50,0x30,0x00,0x00}, // ,
{0x08,0x08,0x08,0x08,0x08}, // -
{0x00,0x60,0x60,0x00,0x00}, // .
{0x20,0x10,0x08,0x04,0x02}, // /
{0x3E,0x51,0x49,0x45,0x3E}, // 0
{0x00,0x42,0x7F,0x40,0x00}, // 1
{0x62,0x51,0x49,0x49,0x46}, // 2
{0x22,0x49,0x49,0x49,0x36}, // 3
{0x18,0x14,0x12,0x7F,0x10}, // 4
{0x2F,0x49,0x49,0x49,0x31}, // 5
{0x3E,0x49,0x49,0x49,0x32}, // 6
{0x01,0x71,0x09,0x05,0x03}, // 7
{0x36,0x49,0x49,0x49,0x36}, // 8
{0x26,0x49,0x49,0x49,0x3E}, // 9
{0x00,0x36,0x36,0x00,0x00}, // :
{0x00,0x56,0x36,0x00,0x00}, // ;
{0x08,0x14,0x22,0x41,0x00}, // <
{0x14,0x14,0x14,0x14,0x14}, // =
{0x00,0x41,0x22,0x14,0x08}, // >
{0x02,0x01,0x59,0x09,0x06}, // ?
{0x3E,0x41,0x5D,0x59,0x4E}, // @
{0x7E,0x11,0x11,0x11,0x7E}, // A
{0x7F,0x49,0x49,0x49,0x36}, // B
{0x3E,0x41,0x41,0x41,0x22}, // C
{0x7F,0x41,0x41,0x22,0x1C}, // D
{0x7F,0x49,0x49,0x49,0x41}, // E
{0x7F,0x09,0x09,0x09,0x01}, // F
{0x3E,0x41,0x49,0x49,0x7A}, // G
{0x7F,0x08,0x08,0x08,0x7F}, // H
{0x00,0x41,0x7F,0x41,0x00}, // I
{0x20,0x40,0x41,0x3F,0x01}, // J
{0x7F,0x08,0x14,0x22,0x41}, // K
{0x7F,0x40,0x40,0x40,0x40}, // L
{0x7F,0x02,0x04,0x02,0x7F}, // M
{0x7F,0x04,0x08,0x10,0x7F}, // N
{0x3E,0x41,0x41,0x41,0x3E}, // O
{0x7F,0x09,0x09,0x09,0x06}, // P
{0x3E,0x41,0x51,0x21,0x5E}, // Q
{0x7F,0x09,0x19,0x29,0x46}, // R
{0x46,0x49,0x49,0x49,0x31}, // S
{0x01,0x01,0x7F,0x01,0x01}, // T
{0x3F,0x40,0x40,0x40,0x3F}, // U
{0x1F,0x20,0x40,0x20,0x1F}, // V
{0x7F,0x20,0x18,0x20,0x7F}, // W
{0x63,0x14,0x08,0x14,0x63}, // X
{0x03,0x04,0x78,0x04,0x03}, // Y
{0x61,0x51,0x49,0x45,0x43}, // Z
};

static void draw_char_buf(uint16_t *buf, int x, int y, char c, uint16_t color)
{
    if (c < 32 || c > 126) return;

    const uint8_t *g = font5x7[c - 32];

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 7; j++) {
            if (g[i] & (1 << j)) {
                int px = x + i;
                int py = y + j;
                if (px < LCD_WIDTH && py < LCD_HEIGHT)
                    buf[py * LCD_WIDTH + px] = color;
            }
        }
    }
}

void draw_string_buf(uint16_t *buf, int x, int y, const char *s, uint16_t color)
{
    while (*s) {
        draw_char_buf(buf, x, y, *s, color);
        x += 6;
        s++;
    }
}

void lcd_show_bac_screen(void)
{
    uint16_t *fb = heap_caps_malloc(LCD_WIDTH * LCD_HEIGHT * 2, MALLOC_CAP_DMA);

    for (int i = 0; i < LCD_WIDTH * LCD_HEIGHT; i++) {
        fb[i] = 0x0000;
    }

    int cx = (LCD_WIDTH - 3 * 6) / 2;
    int cy = (LCD_HEIGHT / 2) - 20;

    //draw_string_buf(fb, cx, cy, "BAC", 0x001F);
    // draw_string_buf(fb, cx - 6, cy + 20, "12.34567890", 0x001F);

    for (int y = 0; y < LCD_HEIGHT; y += PARALLEL_LINES) {
        send_lines(y, &fb[y * LCD_WIDTH]);
        send_line_finish();
    }

    free(fb);
}

void draw_string_buf(uint16_t *buf, int x, int y, const char *s, uint16_t color);
void send_lines(int ypos, uint16_t *linedata);
void send_line_finish(void);
