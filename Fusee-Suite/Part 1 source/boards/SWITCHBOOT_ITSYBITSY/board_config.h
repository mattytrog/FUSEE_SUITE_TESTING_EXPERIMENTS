#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#define CRYSTALLESS    1

#define VENDOR_NAME "Adafruit Industries"
#define PRODUCT_NAME "ItsyBitsy M0 Express"
#define VOLUME_LABEL "FUSEE"
#define INDEX_URL "http://adafru.it/3727"
#define BOARD_ID "SAMD21G18A-ItsyBitsy-v0"

#define USB_VID 0x239A
#define USB_PID 0x000F




//#define BOARD_RGBLED_CLOCK_PIN            PIN_PA00
//#define BOARD_RGBLED_DATA_PIN             PIN_PA01

#define UF2_NAME "ITSYBIT UF2" // 7 char, space, extn
#define LED_PIN PIN_PA17
#define JOYCON_PIN PIN_PA07
#define VOLUP_PIN PIN_PA16
#define USB_PIN PIN_PA21
#define STRAP_HOLD_TIME 75
//#define RESET_BTN_DISABLED
#define HOLD_PIN PIN_PA31
#define HOLD_PIN_PULLUP
#define HOLD_STATE 0
#endif
