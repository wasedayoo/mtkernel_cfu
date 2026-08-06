#include <stdint.h>

/* Method 4: .rodata and .data are preloaded directly into DMEM. */
__attribute__((section(".test_status")))
volatile uint32_t test_status = 0xc001d00d;

volatile const uint32_t rodata_value = 0xa5a55a5a;
volatile uint32_t initialized_data = 0x13579bdf;
volatile uint32_t zero_initialized_data;

extern uint32_t __bss_start[];
extern uint32_t __bss_end[];

static void clear_bss(void)
{
    uint32_t *p;

    for (p = __bss_start; p < __bss_end; p++) {
        *p = 0;
    }
}

__attribute__((noinline))
static uint32_t stack_test(uint32_t high, uint32_t low)
{
    volatile uint32_t local[4];

    local[0] = high;
    local[1] = low;
    local[2] = local[0] + local[1];
    local[3] = local[2];

    return local[3];
}

void Reset_Handler(void)
{
    uint32_t result;

    /* Make the BSS clear observable instead of relying on BRAM power-up data. */
    zero_initialized_data = 0xffffffff;
    clear_bss();

    if (rodata_value != 0xa5a55a5a) {
        result = 0xdead0001;
    } else if (initialized_data != 0x13579bdf) {
        result = 0xdead0002;
    } else if (zero_initialized_data != 0) {
        result = 0xdead0003;
    } else {
        result = stack_test(0x12340000, 0x00005678);
    }

    test_status = result;

    while (1) {
    }
}
