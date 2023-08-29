#include "helper.h"
#include <stm32f4xx_ll_utils.h>

uint8_t get_first_index_of_one(uint16_t value) {
    uint8_t index = 0;
    for(index = 0; index < 16; index++) {
        if(value & (1 << index)) break;
    }
    return index;
}

void delay_ms(uint32_t ms) {
    LL_mDelay(ms);
}