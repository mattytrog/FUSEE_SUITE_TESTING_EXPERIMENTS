#include <Arduino.h>


//When EEPROM_DUAL_BOOT_TOGGLE = 1, normal dual-boot(Hold VOL+ to boot into RCM / Fusee)

//When EEPROM_DUAL_BOOT_TOGGLE = 2, ChipRCM is active - PERMANENT autoRCM (chipRCM is autoRCM triggered by the chip) - 
    //so if plugged into USB when attempting to boot, RCM mode will be active and no boot(black screen).

//When EEPROM_DUAL_BOOT_TOGGLE = 3, Failsafe RCM is active. This is PERMANENT autoRCM, IF USB PORT HAS ZERO VOLTS - 
    //so if you are plugged into USB when attempting to boot, the chip functions will be bypassed and a normal OFW boot will occur.
    //IF YOU ARE SAVING YOUR FUSES, NEVER USE THIS. THIS WILL BURN YOUR FUSES. This mode was designed so its impossible to
    //get stuck in RCM mode, flattening battery.
    //REMEMBER! Most units will NOT BOOT CFW from a chip if USB is plugged into a power source! We normally have to unplug
    //or pull unit out of dock to boot CFW

// 1 = Dual-boot / ChipRCM.   2 = Dual-boot / FailsafeRCM   3 = Dual-boot / ChipRCM / FailsafeRCM
#define BOOT_OPTIONS_AVAILABLE 1

//All options disabled. Updateable via SETTINGS.UF2. Long-press for 20 seconds for UF2 mode. 1 = Settings Disabled. 0 = Enabled
#define VOLUP_SETTINGS_DISABLED 0

//Uncomment chosen chip and CHANGE CHIP in Tools menu under "Board"
#define TRINKET
//#define QTPY
//#define XIAO
//#define REBUG
//#define RCMX86_INTERNAL
//#define GEMMA
//#define ITSYBITSY
//#define FEATHER

//NOT_BUILT_BY_DEFAULT
//#define TRINKETMETHOD3
//#define TRINKETLEGACY3
//#define RCMX86
//#define GENERIC_TRINKET_DONGLE
//#define GENERIC_GEMMA_DONGLE

#define DEFAULT_DOTSTAR_BRIGHTNESS 180
#define DEFAULT_JOYCON 0
#define DEFAULT_VOLUME 0
#define DEFAULT_COLOUR 1
#define DEFAULT_SETTINGS_CHANGE 0 //0 = Long-press / 1 = Multiple-press
#define STORAGE_ARRAY 1 // 0 = Old EEPROM array list / 1 = New EEPROM array list

//Time in mS to select an option with rapid-press in Chip Assistant
#define SELECTION_RELEASE_TIME 200
#define SELECTION_CONFIRM_TIME 200
#define TIME_TUNE 30 // Compensation for timing errors. Worked out manually
#define SAFETY_PRESS_BARRIER 5 // How many "non-presses" before you get to your first setting
#define BUTTON_GAP 2 // Gap to leave between options
#define OPTIONS_TO_SHOW 6 // How many options to show

#define SETTINGSMENU_HOLD_TIME 125 //Time to hold VOL+ to enter menu
#define BOOTLOGO_DISP_TIME 100 //Time to display Nintendo bootlogo
#define SETTINGS_TIMEOUT 34 // Chip setup assistant timeout. It times out because we don't want accidental presses when console is operating

#define PACKET_CHUNK_SIZE 0x1000 //Size of packet parts transported to higher / lower buffer

//You can now specify custom pins if you don't like the defaults, for all functions. This is done as follows:
//Defines with a _PA at the end (ie JOYCON_STRAP_PIN_PA, VOLUP_STRAP_PIN_PA) can be uncommented and the
//PA pin number (from the Adafruit website/SAMD21 datasheet etc etc) can be entered here.

//Lets say for example, you wanted to change all your pins on your Trinket to be "neater" on the motherboard.
//We could change Trinket pin 2 to be VOLUP strap/pin, pin 0 to be joycon and pin 4 to be USB strap/pin...
//First, we need the PA numbers from the Adafruit website. We find that:
//Adafruit / Arduino Pin 0 is PA08 (will be JOYCON)
//Adafruit / Arduino Pin 2 is PA09 (will be VOLUP)
//Adafruit / Arduino Pin 4 is PA06 (will be USB)

//EXAMPLE: So if we do this (all uncommented obviously)
//////////
//////////  #ifdef TRINKET
//////////  #include <Usb.h>
//////////  #define DOTSTAR_ENABLED 1
//////////  #define JOYCON_STRAP_PIN_PA 8 //PA08
//////////  #define VOLUP_STRAP_PIN_PA 9 //PA09
//////////  #define USB_LOW_RESET_PA 6 //PA06
//////////  #define JOYCON_STRAP_PIN 0 // Match these up to Arduino pin numbers
//////////  #define VOLUP_STRAP_PIN 2 // Match these up to Arduino pin numbers
//////////  #define USB_LOW_RESET 4 // Match these up to Arduino pin numbers
//////////  #define ONBOARD_LED 13
//////////  #endif

//We have created custom pins, that will be saved to EEPROM and will be read by the Part 1 FUSEE bootloader. And the
//custom straps activated.
//If specifying PA pins, it isn't mandatory to define the matched Arduino numbered pins... Keep reading

