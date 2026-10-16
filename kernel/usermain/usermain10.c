// Atomic counter example: count events from a task and a timer handler

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

#define TIMER_EVENT_TARGET 5U

// Shared counter data
LOCAL volatile UW event_count;                // Updated atomically by both contexts
LOCAL volatile UW timer_event_count;          // Number of timer-side updates
LOCAL ID cycid_counter;                       // Cyclic handler ID

// Cyclic handler configuration
LOCAL void counter_handler(void *exinf);       // Handler function
LOCAL T_CCYC ccyc_counter = {
    .exinf  = NULL,                            // Handler argument
    .cycatr = TA_HLNG,                         // High-level language handler
    .cychdr = (FP)counter_handler,             // Handler function
    .cyctim = 10U,                             // 10 ms interval
    .cycphs = 0U,                              // Initial phase
};

// Counter task configuration
LOCAL void task_counter(INT stacd, void *exinf); // Entry function
LOCAL T_CTSK ctsk_counter = {
    .itskpri = 10,                             // Initial priority
    .stksz   = 1024,                           // Stack size
    .task    = (FP)task_counter,               // Entry function
    .tskatr  = TA_HLNG | TA_RNG3,              // Task attributes
};

// Add one timer event to the shared counter
LOCAL void counter_handler(void *exinf)
{
    (void)exinf;

    (void)__atomic_fetch_add(&event_count, 1U, __ATOMIC_RELAXED);
    ++timer_event_count;
}

// Add task events while timer events update the same counter
LOCAL void task_counter(INT stacd, void *exinf)
{
    UW task_event_count = 0U;
    UW final_count;
    UW expected_count;

    (void)stacd;
    (void)exinf;

    tm_putstring((const UB *)"atomic counter test start\n");
    (void)tk_sta_cyc(cycid_counter);            // Start timer-side updates

    while (timer_event_count < TIMER_EVENT_TARGET) {
        (void)__atomic_fetch_add(&event_count, 1U, __ATOMIC_RELAXED);
        ++task_event_count;
    }

    (void)tk_stp_cyc(cycid_counter);            // Stop timer-side updates

    final_count = __atomic_load_n(&event_count, __ATOMIC_RELAXED);
    expected_count = task_event_count + timer_event_count;

    tm_printf((const UB *)"task events  = %u\n", task_event_count);
    tm_printf((const UB *)"timer events = %u\n", timer_event_count);
    tm_printf((const UB *)"atomic total = %u\n", final_count);

    if (final_count == expected_count) {
        tm_putstring((const UB *)"PASS: no counter update was lost\n");
    } else {
        tm_printf((const UB *)"FAIL: expected total = %u\n", expected_count);
    }

    tk_ext_tsk();                               // Exit counter task
}

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    ID tskid_counter;

    cycid_counter = tk_cre_cyc(&ccyc_counter);  // Create in stopped state
    tskid_counter = tk_cre_tsk(&ctsk_counter);  // Create counter task
    (void)tk_sta_tsk(tskid_counter, 0);         // Start counter task

    (void)tk_slp_tsk(TMO_FEVR);                 // Sleep forever
    return 0;                                   // Unreachable
}
