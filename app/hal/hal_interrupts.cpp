#include <cstddef>
#include "hal.h"
#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_system.h>
#include <stm32f4xx_ll_exti.h>
#include <stm32f4xx_ll_dma.h>

typedef struct {
    Hal::Callback callback;
    void* context;
} Interrupt;

static volatile Interrupt gpio_interrupt[GPIO_COUNT] = {0};

static const size_t dma_count = 2;
static const size_t dma_stream_count = 8;
static volatile Interrupt dma_stream_interrupt[dma_count * dma_stream_count] = {0};

static size_t get_dma_index(DMA_TypeDef* dma) {
    if(dma == DMA2) {
        return 1;
    } else {
        return 0;
    }
}

static void gpio_interrupt_call(uint16_t pin_num) {
    if(gpio_interrupt[pin_num].callback != NULL) {
        gpio_interrupt[pin_num].callback(gpio_interrupt[pin_num].context);
    }
}

static void dma_interrupt_call(DMA_TypeDef* dma, uint8_t stream) {
    size_t dma_interrupt_index = get_dma_index(dma) * dma_stream_count + stream;
    if(dma_stream_interrupt[dma_interrupt_index].callback != NULL) {
        dma_stream_interrupt[dma_interrupt_index].callback(
            dma_stream_interrupt[dma_interrupt_index].context);
    }
}

void HalGpio::set_interrupt_callback(Hal::Callback callback, void* context) {
    __disable_irq();
    uint8_t pin_num = get_first_index_of_one(this->pin);
    gpio_interrupt[pin_num].callback = callback;
    gpio_interrupt[pin_num].context = context;
    __enable_irq();
}

void HalDma::set_dma_interrupt_callback(
    DMA_TypeDef* dma,
    uint8_t stream,
    Hal::Callback callback,
    void* context) {
    __disable_irq();
    size_t dma_interrupt_index = get_dma_index(dma) * dma_stream_count + stream;
    dma_stream_interrupt[dma_interrupt_index].callback = callback;
    dma_stream_interrupt[dma_interrupt_index].context = context;
    __enable_irq();
}

/* Interrupt handlers */
extern "C" void EXTI0_IRQHandler(void) {
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_0)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_0);
        gpio_interrupt_call(0);
    }
}

extern "C" void EXTI1_IRQHandler(void) {
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_1)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_1);
        gpio_interrupt_call(1);
    }
}

extern "C" void EXTI2_IRQHandler(void) {
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_2)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_2);
        gpio_interrupt_call(2);
    }
}

extern "C" void EXTI3_IRQHandler(void) {
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_3)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_3);
        gpio_interrupt_call(3);
    }
}

extern "C" void EXTI4_IRQHandler(void) {
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_4)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_4);
        gpio_interrupt_call(4);
    }
}

extern "C" void EXTI9_5_IRQHandler(void) {
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_5)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_5);
        gpio_interrupt_call(5);
    }
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_6)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_6);
        gpio_interrupt_call(6);
    }
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_7)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_7);
        gpio_interrupt_call(7);
    }
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_8)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_8);
        gpio_interrupt_call(8);
    }
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_9)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_9);
        gpio_interrupt_call(9);
    }
}

extern "C" void EXTI15_10_IRQHandler(void) {
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_10)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_10);
        gpio_interrupt_call(10);
    }
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_11)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_11);
        gpio_interrupt_call(11);
    }
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_12)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_12);
        gpio_interrupt_call(12);
    }
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_13)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_13);
        gpio_interrupt_call(13);
    }
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_14)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_14);
        gpio_interrupt_call(14);
    }
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_15)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_15);
        gpio_interrupt_call(15);
    }
}

extern "C" void NMI_Handler(void) {
    while(1) {
    }
}

extern "C" void HardFault_Handler(void) {
    while(1) {
    }
}

extern "C" void MemManage_Handler(void) {
    while(1) {
    }
}

extern "C" void BusFault_Handler(void) {
    while(1) {
    }
}

extern "C" void UsageFault_Handler(void) {
    while(1) {
    }
}

extern "C" void SVC_Handler(void) {
}

extern "C" void DebugMon_Handler(void) {
}

extern "C" void PendSV_Handler(void) {
}

extern "C" void SysTick_Handler(void) {
}

extern "C" void DMA2_Stream0_IRQHandler(void) {
    if(LL_DMA_IsActiveFlag_TC0(DMA2)) {
        LL_DMA_ClearFlag_TC0(DMA2);
        dma_interrupt_call(DMA2, LL_DMA_STREAM_0);
    }
    if(LL_DMA_IsActiveFlag_HT0(DMA2)) {
        LL_DMA_ClearFlag_HT0(DMA2);
    }
}

extern "C" void DMA2_Stream2_IRQHandler(void) {
    if(LL_DMA_IsActiveFlag_TC2(DMA2)) {
        LL_DMA_ClearFlag_TC2(DMA2);
        dma_interrupt_call(DMA2, LL_DMA_STREAM_2);
    }
    if(LL_DMA_IsActiveFlag_HT2(DMA2)) {
        LL_DMA_ClearFlag_HT2(DMA2);
    }
}