#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#define CRYSTALLESS    1

#define VENDOR_NAME "Adafruit Industries"
#define PRODUCT_NAME "Gemma M0"
#define VOLUME_LABEL "FUSEE"
#define INDEX_URL "http://adafru.it/3501"
#define BOARD_ID "SAMD21E18A-Gemma-v0"

#define USB_VID 0x239A
#define USB_PID 0x001C

//#define LED_TX_PIN PIN_PA27
//#define LED_RX_PIN PIN_PB03

//#define BOARD_RGBLED_CLOCK_PIN            PIN_PA01
//#define BOARD_RGBLED_DATA_PIN             PIN_PA00

#define UF2_NAME "GEMMAM0 UF2" // 7 char, space, extn
#define LED_PIN PIN_PA23
#define JOYCON_PIN PIN_PA05
#define VOLUP_PIN PIN_PA04
#define USB_PIN PIN_PA02
#define STRAP_HOLD_TIME 75
//#define RESET_BTN_DISABLED
#define HOLD_PIN PIN_PA31
#define HOLD_PIN_PULLUP
#define HOLD_STATE 0
#endif
