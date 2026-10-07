// Cyclic handler example: count periodic timer events

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

// Cyclic handler configuration
LOCAL volatile UINT cyclic_count;             // Number of handler calls
LOCAL ID cycid_test;                          // Cyclic handler ID
LOCAL void cyclic_handler(void *exinf);       // Handler function
LOCAL T_CCYC ccyc_test = {
    .exinf  = (void *)&cyclic_count,           // Handler argument
    .cycatr = TA_HLNG,                        // High-level language handler
    .cychdr = (FP)cyclic_handler,              // Handler function
    .cyctim = 10U,                            // 10 ms interval
    .cycphs = 0U,                             // Initial phase
};

// Cyclic test task configuration
LOCAL void task_cyclic(INT stacd, void *exinf); // Entry function
LOCAL T_CTSK ctsk_cyclic = {
    .itskpri = 10,                            // Initial priority
    .stksz   = 1024,                          // Stack size
    .task    = (FP)task_cyclic,               // Entry function
    .tskatr  = TA_HLNG | TA_RNG3,             // Task attributes
};

// Cyclic handler (runs every 10 ms while started)
LOCAL void cyclic_handler(void *exinf)
{
    volatile UINT *count = (volatile UINT *)exinf;
    ++(*count);                                // Keep interrupt-side work short
}

// Start the cyclic handler, observe repeated calls, and stop it
LOCAL void task_cyclic(INT stacd, void *exinf)
{
    (void)stacd;
    (void)exinf;

    tm_printf((const UB *)"cyclic count before start = %u\n", cyclic_count);
    (void)tk_sta_cyc(cycid_test);              // Start cyclic handler
    tm_putstring((const UB *)"cyclic task: handler started\n");

    (void)tk_dly_tsk(1000U);                    // Allow several handler calls
    (void)tk_stp_cyc(cycid_test);              // Stop periodic execution
    tm_printf((const UB *)"cyclic count after stop = %u\n", cyclic_count);

    (void)tk_del_cyc(cycid_test);              // Delete cyclic handler
    tk_ext_tsk();                              // Exit cyclic test task
}

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    ID tskid_cyclic;

    cycid_test = tk_cre_cyc(&ccyc_test);       // Create in stopped state

    tskid_cyclic = tk_cre_tsk(&ctsk_cyclic);   // Create cyclic test task
    (void)tk_sta_tsk(tskid_cyclic, 0);         // Start cyclic test task

    (void)tk_slp_tsk(TMO_FEVR);                // Sleep forever
    return 0;                                  // Unreachable
}
