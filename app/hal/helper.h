#include <stdint.h>

uint8_t get_first_index_of_one(uint16_t value);

void delay_ms(uint32_t ms);

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&);             \
    void operator=(const TypeName&)

namespace Hal {
typedef void (*Callback)(void* context);
}