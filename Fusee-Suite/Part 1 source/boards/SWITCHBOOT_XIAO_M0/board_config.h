#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#define VENDOR_NAME		"Seeed Studio"
#define PRODUCT_NAME		"Seeeduino XIAO"
#define VOLUME_LABEL		"Seeed XIAO"
#define INDEX_URL		"https://www.seeedstudio.com/"

#define BOARD_ID		"SAMD21G18A-XIAO-v0"

#define USB_VID			0x2886
#define USB_PID			0x002F

#define LED_PIN			PIN_PA17
#define LED_TX_PIN		PIN_PA19
#define LED_RX_PIN		PIN_PA18

#define LED_PIN_PULLUP
#define LED_TX_PIN_PULLUP
#define LED_RX_PIN_PULLUP

#define UF2_NAME "XIAO_M0 UF2" // 7 char, space, extn
#define JOYCON_PIN PIN_PA06 //10
#define VOLUP_PIN PIN_PB09 //7
#define USB_PIN PIN_PA10 //2
#define STRAP_HOLD_TIME 75
//#define RESET_BTN_DISABLED
#define HOLD_PIN PIN_PA31
#define HOLD_PIN_PULLUP
#define HOLD_STATE 0
#endif

