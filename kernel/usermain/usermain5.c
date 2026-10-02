// Event flag example: broadcast one event to two waiting tasks

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

// Event flag configuration (multiple tasks may wait)
LOCAL ID flgid_broadcast;
LOCAL T_CFLG cflg_broadcast = {
    .flgatr  = TA_TFIFO | TA_WMUL,
    .iflgptn = 0U,
};

// Common waiting task
LOCAL void task_listener(INT stacd, void *exinf)
{
    UINT pattern;

    (void)exinf;
    (void)tk_wai_flg(flgid_broadcast, 0x01U,
                     TWF_ORW, &pattern, TMO_FEVR);
    tm_printf((const UB *)"listener %d: event received\n", stacd);
    tk_ext_tsk();
}

// Waiting task configuration
LOCAL T_CTSK ctsk_listener = {
    .itskpri = 10,
    .stksz   = 1024,
    .task    = (FP)task_listener,
    .tskatr  = TA_HLNG | TA_RNG3,
};

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    ID listener1;
    ID listener2;

    flgid_broadcast = tk_cre_flg(&cflg_broadcast); // Create event flag

    listener1 = tk_cre_tsk(&ctsk_listener);
    (void)tk_sta_tsk(listener1, 1);                // stacd identifies listener 1
    listener2 = tk_cre_tsk(&ctsk_listener);
    (void)tk_sta_tsk(listener2, 2);                // stacd identifies listener 2

    (void)tk_dly_tsk(20U);                         // Let both tasks begin waiting
    tm_putstring((const UB *)"main: broadcast event\n");
    (void)tk_set_flg(flgid_broadcast, 0x01U);      // Wake both listeners

    (void)tk_dly_tsk(20U);
    (void)tk_del_flg(flgid_broadcast);             // Delete after both received it
    (void)tk_slp_tsk(TMO_FEVR);
    return 0;
}
