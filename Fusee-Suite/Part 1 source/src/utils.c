#include "uf2.h"
#include "neopixel.h"

static uint32_t timerLow;
uint32_t timerHigh, resetHorizon;

void __attribute__ ((noinline)) delay(uint32_t ms) {
    // These multipliers were determined empirically and are only approximate.
    // After the pulsing LED is enabled (led_tick_on), the multipliers need to change
    // due to the interrupt overhead of the pulsing.
    // SAMD21 starts up at 1MHz by default.
#ifdef SAMD21
    uint32_t count = ms * (current_cpu_frequency_MHz) * (167);
#endif
#ifdef SAMD51
    // SAMD51 starts up at 48MHz by default, and we set the clock to
    // 48MHz, so we don't need to adjust for current_cpu_frequency_MHz.
    uint32_t count = ms * (led_tick_on ? 6353 : 6826);
#endif
    for (uint32_t i = 1; i < count; ++i) {
        asm volatile("");
    }
}

// Useful for debugging.
// PIN_PA19 is D12 on Metro M0, D11 on Metro M4
#ifdef BLINK_DEBUG
void blink_n(uint32_t pin, uint32_t n, uint32_t interval) {
    // Start out off.
    PINOP(pin, DIRSET);
    PINOP(pin, OUTCLR);
    delay(interval);
    for (int i = 0; i < n; ++i) {
        PINOP(pin, OUTSET);
        delay(interval);
        PINOP(pin, OUTCLR);
        delay(interval);
    }
}

void blink_n_forever(uint32_t pin, uint32_t n, uint32_t interval) {
    while(1) {
        blink_n(pin, n, interval);
        delay(interval*5);
    }
}
#endif

void timerTick(void) {
    if (timerLow-- == 0) {
        timerLow = TIMER_STEP;
        timerHigh++;
        if (resetHorizon && timerHigh >= resetHorizon) {
            resetHorizon = 0;
            resetIntoApp();
        }
    }
}

void panic(int code) {
    logval("PANIC", code);
    while (1) {
    }
}

int writeNum(char *buf, uint32_t n, bool full) {
    int i = 0;
    int sh = 28;
    while (sh >= 0) {
        int d = (n >> sh) & 0xf;
        if (full || d || sh == 0 || i) {
            buf[i++] = d > 9 ? 'A' + d - 10 : '0' + d;
        }
        sh -= 4;
    }
    return i;
}

void resetIntoApp() {
    // reset without waiting for double tap (only works for one reset)
    RGBLED_set_color(COLOR_LEAVE);
    *DBL_TAP_PTR = DBL_TAP_MAGIC_QUICK_BOOT;
    NVIC_SystemReset();
}

void resetIntoBootloader() {
    // reset without waiting for double tap (only works for one reset)
    *DBL_TAP_PTR = DBL_TAP_MAGIC;
    NVIC_SystemReset();
}

#if USE_LOGS
struct LogStore logStoreUF2;

void logreset() {
    logStoreUF2.ptr = 0;
    logmsg("Reset logs.");
}

void logwritenum(uint32_t n) {
    char buff[9];
    buff[writeNum(buff, n, false)] = 0;
    logwrite("0x");
    logwrite(buff);
}

void logwrite(const char *msg) {
    const int jump = sizeof(logStoreUF2.buffer) / 4;
    if (logStoreUF2.ptr >= sizeof(logStoreUF2.buffer) - jump) {
        logStoreUF2.ptr -= jump;
        memmove(logStoreUF2.buffer, logStoreUF2.buffer + jump, logStoreUF2.ptr);
    }
    int l = strlen(msg);
    if (l + logStoreUF2.ptr >= sizeof(logStoreUF2.buffer)) {
        logwrite("TOO LONG!\n");
        return;
    }
    memcpy(logStoreUF2.buffer + logStoreUF2.ptr, msg, l);
    logStoreUF2.ptr += l;
    logStoreUF2.buffer[logStoreUF2.ptr] = 0;
}

void logmsg(const char *msg) {
    logwrite(msg);
    logwrite("\n");
}

void logval(const char *lbl, uint32_t v) {
    logwrite(lbl);
    logwrite(": ");
    logwritenum(v);
    logwrite("\n");
}
#endif

