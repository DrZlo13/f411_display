#include "hal.h"
#include <stm32f4xx_ll_dma.h>
#include <atomic>
#include <stddef.h>
#include "hal_spi.h"

static std::atomic<bool> dma2_stream2_transfer_complete = false;
#define SPI_DMA_CONFIG DMA2, LL_DMA_STREAM_2

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

void HalSpi::transmit_dma(const uint8_t* buffer, size_t buffer_size) {
    dma2_stream2_transfer_complete = false;
    Hal::Callback cb = [](void* context) { dma2_stream2_transfer_complete = true; };
    transmit_dma_cb(buffer, buffer_size, cb, NULL);
}

void HalSpi::transmit_dma_cb(
    const uint8_t* buffer,
    size_t buffer_size,
    Hal::Callback callback,
    void* context) {
    LL_DMA_InitTypeDef dma_config = {0};
    LL_DMA_StructInit(&dma_config);
    dma_config.Channel = LL_DMA_CHANNEL_2;
    dma_config.PeriphOrM2MSrcAddress = reinterpret_cast<uint32_t>(&spi->DR);
    dma_config.MemoryOrM2MDstAddress = reinterpret_cast<uint32_t>(buffer);
    dma_config.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    dma_config.Mode = LL_DMA_MODE_NORMAL;
    dma_config.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    dma_config.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    dma_config.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;
    dma_config.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
    dma_config.NbData = buffer_size;
    dma_config.Priority = LL_DMA_PRIORITY_VERYHIGH;
    LL_DMA_Init(SPI_DMA_CONFIG, &dma_config);

    LL_DMA_ClearFlag_TC2(DMA2);

    HalDma::set_dma_interrupt_callback(SPI_DMA_CONFIG, callback, context);
    LL_DMA_EnableIT_TC(SPI_DMA_CONFIG);

    LL_SPI_EnableDMAReq_TX(spi);
    LL_DMA_EnableStream(SPI_DMA_CONFIG);
}

void HalSpi::wait_for_dma_complete() {
    while(!dma2_stream2_transfer_complete)
        ;
    HalDma::set_dma_interrupt_callback(SPI_DMA_CONFIG, NULL, NULL);
    LL_DMA_DeInit(SPI_DMA_CONFIG);
}

void HalSpi::transmit_dma_blocking(const uint8_t* buffer, size_t buffer_size) {
    transmit_dma(buffer, buffer_size);
    wait_for_dma_complete();
}

#define MEM_DMA_CONFIG DMA2, LL_DMA_STREAM_0

void HalDma::memory_set(
    void* dest,
    uint8_t* value,
    size_t size,
    Hal::Callback callback,
    void* context) {
    LL_DMA_DisableStream(MEM_DMA_CONFIG);
    LL_DMA_InitTypeDef dma_config = {0};
    LL_DMA_StructInit(&dma_config);
    dma_config.Channel = LL_DMA_CHANNEL_0;
    dma_config.PeriphOrM2MSrcAddress = reinterpret_cast<uint32_t>(value);
    dma_config.MemoryOrM2MDstAddress = reinterpret_cast<uint32_t>(dest);
    dma_config.Direction = LL_DMA_DIRECTION_MEMORY_TO_MEMORY;
    dma_config.Mode = LL_DMA_MODE_NORMAL;
    dma_config.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    dma_config.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    dma_config.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;
    dma_config.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
    dma_config.NbData = size;
    dma_config.Priority = LL_DMA_PRIORITY_HIGH;
    dma_config.FIFOMode = LL_DMA_FIFOMODE_ENABLE;
    dma_config.FIFOThreshold = LL_DMA_FIFOTHRESHOLD_FULL;
    dma_config.MemBurst = LL_DMA_MBURST_SINGLE;
    dma_config.PeriphBurst = LL_DMA_PBURST_SINGLE;

    LL_DMA_Init(MEM_DMA_CONFIG, &dma_config);

    LL_DMA_ClearFlag_TC0(DMA2);

    HalDma::set_dma_interrupt_callback(MEM_DMA_CONFIG, callback, context);
    LL_DMA_EnableIT_TC(MEM_DMA_CONFIG);

    LL_DMA_EnableStream(MEM_DMA_CONFIG);
}

