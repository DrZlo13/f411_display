#pragma once
#include <atomic>
#include <stdint.h>
#include <hal/hal.h>

class ST7789 {
private:
    DISALLOW_COPY_AND_ASSIGN(ST7789);

    HalGpio& gpio_reset;
    HalGpio& gpio_dc;
    HalGpio& gpio_cs;
    HalGpio& gpio_backlight;
    HalSpi& spi;

    const size_t x_shift;
    const size_t y_shift;

    void reset();
    void select();
    void deselect();
    void command_mode();
    void data_mode();
    void write_command(uint8_t cmd);
    void write_data(uint8_t data);
    void write_data(const uint8_t* data, size_t data_size);
    void write_data(std::initializer_list<uint8_t> data);
    void write_command_data(uint8_t cmd, const uint8_t* data, size_t data_size);
    void write_command_data(uint8_t cmd, std::initializer_list<uint8_t> data);
    void write_command_data(uint8_t cmd, uint8_t data);
    void set_address_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

public:
    ST7789(
        HalSpi& spi,
        HalGpio& gpio_reset,
        HalGpio& gpio_dc,
        HalGpio& gpio_cs,
        HalGpio& gpio_backlight);
    ~ST7789();

    static const size_t height = 280;
    static const size_t width = 240;

    uint8_t buffer[(height * width * 2) / 2];

    void init();
    void rotation(uint8_t rotation);
    void backlight(bool on);

    void fill(uint16_t color);

    typedef void (*BufferRenderCallback)(
        uint8_t* buffer,
        size_t buffer_size,
        size_t x,
        size_t y,
        size_t width,
        size_t height,
        void* context);

    void render_and_send_buffer(BufferRenderCallback callback, void* context);

    bool is_busy();

private:
    typedef struct {
        BufferRenderCallback callback;
        void* context;
        ST7789* display;
    } DmaContext;

    std::atomic<bool> busy;
    DmaContext* dma_context;
};
