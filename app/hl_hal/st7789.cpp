#include "st7789.h"

typedef enum {
    NOP = 0x00, // No Operation
    SWRESET = 0x01, // Software Reset
    RDDID = 0x04, // Read Display ID
    RDDST = 0x09, // Read Display Status
    RDDPM = 0x0A, // Read Display Power Mode
    RDDMADCTL = 0x0B, // Read Display MADCTL
    RDDCOLMOD = 0x0C, // Read Display Pixel Format
    RDDIM = 0x0D, // Read Display Image Mode
    RDDSM = 0x0E, // Read Display Signal Mode
    RDDSDR = 0x0F, // Read Display Self-Diagnostic Result
    SLPIN = 0x10, // Sleep In
    SLPOUT = 0x11, // Sleep Out
    PTLON = 0x12, // Partial Display Mode On
    NORON = 0x13, // Normal Display Mode On
    INVOFF = 0x20, // Display Inversion Off
    INVON = 0x21, // Display Inversion On
    GAMSET = 0x26, // Gamma Set
    DISPOFF = 0x28, // Display Off
    DISPON = 0x29, // Display On
    CASET = 0x2A, // Column Address Set
    RASET = 0x2B, // Row Address Set
    RAMWR = 0x2C, // Memory Write
    RAMRD = 0x2E, // Memory Read
    PTLAR = 0x30, // Partial Area
    VSCRDEF = 0x33, // Vertical Scrolling Definition
    TEOFF = 0x34, // Tearing Effect Line OFF
    TEON = 0x35, // Tearing Effect Line ON
    MADCTL = 0x36, // Memory Data Access Control
    VSCRSADD = 0x37, // Vertical Scrolling Start Address
    IDMOFF = 0x38, // Idle Mode Off
    IDMON = 0x39, // Idle Mode On
    COLMOD = 0x3A, // Interface Pixel Format
    RAMWRC = 0x3C, // Memory Write Continue
    RAMRDC = 0x3E, // Memory Read Continue
    TESCAN = 0x44, // Tearing Effect Line Area
    RDTESCAN = 0x45, // Read Tear Effect Line Area
    WRDISBV = 0x51, // Write Display Brightness
    RDDISBV = 0x52, // Read Display Brightness Value
    WRCTRLD = 0x53, // Write CTRL Display
    RDCTRLD = 0x54, // Read CTRL Value Display
    WRCACE = 0x55, // Write Content Adaptive Brightness Control and Color Enhancement
    RDCABC = 0x56, // Read Content Adaptive Brightness Control
    WRCABCMB = 0x5E, // Write CABC Minimum Brightness
    RDCABCMB = 0x5F, // Read CABC Minimum Brightness
    RDABCSDR = 0x68, // Read Automatic Brightness Control Self-Diagnostic Result
    RDID1 = 0xDA, // Read ID1
    RDID2 = 0xDB, // Read ID2
    RDID3 = 0xDC, // Read ID3
    RAMCTRL = 0xB0, // RAM Control
    RGBCTRL = 0xB1, // RGB Interface Control
    PORCTRL = 0xB2, // Porch Setting
    FRCTRL1 = 0xB3, // Frame Rate Control 1
    PARCTRL = 0xB5, // Partial Mode Control
    GCTRL = 0xB7, // Gate Control
    GTADJ = 0xB8, // Gate On Timing Adjustment
    DGMEN = 0xBA, // Digital Gamma Enable
    VCOMS = 0xBB, // VCOMS Setting
    LCMCTRL = 0xC0, // LCM Control
    IDSET = 0xC1, // ID Code Setting
    VDVVRHEN = 0xC2, // VDV and VRH Command Enable
    VRHS = 0xC3, // VRH Set
    VDVS = 0xC4, // VDV Set
    VCMOFSET = 0xC5, // VCOMS Offset Set
    FRCTRL2 = 0xC6, // Frame Rate Control in Normal Mode
    CABCCTRL = 0xC7, // CABC Control
    REGSEL1 = 0xC8, // Register Value Section 1
    REGSEL2 = 0xCA, // Register Value Section 2
    PWMFRSEL = 0xCC, // PWM Frequency Selection
    PWCTRL1 = 0xD0, // Power Control 1
    VAPVANEN = 0xD2, // Enable VAP/VAN signal output
    CMD2EN = 0xDF, // Command 2 Enable
    PVGAMCTRL = 0xE0, // Positive Voltage Gamma Control
    NVGAMCTRL = 0xE1, // Negative Voltage Gamma Control
    DGMLUTR = 0xE2, // Digital Gamma Look-up Table for Red
    DGMLUTB = 0xE3, // Digital Gamma Look-up Table for Blue
    GATECTRL = 0xE4, // Gate Control
    SPI2EN = 0xE7, // SPI2 Enable
    PWCTRL2 = 0xE8, // Power Control 2
    EQCTRL = 0xE9, // Equalize time control
    PROMCTRL = 0xEC, // Program Mode Control
    PROMEN = 0xFA, // Program Mode Enable
    NVMSET = 0xFC, // NVM Setting
    PROMACT = 0xFE, // Program action
} CMD;

typedef enum {
    MY = 0x80, // Page Address Order ('0': Top to Bottom, '1': the opposite)
    MX = 0x40, // Column Address Order ('0': Left to Right, '1': the opposite)
    MV = 0x20, // Page/Column Order ('0' = Normal Mode, '1' = Reverse Mode)
    ML = 0x10, // Line Address Order ('0' = LCD Refresh Top to Bottom, '1' = the opposite)
    RGB = 0x00, // RGB/BGR Order ('0' = RGB, '1' = BGR)
} MADCTL_BIT;

