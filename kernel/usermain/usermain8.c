// Alarm example: run and cancel a one-shot handler

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

// Alarm configuration
LOCAL volatile UINT alarm_count;              // Number of handler calls
LOCAL ID almid_test;                          // Alarm ID
LOCAL void alarm_handler(void *exinf);        // Handler function
LOCAL T_CALM calm_test = {
    .exinf  = (void *)&alarm_count,            // Handler argument
    .almatr = TA_HLNG,                        // High-level language handler
    .almhdr = (FP)alarm_handler,               // Handler function
};

// Alarm test task configuration
LOCAL void task_alarm(INT stacd, void *exinf); // Entry function
LOCAL T_CTSK ctsk_alarm = {
    .itskpri = 10,                            // Initial priority
    .stksz   = 1024,                          // Stack size
    .task    = (FP)task_alarm,                // Entry function
    .tskatr  = TA_HLNG | TA_RNG3,             // Task attributes
};

// Alarm handler (runs in a task-independent context)
LOCAL void alarm_handler(void *exinf)
{
    volatile UINT *count = (volatile UINT *)exinf;
    ++(*count);                                // Keep interrupt-side work short
}

// Start one alarm, then start and cancel a second alarm
LOCAL void task_alarm(INT stacd, void *exinf)
{
    (void)stacd;
    (void)exinf;
    tm_printf((const UB *)"alarm count before first alarm = %u\n", alarm_count);
    tm_putstring((const UB *)"alarm task: start first alarm\n");
    (void)tk_sta_alm(almid_test, 500U);         // Run handler after 500 ms
    (void)tk_dly_tsk(1000U);                    // Wait until handler has run
    tm_printf((const UB *)"alarm count after first alarm = %u\n", alarm_count);

    tm_putstring((const UB *)"alarm task: start second alarm\n");
    (void)tk_sta_alm(almid_test, 1000U);         // Schedule handler again
    (void)tk_dly_tsk(500U);                    // Wait less than alarm time
    (void)tk_stp_alm(almid_test);             // Cancel before it fires
    tm_putstring((const UB *)"alarm task: stop second alarm\n");

    (void)tk_dly_tsk(1000U);                    // Confirm canceled alarm does not run
    tm_printf((const UB *)"alarm count after stop = %u\n", alarm_count);

    (void)tk_del_alm(almid_test);             // Delete alarm
    tk_ext_tsk();                              // Exit alarm test task
}

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    ID tskid_alarm;

    almid_test = tk_cre_alm(&calm_test);       // Create alarm

    tskid_alarm = tk_cre_tsk(&ctsk_alarm);     // Create alarm test task
    (void)tk_sta_tsk(tskid_alarm, 0);          // Start alarm test task

    (void)tk_slp_tsk(TMO_FEVR);                // Sleep forever
    return 0;                                  // Unreachable
}
