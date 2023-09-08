#include "hal/hal.h"
#include "hl_hal/debug.h"
#include "hl_hal/st7789.h"
#include <main.h>
#include <party_gfx.h>

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

using namespace PartyGFX;

static void dma_memset(uint8_t* buffer, size_t buffer_size, uint16_t value) {
    std::atomic<bool> memset_complete = false;
    Hal::Callback callback = [](void* context) {
        std::atomic<bool>* memset_complete = static_cast<std::atomic<bool>*>(context);
        *memset_complete = true;
    };
    HalDma::memory_set(buffer, &value, buffer_size / 2, callback, &memset_complete);
    while(!memset_complete)
        ;
}

static void render(
    uint8_t* buffer,
    size_t buffer_size,
    size_t x,
    size_t y,
    size_t width,
    size_t height,
    void* context) {
    dma_memset(buffer, buffer_size, Color(0, 0, 0));

    Elements* elements = static_cast<Elements*>(context);
    ScreenSpan screen_span(
        Rect(Point(x, y), Point(x + width, y + height)), buffer, buffer_size, 2);
    elements->render(screen_span);
}

Color rand_color() {
    return Color(rand() % 256, rand() % 256, rand() % 256);
}

extern "C" void app_main() {
    hal_init();
    Debug::init();
    Debug::info(TAG, "App started");

    display.init();
    display.backlight(true);

    Elements elements;
    auto pixels = std::vector<std::shared_ptr<Pixel> >();
    for(size_t i = 0; i < 1012; i++) {
        pixels.push_back(std::make_shared<Pixel>(Point(rand() % 240, rand() % 280), rand_color()));
        elements.add_element(pixels.back());
    }

    while(1) {
        display.render_and_send_buffer(render, &elements);
        while(display.is_busy())
            ;
        for(auto& pixel : pixels) {
            Point point = pixel->get_point();
            point.x += rand() % 3 - 1;
            point.y += rand() % 3 - 1;
            if(point.x < 0) point.x = 239;
            if(point.x > 239) point.x = 0;
            if(point.y < 0) point.y = 279;
            if(point.y > 279) point.y = 0;
            pixel->set_point(point);
            pixel->set_color(rand_color());
        }
    }

    Debug::error(TAG, "App ended");
}