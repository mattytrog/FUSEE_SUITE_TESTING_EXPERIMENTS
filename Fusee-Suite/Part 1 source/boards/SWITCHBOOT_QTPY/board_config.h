#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#define CRYSTALLESS    1

#define VENDOR_NAME "Adafruit Industries"
#define PRODUCT_NAME "QT Py M0"
#define VOLUME_LABEL "FUSEE"
#define INDEX_URL "http://adafru.it/4600"
#define BOARD_ID "SAMD21E18A-QTPy-v0"

#define USB_VID 0x239A
#define USB_PID 0x00CB

//#define LED_TX_PIN PIN_PA27
//#define LED_RX_PIN PIN_PB03
#define BOARD_NEOPIXEL_POWERPIN PIN_PA15
#define BOARD_NEOPIXEL_PIN PIN_PA18
#define BOARD_NEOPIXEL_COUNT 1

#define UF2_NAME "QTPY_M0 UF2" // 7 char, space, extn
//#define LED_PIN PIN_PA10
#define JOYCON_PIN PIN_PA02
#define VOLUP_PIN PIN_PA05
#define USB_PIN PIN_PA03
#define STRAP_HOLD_TIME 75
//#define RESET_BTN_DISABLED
#define HOLD_PIN PIN_PA31
#define HOLD_PIN_PULLUP
#define HOLD_STATE 0
#endif