void led_tick() {
    // if(led_tick_on)
	// {
		// LED_MSC_ON();
		// led_tick_on = false;
	// }
	// else
	// {
		// LED_MSC_OFF();
		// led_tick_on = true;
	// }
//LED_MSC_ON();	
}

/* void led_tick() {
    led_tick_on = true;
    now++;
    if (signal_end) {
        if (now == signal_end - 1000) {
            LED_MSC_ON();
        }
        if (now == signal_end) {
            signal_end = 0;
        }
    } else {
        uint8_t curr = now & 0xff;
        if (curr == 0) {
            LED_MSC_ON();
            if (limit < 10 || limit > 250) {
                led_tick_step = -led_tick_step;
            }
            limit += led_tick_step;
        } else if (curr == limit) {
            LED_MSC_OFF();
        }
    }
} */

void led_signal() {
//LED_MSC_OFF();
}

void led_init() {
#if defined(LED_PIN)
    PINOP(LED_PIN, DIRSET);
#endif
    LED_MSC_OFF();

#if defined(BOARD_RGBLED_CLOCK_PIN)
    // using APA102, set pins to outputs
    PINOP(BOARD_RGBLED_CLOCK_PIN, DIRSET);
    PINOP(BOARD_RGBLED_DATA_PIN, DIRSET);

    // This won't work for neopixel, because we're running at 1MHz or thereabouts...
    RGBLED_set_color(COLOR_LEAVE);
#endif

#if USE_SCREEN
    // turn display backlight
    screen_early_init();
#endif

#if defined(LED_TX_PIN)
    PINOP(LED_TX_PIN, DIRSET);
    LED_TX_OFF();
#endif

#if defined(LED_RX_PIN)
    PINOP(LED_RX_PIN, DIRSET);
    LED_RX_OFF();
#endif

#if defined(BOARD_NEOPIXEL_POWERPIN)
    PINOP(BOARD_NEOPIXEL_POWERPIN, DIRSET);
    PINOP(BOARD_NEOPIXEL_POWERPIN, OUTSET);
#endif
}

#if defined(BOARD_RGBLED_CLOCK_PIN)
void write_apa_byte(uint8_t x) {
    for (uint8_t i = 0x80; i != 0; i >>= 1) {
        if (x & i)
            PINOP(BOARD_RGBLED_DATA_PIN, OUTSET);
        else
            PINOP(BOARD_RGBLED_DATA_PIN, OUTCLR);

        PINOP(BOARD_RGBLED_CLOCK_PIN, OUTSET);
        // for (uint8_t j=0; j<25; j++) /* 0.1ms */
        //  __asm__ __volatile__("");

        PINOP(BOARD_RGBLED_CLOCK_PIN, OUTCLR);
        // for (uint8_t j=0; j<25; j++) /* 0.1ms */
        //  __asm__ __volatile__("");
    }
}
#endif

void RGBLED_set_color(uint32_t color) {
	LED_MSC_ON();
#if defined(BOARD_RGBLED_CLOCK_PIN)
    write_apa_byte(0x0);
    write_apa_byte(0x0);
    write_apa_byte(0x0);
    write_apa_byte(0x0);

    write_apa_byte(0xFF);
    write_apa_byte(color >> 16);
    write_apa_byte(color >> 8);
    write_apa_byte(color);

    write_apa_byte(0xFF);
    write_apa_byte(0xFF);
    write_apa_byte(0xFF);
    write_apa_byte(0xFF);

    // set clock port low for ~10ms
    delay(50);
#elif defined(BOARD_NEOPIXEL_PIN)
    uint8_t buf[BOARD_NEOPIXEL_COUNT * 3];
#if 0
    memset(buf, 0, sizeof(buf));
    buf[0] = color >> 8;
    buf[1] = color >> 16;
    buf[2] = color;
#else
    for (int i = 0; i < BOARD_NEOPIXEL_COUNT * 3; i += 3) {
        buf[i + 0] = color >> 8;
        buf[i + 1] = color >> 16;
        buf[i + 2] = color;
    }
#endif
    neopixel_send_buffer(buf, BOARD_NEOPIXEL_COUNT * 3);
#endif
}


