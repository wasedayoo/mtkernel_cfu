// Cyclic handler example: count periodic timer events

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

LOCAL volatile UINT cyclic_count;

// Cyclic handler (runs every 10 ms while started)
LOCAL void cyclic_handler(void *exinf)
{
    volatile UINT *count = (volatile UINT *)exinf;
    ++(*count);                                // Keep interrupt-side work short
}

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    T_CCYC ccyc_counter = {
        .exinf  = (void *)&cyclic_count,
        .cycatr = TA_HLNG,
        .cychdr = (FP)cyclic_handler,
        .cyctim = 10U,                        // 10 ms interval
        .cycphs = 0U,
    };
    ID cycid_counter;

    cycid_counter = tk_cre_cyc(&ccyc_counter); // Create in stopped state
    (void)tk_sta_cyc(cycid_counter);           // Start cyclic handler
    tm_putstring((const UB *)"main: cyclic handler started\n");

    (void)tk_dly_tsk(60U);
    (void)tk_stp_cyc(cycid_counter);           // Stop periodic execution
    tm_printf((const UB *)"cyclic count = %u\n", cyclic_count);

    (void)tk_del_cyc(cycid_counter);           // Delete cyclic handler
    (void)tk_slp_tsk(TMO_FEVR);
    return 0;
}