void ST7789::reset() {
    gpio_reset.write(false);
    delay_ms(25);
    gpio_reset.write(true);
}

void ST7789::select() {
    gpio_cs.write(false);
}

void ST7789::deselect() {
    gpio_cs.write(true);
}

void ST7789::command_mode() {
    gpio_dc.write(false);
}

void ST7789::data_mode() {
    gpio_dc.write(true);
}

void ST7789::write_command(uint8_t cmd) {
    select();
    command_mode();
    spi.transmit(cmd);
    deselect();
}

void ST7789::write_data(uint8_t data) {
    select();
    data_mode();
    spi.transmit(data);
    deselect();
}

void ST7789::write_data(const uint8_t* data, size_t data_size) {
    select();
    data_mode();
    spi.transmit(data, data_size);
    deselect();
}

void ST7789::write_data(std::initializer_list<uint8_t> data) {
    write_data(data.begin(), data.size());
}

void ST7789::write_command_data(uint8_t cmd, const uint8_t* data, size_t data_size) {
    select();
    command_mode();
    spi.transmit(cmd);
    deselect();

    select();
    data_mode();
    spi.transmit(data, data_size);
    deselect();
}

void ST7789::write_command_data(uint8_t cmd, std::initializer_list<uint8_t> data) {
    write_command_data(cmd, data.begin(), data.size());
}

void ST7789::write_command_data(uint8_t cmd, uint8_t data) {
    write_command_data(cmd, &data, sizeof(data));
}

void ST7789::set_address_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    uint16_t x_start = x0 + x_shift;
    uint16_t x_end = x1 + x_shift;
    uint16_t y_start = y0 + y_shift;
    uint16_t y_end = y1 + y_shift;

    // column address set
    write_command_data(
        CMD::CASET,
        {
            uint8_t(x_start >> 8),
            uint8_t(x_start & 0xFF),
            uint8_t(x_end >> 8),
            uint8_t(x_end & 0xFF),
        });

    // row address set
    write_command_data(
        CMD::RASET,
        {
            uint8_t(y_start >> 8),
            uint8_t(y_start & 0xFF),
            uint8_t(y_end >> 8),
            uint8_t(y_end & 0xFF),
        });
}

ST7789::ST7789(
    HalSpi& spi,
    HalGpio& gpio_reset,
    HalGpio& gpio_dc,
    HalGpio& gpio_cs,
    HalGpio& gpio_backlight)
    : gpio_reset(gpio_reset)
    , gpio_dc(gpio_dc)
    , gpio_cs(gpio_cs)
    , gpio_backlight(gpio_backlight)
    , spi(spi)
    , x_shift(0)
    , y_shift(20)
    , height(280)
    , width(240) {
}

ST7789::~ST7789() {
}

void ST7789::init() {
    {
        using Mode = HalGpio::Mode;
        using Pull = HalGpio::Pull;
        using Speed = HalGpio::Speed;

        gpio_reset.config(Mode::OutputPushPull, Pull::No, Speed::VeryHigh);
        gpio_dc.config(Mode::OutputPushPull, Pull::No, Speed::VeryHigh);
        gpio_cs.config(Mode::OutputPushPull, Pull::No, Speed::VeryHigh);
        gpio_backlight.config(Mode::OutputPushPull, Pull::No, Speed::VeryHigh);

        spi.disable();
        spi.config(
            HalSpi::DataWidth::Bits8,
            HalSpi::ClockPolarity::Low,
            HalSpi::ClockPhase::FirstEdge,
            HalSpi::BaudRate::Div2,
            HalSpi::BitOrder::MsbFirst);
        spi.enable();
    }

    delay_ms(25);
    reset();
    delay_ms(50);

    write_command(CMD::SWRESET);
    delay_ms(150);
    write_command(CMD::SLPOUT);
    delay_ms(10);
    write_command_data(CMD::COLMOD, 0x55);
    delay_ms(10);
    write_command_data(CMD::MADCTL, 0x08);
    set_address_window(0, 0, width, height);
    write_command(CMD::INVON);
    delay_ms(10);
    write_command(CMD::NORON);
    delay_ms(10);
    write_command(CMD::DISPON);
    delay_ms(10);
    rotation(0);
}

void ST7789::rotation(uint8_t rotation) {
    switch(rotation) {
    case 0:
        write_command_data(CMD::MADCTL, MADCTL_BIT::MX | MADCTL_BIT::MY | MADCTL_BIT::RGB);
        break;
    case 1:
        write_command_data(CMD::MADCTL, MADCTL_BIT::MY | MADCTL_BIT::MV | MADCTL_BIT::RGB);
        break;
    case 2:
        write_command_data(CMD::MADCTL, MADCTL_BIT::RGB);
        break;
    case 3:
        write_command_data(CMD::MADCTL, MADCTL_BIT::MX | MADCTL_BIT::MV | MADCTL_BIT::RGB);
        break;
    default:
        break;
    }
}

void ST7789::fill(uint16_t color) {
    set_address_window(0, 0, width - 1, height - 1);
    write_command(CMD::RAMWR);

    select();
    for(size_t i = 0; i < width * height; i++) {
        write_data({
            uint8_t(color >> 8),
            uint8_t(color & 0xFF),
        });
    }
    deselect();
}

void ST7789::backlight(bool on) {
    gpio_backlight.write(on);
}
