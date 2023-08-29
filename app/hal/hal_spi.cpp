#include "hal_spi.h"

HalSpi::HalSpi(SPI_TypeDef* spi)
    : spi(spi) {
}

HalSpi::~HalSpi() {
}

void HalSpi::end_transmission() {
    while(LL_SPI_IsActiveFlag_RXNE(spi)) {
        LL_SPI_ReceiveData8(spi);
    }
    while(LL_SPI_IsActiveFlag_BSY(spi))
        ;
    while(LL_SPI_IsActiveFlag_RXNE(spi)) {
        LL_SPI_ReceiveData8(spi);
    }
}

void HalSpi::config(
    DataWidth data_width,
    ClockPolarity clock_polarity,
    ClockPhase clock_phase,
    BaudRate baud_rate,
    BitOrder bit_order) {
    LL_SPI_InitTypeDef SPI_InitStruct = {0};
    SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
    SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
    SPI_InitStruct.DataWidth = static_cast<uint32_t>(data_width);
    SPI_InitStruct.ClockPolarity = static_cast<uint32_t>(clock_polarity);
    SPI_InitStruct.ClockPhase = static_cast<uint32_t>(clock_phase);
    SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
    SPI_InitStruct.BaudRate = static_cast<uint32_t>(baud_rate);
    SPI_InitStruct.BitOrder = static_cast<uint32_t>(bit_order);
    SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
    SPI_InitStruct.CRCPoly = 10;
    LL_SPI_Init(spi, &SPI_InitStruct);
    LL_SPI_SetStandard(spi, LL_SPI_PROTOCOL_MOTOROLA);
}

void HalSpi::enable() {
    if(LL_SPI_IsEnabled(spi)) {
        return;
    }

    LL_SPI_Enable(spi);
}

void HalSpi::disable() {
    if(!LL_SPI_IsEnabled(spi)) {
        return;
    }

    LL_SPI_Disable(spi);
}

void HalSpi::transmit(const uint8_t* buffer, size_t size) {
    while(size > 0) {
        if(LL_SPI_IsActiveFlag_TXE(spi)) {
            LL_SPI_TransmitData8(spi, *buffer);
            while(LL_SPI_IsActiveFlag_BSY(spi))
                ;
            buffer++;
            size--;
        }
    }

    end_transmission();
    LL_SPI_ClearFlag_OVR(spi);
}

void HalSpi::transmit(const uint8_t data) {
    transmit(&data, 1);
}
