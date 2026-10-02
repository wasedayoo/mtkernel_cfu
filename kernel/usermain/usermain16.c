// Alarm example: run a one-shot handler after 20 ms

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

LOCAL volatile UINT alarm_count;

// Alarm handler (runs in a task-independent context)
LOCAL void alarm_handler(void *exinf)
{
    volatile UINT *count = (volatile UINT *)exinf;
    ++(*count);                                // Keep interrupt-side work short
}

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    T_CALM calm_once = {
        .exinf  = (void *)&alarm_count,
        .almatr = TA_HLNG,
        .almhdr = (FP)alarm_handler,
    };
    ID almid_once;

    almid_once = tk_cre_alm(&calm_once);       // Create alarm
    tm_putstring((const UB *)"main: start 20 ms alarm\n");
    (void)tk_sta_alm(almid_once, 20U);         // Start one-shot alarm

    (void)tk_dly_tsk(50U);                    // Wait until handler has run
    tm_printf((const UB *)"alarm count = %u\n", alarm_count);

    (void)tk_del_alm(almid_once);              // Delete alarm
    (void)tk_slp_tsk(TMO_FEVR);
    return 0;
}
