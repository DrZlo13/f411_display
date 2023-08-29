#include "hal/hal.h"
#include "hl_hal/debug.h"
#include "hl_hal/st7789.h"
#include <main.h>

constexpr const char* TAG = "app";

static HalGpio gpio_display_reset(GPIO(B, 0));
static HalGpio gpio_display_dc(GPIO(B, 1));
static HalGpio gpio_display_cs(GPIO(B, 2));
static HalGpio gpio_display_backlight(GPIO(B, 10));
static HalSpi spi_display(SPI1);
static ST7789 display(
    spi_display,
    gpio_display_reset,
    gpio_display_dc,
    gpio_display_cs,
    gpio_display_backlight);

extern "C" void app_main() {
    hal_init();
    Debug::init();
    Debug::info(TAG, "App started");

    display.init();
    display.backlight(true);

    Debug::error(TAG, "App ended");
    while(1) {
        display.fill(0b1111100000000000);
        delay_ms(1000);
        display.fill(0b0000011111100000);
        delay_ms(1000);
        display.fill(0b0000000000011111);
        delay_ms(1000);
        display.fill(0b0000000000000000);
        delay_ms(1000);
        display.fill(0b1111111111111111);
        delay_ms(1000);
    }
}