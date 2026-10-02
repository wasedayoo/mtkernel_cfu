

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

// Sleeping task configuration
LOCAL void task_sleep(INT stacd, void *exinf); // Entry function
LOCAL ID        tskid_sleep;                   // Task ID
LOCAL T_CTSK ctsk_sleep = {
    .itskpri = 10,                    // Initial priority
    .stksz   = 1024,                  // Stack size
    .task    = (FP)task_sleep,         // Entry function
    .tskatr  = TA_HLNG | TA_RNG3,     // Task attributes
};

// Wakeup task configuration
LOCAL void task_wakeup(INT stacd, void *exinf); // Entry function
LOCAL ID        tskid_wakeup;                   // Task ID
LOCAL T_CTSK ctsk_wakeup = {
    .itskpri = 10,                    // Initial priority
    .stksz   = 1024,                  // Stack size
    .task    = (FP)task_wakeup,        // Entry function
    .tskatr  = TA_HLNG | TA_RNG3,     // Task attributes
};

// Sleeping task
LOCAL void task_sleep(INT stacd, void *exinf)
{
    (void)stacd;
    (void)exinf;

    while (1) {
        tm_putstring((const UB *)"task_sleep: sleep\n");
        (void)tk_slp_tsk(TMO_FEVR);     // Wait for wakeup
        tm_putstring((const UB *)"task_sleep: wake\n");
    }

    tk_ext_tsk(); // Unreachable
}

// Wakeup task
LOCAL void task_wakeup(INT stacd, void *exinf)
{
    (void)stacd;
    (void)exinf;

    while (1) {
        (void)tk_dly_tsk(1000);          // 1 s delay
        tm_putstring((const UB *)"task_wakeup: wake task_sleep\n");
        (void)tk_wup_tsk(tskid_sleep);   // Wake task_sleep
    }

    tk_ext_tsk(); // Unreachable
}

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    tskid_sleep = tk_cre_tsk(&ctsk_sleep);    // Create sleeping task
    (void)tk_sta_tsk(tskid_sleep, 0);         // Start sleeping task

    tskid_wakeup = tk_cre_tsk(&ctsk_wakeup);  // Create wakeup task
    (void)tk_sta_tsk(tskid_wakeup, 0);        // Start wakeup task

    (void)tk_slp_tsk(TMO_FEVR);               // Sleep forever
    return 0;                                 // Unreachable
}