//If you do NOT specify arduino matched pins, no strap "checking" will take place and the older USB reboot code will not work
//I repeat: If no Arduino numbered pins (ie JOYCON_STRAP_PIN etc) are defined,
//Strap checking will be replaced with a generic warning in the Fusee_UF2 info screen - and the old "USB Reboot" code will not work.
//FailsafeRCM will work however(which sleeps the chip if USB is plugged in during boot, making a OFW boot take place).
//If you want to just use the default pins (in my diagrams - I know they arent ideal. But I did them many years ago. And cannot
//break compatibility for all users), just ensure all _PA lines are commented, with a //.


//Board definitions
#ifdef TRINKET
#include <Usb.h>
#define DOTSTAR_ENABLED 1
#define JOYCON_STRAP_PIN_PA 7 //PA07
#define VOLUP_STRAP_PIN_PA 8 //PA08
#define USB_LOW_RESET_PA 9 //PA09
#define JOYCON_STRAP_PIN 3 //Is PA07
#define VOLUP_STRAP_PIN 0 //Is PA08
#define USB_LOW_RESET 2 //Is PA09
#define ONBOARD_LED 13
#endif

#ifdef QTPY
#include <Usb.h>
#define NEOPIXEL_ENABLED 1
//#define JOYCON_STRAP_PIN_PA 
//#define VOLUP_STRAP_PIN_PA 
//#define USB_LOW_RESET_PA 
#define JOYCON_STRAP_PIN 0 
#define VOLUP_STRAP_PIN 3
#define USB_LOW_RESET 1
#define BOARD_NEOPIXEL_PIN 11
#define BOARD_NEOPIXEL_COUNT 1
#endif

//Board definitions
#ifdef XIAO
#include <Usb.h>
//#define DOTSTAR_ENABLED 1
//#define JOYCON_STRAP_PIN_PA 
//#define VOLUP_STRAP_PIN_PA 
//#define USB_LOW_RESET_PA 
#define JOYCON_STRAP_PIN 10 
#define VOLUP_STRAP_PIN 7
#define USB_LOW_RESET 2
#define ALL_LEDS_ACTIVE // Repurpose TX/RX leds to be additional indicator LEDS. Comment out to switch them off, leaving normal (pin 13) LED as only LED available
#define ONBOARD_LED 13
#define TXLED 11
#define RXLED 12
#endif

#ifdef REBUG
#include <Usb.h>
//#define DOTSTAR_ENABLED 1
//#define JOYCON_STRAP_PIN_PA 
//#define VOLUP_STRAP_PIN_PA 
//#define USB_LOW_RESET_PA 
#define JOYCON_STRAP_PIN 3 
#define VOLUP_STRAP_PIN 0
#define USB_LOW_RESET 2
#define ONBOARD_LED 13
#endif

#ifdef GEMMA
#include <Usb.h>
#define DOTSTAR_ENABLED 1
//#define JOYCON_STRAP_PIN_PA 
//#define VOLUP_STRAP_PIN_PA 
//#define USB_LOW_RESET_PA 
#define JOYCON_STRAP_PIN 2 
#define VOLUP_STRAP_PIN 0
#define USB_LOW_RESET 1
#define ONBOARD_LED 13
#endif

#ifdef ITSYBITSY
#include <Usb.h>
#define DOTSTAR_ENABLED 1
//#define JOYCON_STRAP_PIN_PA 
//#define VOLUP_STRAP_PIN_PA 
//#define USB_LOW_RESET_PA 
#define JOYCON_STRAP_PIN 9  
#define VOLUP_STRAP_PIN 11
#define USB_LOW_RESET 7
#define INTERNAL_DS_DATA 41
#define INTERNAL_DS_CLK 40
#define ONBOARD_LED 13
#endif

#ifdef FEATHER
#include <Usb.h>
#define DOTSTAR_ENABLED 1
//#define JOYCON_STRAP_PIN_PA 
//#define VOLUP_STRAP_PIN_PA 
//#define USB_LOW_RESET_PA 
#define JOYCON_STRAP_PIN 9 
#define VOLUP_STRAP_PIN 11
#define USB_LOW_RESET 6
#define INTERNAL_DS_DATA 41
#define INTERNAL_DS_CLK 40
#define ONBOARD_LED 13
#endif

#ifdef RCMX86_INTERNAL
#include <Usb.h>
//#define JOYCON_STRAP_PIN_PA 
//#define VOLUP_STRAP_PIN_PA 
//#define USB_LOW_RESET_PA 
#define JOYCON_STRAP_PIN 2 
#define VOLUP_STRAP_PIN 1
#define USB_LOW_RESET 4
#define USB_LOGIC 3
#define ONBOARD_LED 0
#endif

//#ifdef TRINKETMETHOD3
//#define DOTSTAR_ENABLED 1
//#define USB_LOW_RESET 4
//#define WAKEUP_PIN 2
//#define JOYCON_STRAP_PIN 3      
//#define VOLUP_STRAP_PIN 0
//#define ONBOARD_LED 13
//#endif
//
//#ifdef TRINKETLEGACY3
//#define DOTSTAR_ENABLED 1
//#define USB_LOW_RESET 2
//#define WAKEUP_PIN 4
//#define JOYCON_STRAP_PIN 3
//#define VOLUP_STRAP_PIN 0
//#define ONBOARD_LED 13
//#endif
