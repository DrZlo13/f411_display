#pragma once
#include "hal_gpio.h"
#include "hal_uart.h"
#include "hal_spi.h"

namespace HalDma {
void set_dma_interrupt_callback(
    DMA_TypeDef* dma,
    uint8_t stream,
    Hal::Callback callback,
    void* context);

void memory_set(
    void* dest,
    uint8_t* value,
    size_t size,
    Hal::Callback callback = nullptr,
    void* context = nullptr);

void memory_set(
    void* dest,
    uint16_t* value,
    size_t size,
    Hal::Callback callback = nullptr,
    void* context = nullptr);

void memory_copy(
    void* dest,
    const void* src,
    size_t size,
    Hal::Callback callback = nullptr,
    void* context = nullptr);
}

void hal_init(void);