#pragma once
#include <stdint.h>
#include <stddef.h>
#include "helper.h"
#include <stm32f4xx_ll_spi.h>

class HalSpi {
private:
    SPI_TypeDef* spi;
    DISALLOW_COPY_AND_ASSIGN(HalSpi);

    void end_transmission();

public:
    HalSpi(SPI_TypeDef* spi);
    ~HalSpi();

    enum class DataWidth {
        Bits8 = LL_SPI_DATAWIDTH_8BIT,
        Bits16 = LL_SPI_DATAWIDTH_16BIT,
    };

    enum class ClockPolarity {
        Low = LL_SPI_POLARITY_LOW,
        High = LL_SPI_POLARITY_HIGH,
    };

    enum class ClockPhase {
        FirstEdge = LL_SPI_PHASE_1EDGE,
        SecondEdge = LL_SPI_PHASE_2EDGE,
    };

    enum class BaudRate {
        Div2 = LL_SPI_BAUDRATEPRESCALER_DIV2,
        Div4 = LL_SPI_BAUDRATEPRESCALER_DIV4,
        Div8 = LL_SPI_BAUDRATEPRESCALER_DIV8,
        Div16 = LL_SPI_BAUDRATEPRESCALER_DIV16,
        Div32 = LL_SPI_BAUDRATEPRESCALER_DIV32,
        Div64 = LL_SPI_BAUDRATEPRESCALER_DIV64,
        Div128 = LL_SPI_BAUDRATEPRESCALER_DIV128,
        Div256 = LL_SPI_BAUDRATEPRESCALER_DIV256,
    };

    enum class BitOrder {
        MsbFirst = LL_SPI_MSB_FIRST,
        LsbFirst = LL_SPI_LSB_FIRST,
    };

    void config(
        DataWidth data_width,
        ClockPolarity clock_polarity,
        ClockPhase clock_phase,
        BaudRate baud_rate,
        BitOrder bit_order);
    void enable();
    void disable();
    void transmit(const uint8_t* buffer, size_t buffer_size);
    void transmit(const uint8_t data);
};