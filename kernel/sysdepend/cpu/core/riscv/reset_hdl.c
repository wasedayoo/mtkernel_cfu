#include <stdint.h>

void Reset_Handler(void)
{
    volatile uint32_t *signature =
        (volatile uint32_t *)0x10000000;

    signature[0] = 0x12345678;

    while (1) {
    }
}