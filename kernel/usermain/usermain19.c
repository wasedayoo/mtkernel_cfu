// Cyclic handler example: start automatically when it is created

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

LOCAL volatile UINT heartbeat_count;

// Heartbeat handler
LOCAL void heartbeat_handler(void *exinf)
{
    volatile UINT *count = (volatile UINT *)exinf;
    ++(*count);
}

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    T_CCYC ccyc_heartbeat = {
        .exinf  = (void *)&heartbeat_count,
        .cycatr = TA_HLNG | TA_STA,           // Start during tk_cre_cyc
        .cychdr = (FP)heartbeat_handler,
        .cyctim = 10U,                        // 10 ms interval
        .cycphs = 10U,                        // First call after 10 ms
    };
    ID cycid_heartbeat;

    // TA_STA means no separate tk_sta_cyc call is needed here.
    cycid_heartbeat = tk_cre_cyc(&ccyc_heartbeat);
    tm_putstring((const UB *)"main: automatic heartbeat started\n");

    (void)tk_dly_tsk(50U);
    (void)tk_stp_cyc(cycid_heartbeat);
    tm_printf((const UB *)"heartbeat count = %u\n", heartbeat_count);

    (void)tk_del_cyc(cycid_heartbeat);
    (void)tk_slp_tsk(TMO_FEVR);
    return 0;
}
