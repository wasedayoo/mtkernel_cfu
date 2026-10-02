// SimRV task example

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

// Task 1 configuration
LOCAL void task1_main(INT stacd, void *exinf); // Entry function
LOCAL ID        tskid_task1;                   // Task ID
LOCAL T_CTSK ctsk_task1 = {
    .itskpri = 10,                    // Initial priority
    .stksz   = 1024,                  // Stack size
    .task    = (FP)task1_main,        // Entry function
    .tskatr  = TA_HLNG | TA_RNG3,     // Task attributes
};

// Task 2 configuration
LOCAL void task2_main(INT stacd, void *exinf); // Entry function
LOCAL ID        tskid_task2;                  // Task ID
LOCAL T_CTSK ctsk_task2 = {
    .itskpri = 10,                    // Initial priority
    .stksz   = 1024,                  // Stack size
    .task    = (FP)task2_main,        // Entry function
    .tskatr  = TA_HLNG | TA_RNG3,     // Task attributes
};

// Task 3 configuration
LOCAL void task3_main(INT stacd, void *exinf); // Entry function
LOCAL ID        tskid_task3;                  // Task ID
LOCAL T_CTSK ctsk_task3 = {
    .itskpri = 10,                    // Initial priority
    .stksz   = 1024,                  // Stack size
    .task    = (FP)task3_main,        // Entry function
    .tskatr  = TA_HLNG | TA_RNG3,     // Task attributes
};

// Task 1
LOCAL void task1_main(INT stacd, void *exinf)
{
    (void)stacd;
    (void)exinf;

    while (1) {
        tm_putstring((const UB *)"task1\n"); // Console output
        (void)tk_dly_tsk(1000U);              // 1 s delay
    }

    tk_ext_tsk(); // Unreachable
}

// Task 2
LOCAL void task2_main(INT stacd, void *exinf)
{
    (void)stacd;
    (void)exinf;

    while (1) {
        tm_putstring((const UB *)"task2\n"); // Console output
        (void)tk_dly_tsk(2000U);              // 2 s delay
    }

    tk_ext_tsk(); // Unreachable
}

// Task 3
LOCAL void task3_main(INT stacd, void *exinf)
{
    (void)stacd;
    (void)exinf;

    while (1) {
        tm_putstring((const UB *)"task3\n"); // Console output
        (void)tk_dly_tsk(3000U);              // 3 s delay
    }

    tk_ext_tsk(); // Unreachable
}

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    tskid_task1 = tk_cre_tsk(&ctsk_task1);  // Create task 1
    (void)tk_sta_tsk(tskid_task1, 0);       // Start task 1

    tskid_task2 = tk_cre_tsk(&ctsk_task2);  // Create task 2
    (void)tk_sta_tsk(tskid_task2, 0);       // Start task 2

    tskid_task3 = tk_cre_tsk(&ctsk_task3);  // Create task 3
    (void)tk_sta_tsk(tskid_task3, 0);       // Start task 3

    (void)tk_slp_tsk(TMO_FEVR);             // Sleep forever
    return 0;                               // Unreachable
}
