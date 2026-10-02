// Alarm example: cancel a scheduled alarm

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

LOCAL volatile UINT alarm_count;

// Alarm handler
LOCAL void alarm_handler(void *exinf)
{
    volatile UINT *count = (volatile UINT *)exinf;
    ++(*count);
}

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    T_CALM calm_cancel = {
        .exinf  = (void *)&alarm_count,
        .almatr = TA_HLNG,
        .almhdr = (FP)alarm_handler,
    };
    ID almid_cancel;

    almid_cancel = tk_cre_alm(&calm_cancel);   // Create alarm
    (void)tk_sta_alm(almid_cancel, 50U);       // Schedule it for later

    (void)tk_dly_tsk(10U);
    (void)tk_stp_alm(almid_cancel);            // Cancel before it fires
    tm_putstring((const UB *)"main: alarm canceled\n");

    (void)tk_del_alm(almid_cancel);            // Delete alarm
    (void)tk_slp_tsk(TMO_FEVR);
    return 0;
}
