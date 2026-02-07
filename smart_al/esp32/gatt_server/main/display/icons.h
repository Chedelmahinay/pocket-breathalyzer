#pragma once
#include <stdint.h>

#define ICON_W 16
#define ICON_H 8

/*
  k0i05__esp_ssd1306 bitmap format:
  - Row-major
  - byte_width = (W+7)/8
  - MSB is leftmost pixel in each byte
  For 16x8: byte_width=2, total bytes = 8*2 = 16.
*/

/* Battery outline (empty) 16x8 */
static const uint8_t icon_batt_empty_16x8[16] = {
    0x00, 0x00,  // row 0
    0xFF, 0xF8,  // row 1
    0x80, 0x08,  // row 2
    0x80, 0x0F,  // row 3
    0x80, 0x0F,  // row 4
    0x80, 0x08,  // row 5
    0xFF, 0xF8,  // row 6
    0x00, 0x00   // row 7
};

/* Battery half-filled 16x8 */
static const uint8_t icon_batt_half_16x8[16] = {
    0x00, 0x00,  // row 0
    0xFF, 0xF8,  // row 1
    0xFC, 0x08,  // row 2
    0xFC, 0x0F,  // row 3
    0xFC, 0x0F,  // row 4
    0xFC, 0x08,  // row 5
    0xFF, 0xF8,  // row 6
    0x00, 0x00   // row 7
};

/* Battery full 16x8 */
static const uint8_t icon_batt_full_16x8[16] = {
    0x00, 0x00,  // row 0
    0xFF, 0xF8,  // row 1
    0xFF, 0xF8,  // row 2
    0xFF, 0xFF,  // row 3
    0xFF, 0xFF,  // row 4
    0xFF, 0xF8,  // row 5
    0xFF, 0xF8,  // row 6
    0x00, 0x00   // row 7
};

/*
  MQ3 icon as a droplet (16x8)
  (Simple but readable at 8px tall)
*/
static const uint8_t icon_mq3_16x8[16] = {
    0x06, 0x00,  // row 0  .....##.........
    0x0F, 0x00,  // row 1  ....####........
    0x1F, 0x80,  // row 2  ...######.......
    0x3F, 0xC0,  // row 3  ..########......
    0x7F, 0xE0,  // row 4  .##########.....
    0x7F, 0xE0,  // row 5  .##########.....
    0x3F, 0xC0,  // row 6  ..########......
    0x1F, 0x80   // row 7  ...######.......
};

/*
  BLE icon as WiFi-style arcs (16x8)
  (More readable than a Bluetooth rune at 16x8)
*/
static const uint8_t icon_ble_16x8[16] = {
    0x0F, 0xF0,  // row 0  ....########....
    0x1F, 0xF8,  // row 1  ...##########...
    0x3C, 0x3C,  // row 2  ..####....####..
    0x01, 0x80,  // row 3  .......##.......
    0x03, 0xC0,  // row 4  ......####......
    0x02, 0x40,  // row 5  ......#..#......
    0x01, 0x80,  // row 6  .......##.......
    0x01, 0x80   // row 7  .......##.......
};
