// Event flag example: one task notifies another task

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

// Event flag configuration
LOCAL ID flgid_event;                         // Event flag ID
LOCAL T_CFLG cflg_event = {
    .flgatr  = TA_TFIFO | TA_WSGL,            // FIFO, one waiting task
    .iflgptn = 0U,                            // Initial pattern: no event
};

// Waiting task configuration
LOCAL void task_wait(INT stacd, void *exinf); // Entry function
LOCAL T_CTSK ctsk_wait = {
    .itskpri = 10,                            // Initial priority
    .stksz   = 1024,                          // Stack size
    .task    = (FP)task_wait,                 // Entry function
    .tskatr  = TA_HLNG | TA_RNG3,             // Task attributes
};

// Notification task configuration
LOCAL void task_set(INT stacd, void *exinf);  // Entry function
LOCAL T_CTSK ctsk_set = {
    .itskpri = 10,
    .stksz   = 1024,
    .task    = (FP)task_set,
    .tskatr  = TA_HLNG | TA_RNG3,
};

// Wait for bit 0 to be set
LOCAL void task_wait(INT stacd, void *exinf)
{
    UINT pattern;

    (void)stacd;
    (void)exinf;
    tm_putstring((const UB *)"task_wait: waiting for event\n");
    (void)tk_wai_flg(flgid_event, 0x01U,
                     TWF_ORW | TWF_BITCLR, &pattern, TMO_FEVR);
    tm_putstring((const UB *)"task_wait: event received\n");
    (void)tk_del_flg(flgid_event);             // Event flag is no longer needed
    tk_ext_tsk();
}

// Set bit 0 after a short delay
LOCAL void task_set(INT stacd, void *exinf)
{
    (void)stacd;
    (void)exinf;
    (void)tk_dly_tsk(20U);
    tm_putstring((const UB *)"task_set: set event\n");
    (void)tk_set_flg(flgid_event, 0x01U);
    tk_ext_tsk();
}

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    ID tskid_wait;
    ID tskid_set;

    // An event flag can be used immediately after creation; it has no start API.
    flgid_event = tk_cre_flg(&cflg_event);

    tskid_wait = tk_cre_tsk(&ctsk_wait);       // Create waiting task
    (void)tk_sta_tsk(tskid_wait, 0);           // Start waiting task

    tskid_set = tk_cre_tsk(&ctsk_set);         // Create notification task
    (void)tk_sta_tsk(tskid_set, 0);            // Start notification task

    (void)tk_slp_tsk(TMO_FEVR);                // Sleep forever
    return 0;
}