#define PAGE_00 0x3FC00
#define PAGE_01 0x3FC40
#define PAGE_02 0x3FC80
#define PAGE_03 0x3FCC0
#define PAGE_04 0x3FD00
#define PAGE_05 0x3FD40
#define PAGE_06 0x3FD80
#define PAGE_07 0x3FDC0
#define PAGE_08 0x3FE00
#define PAGE_09 0x3FE40
#define PAGE_10 0x3FE80
#define PAGE_11 0x3FEC0
#define PAGE_12 0x3FF00
#define PAGE_13 0x3FF40
#define PAGE_14 0x3FF80
#define PAGE_15 0x3FFC0
#define PAGE_SIZE 0x40
void joycon_init() {
	
	typedef struct __attribute__((__packed__)) usersettings
{
    uint8_t a1;
  uint8_t b1;
  uint8_t c1;
  uint8_t d1;
  uint8_t e1;
  uint8_t f1;
  uint8_t g1;
  uint8_t h1;
  uint8_t i1;
  uint8_t j1;
  uint8_t k1;
  uint8_t l1;
  uint8_t m1;
} usersettings_t;

//uint8_t EEPROM_BL_MAGIC = 0;
uint8_t EEPROM_DUAL_BOOT_TOGGLE = 0;
 for (int y = PAGE_15; y >= (PAGE_00); y -= PAGE_SIZE)
  {
    usersettings_t *config = (usersettings_t *)y;
    if (config->a1 == 0)
    {
		//EEPROM_BL_MAGIC = config-> d1;
      EEPROM_DUAL_BOOT_TOGGLE = config-> m1;
      
      break;
    }
  }	

if(EEPROM_DUAL_BOOT_TOGGLE == 3)
{
		// Check if there is an IO which will hold us inside the bootloader.
  #if defined(USB_PIN)
    PORT_PINCFG_Type pincfg = {0};
    pincfg.bit.PMUXEN = false;
    pincfg.bit.INEN   = true;
    pincfg.bit.DRVSTR = true;

    PINOP(USB_PIN, DIRCLR);        // Pin is an input

  /* #if defined(HOLD_PIN_PULLUP)
    pincfg.bit.PULLEN = true;
    PINOP(USB_PIN, OUTSET); // Pin is pulled up.
  #elif defined(HOLD_PIN_PULLDOWN)
    pincfg.bit.PULLEN = true;
    PINOP(USB_PIN, OUTCLR); // Pin is pulled up.
  #endif */
    PINCFG(USB_PIN) = pincfg.reg;

    if (PINIP(USB_PIN) != 0) 
	{
		SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk; /* Enable deepsleep */
		GCLK->CLKCTRL.reg = (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2
                                | GCLK_CLKCTRL_ID (GCLK_CLKCTRL_ID_EIC_Val));
		while (GCLK->STATUS.bit.SYNCBUSY){}

  __DSB (); /* Ensure effect of last store takes effect */
  __WFI (); /* Enter sleep mode */
		
		
	}
#endif
	PINOP(JOYCON_PIN, DIRSET); //Set to output
	PINOP(VOLUP_PIN, DIRSET);
	PINOP(JOYCON_PIN, OUTCLR); //Write low
	PINOP(VOLUP_PIN, OUTCLR);
	delay(STRAP_HOLD_TIME); //75 mS to be safe?
	PINOP(JOYCON_PIN, DIRTGL); //Change direction to input to prevent gremlins
	PINOP(VOLUP_PIN, DIRTGL);
}

if(EEPROM_DUAL_BOOT_TOGGLE == 2)
{
	PINOP(JOYCON_PIN, DIRSET); //Set to output
	PINOP(VOLUP_PIN, DIRSET);
	PINOP(JOYCON_PIN, OUTCLR); //Write low
	PINOP(VOLUP_PIN, OUTCLR);
	delay(STRAP_HOLD_TIME); //75 mS to be safe?
	PINOP(JOYCON_PIN, DIRTGL); //Change direction to input to prevent gremlins
	PINOP(VOLUP_PIN, DIRTGL);
}
if(EEPROM_DUAL_BOOT_TOGGLE == 1)
{
PINOP(JOYCON_PIN, DIRSET); //Set to output
	//PINOP(VOLUP_PIN, DIRSET);
	delay(STRAP_HOLD_TIME);
	PINOP(JOYCON_PIN, DIRTGL); //Change direction to input to prevent gremlins
	//PINOP(VOLUP_PIN, DIRTGL);
}	
}