void HalDma::memory_set(
    void* dest,
    uint16_t* value,
    size_t size,
    Hal::Callback callback,
    void* context) {
    LL_DMA_DisableStream(MEM_DMA_CONFIG);
    LL_DMA_InitTypeDef dma_config = {0};
    LL_DMA_StructInit(&dma_config);
    dma_config.Channel = LL_DMA_CHANNEL_0;
    dma_config.PeriphOrM2MSrcAddress = reinterpret_cast<uint32_t>(value);
    dma_config.MemoryOrM2MDstAddress = reinterpret_cast<uint32_t>(dest);
    dma_config.Direction = LL_DMA_DIRECTION_MEMORY_TO_MEMORY;
    dma_config.Mode = LL_DMA_MODE_NORMAL;
    dma_config.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    dma_config.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    dma_config.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_HALFWORD;
    dma_config.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_HALFWORD;
    dma_config.NbData = size;
    dma_config.Priority = LL_DMA_PRIORITY_HIGH;
    dma_config.FIFOMode = LL_DMA_FIFOMODE_ENABLE;
    dma_config.FIFOThreshold = LL_DMA_FIFOTHRESHOLD_FULL;
    dma_config.MemBurst = LL_DMA_MBURST_SINGLE;
    dma_config.PeriphBurst = LL_DMA_PBURST_SINGLE;

    LL_DMA_Init(MEM_DMA_CONFIG, &dma_config);

    LL_DMA_ClearFlag_TC0(DMA2);

    HalDma::set_dma_interrupt_callback(MEM_DMA_CONFIG, callback, context);
    LL_DMA_EnableIT_TC(MEM_DMA_CONFIG);

    LL_DMA_EnableStream(MEM_DMA_CONFIG);
}

void HalDma::memory_copy(
    void* dest,
    const void* src,
    size_t size,
    Hal::Callback callback,
    void* context) {
    LL_DMA_DisableStream(MEM_DMA_CONFIG);
    LL_DMA_InitTypeDef dma_config = {0};
    LL_DMA_StructInit(&dma_config);
    dma_config.Channel = LL_DMA_CHANNEL_0;
    dma_config.PeriphOrM2MSrcAddress = reinterpret_cast<uint32_t>(src);
    dma_config.MemoryOrM2MDstAddress = reinterpret_cast<uint32_t>(dest);
    dma_config.Direction = LL_DMA_DIRECTION_MEMORY_TO_MEMORY;
    dma_config.Mode = LL_DMA_MODE_NORMAL;
    dma_config.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_INCREMENT;
    dma_config.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    dma_config.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;
    dma_config.MemoryOrM2MDstDataSize = LL_DMA_PDATAALIGN_BYTE;
    dma_config.NbData = size;
    dma_config.Priority = LL_DMA_PRIORITY_HIGH;
    dma_config.FIFOMode = LL_DMA_FIFOMODE_ENABLE;
    dma_config.FIFOThreshold = LL_DMA_FIFOTHRESHOLD_FULL;
    dma_config.MemBurst = LL_DMA_MBURST_SINGLE;
    dma_config.PeriphBurst = LL_DMA_PBURST_SINGLE;

    LL_DMA_Init(MEM_DMA_CONFIG, &dma_config);

    LL_DMA_ClearFlag_TC0(DMA2);

    HalDma::set_dma_interrupt_callback(MEM_DMA_CONFIG, callback, context);
    LL_DMA_EnableIT_TC(MEM_DMA_CONFIG);

    LL_DMA_EnableStream(MEM_DMA_CONFIG);
}