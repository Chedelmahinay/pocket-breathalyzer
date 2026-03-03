idf_component_register(
    SRCS
        "main.c"
        "lcd/lcd.c"
        "lcd/decode_image.c"
        "uart/uart.c"
        "ble/gatt_server.c"
    INCLUDE_DIRS
        "."
        "lcd"
        "uart"
        "ble"
    EMBED_FILES
        "lcd/cat.jpg"
        "lcd/cat2.jpg"
        "lcd/menu.jpg"
)

