// Mutex example: two tasks update the same counter safely

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

// Shared data and mutex
LOCAL ID mtxid_counter;
LOCAL INT shared_counter;

// Worker task
LOCAL void task_counter(INT stacd, void *exinf)
{
    INT i;

    (void)exinf;
    for (i = 0; i < 3; ++i) {
        (void)tk_loc_mtx(mtxid_counter, TMO_FEVR); // Enter critical section
        ++shared_counter;
        tm_printf((const UB *)"task %d: counter = %d\n", stacd, shared_counter);
        (void)tk_unl_mtx(mtxid_counter);           // Leave critical section
    }
    tk_ext_tsk();
}

// Worker task configuration
LOCAL T_CTSK ctsk_counter = {
    .itskpri = 10,
    .stksz   = 1024,
    .task    = (FP)task_counter,
    .tskatr  = TA_HLNG | TA_RNG3,
};

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    T_CMTX cmtx_counter = {
        .mtxatr = TA_TFIFO,
        .ceilpri = 0,
    };
    ID task1;
    ID task2;

    mtxid_counter = tk_cre_mtx(&cmtx_counter);
    task1 = tk_cre_tsk(&ctsk_counter);
    (void)tk_sta_tsk(task1, 1);
    task2 = tk_cre_tsk(&ctsk_counter);
    (void)tk_sta_tsk(task2, 2);

    (void)tk_dly_tsk(20U);                    // Wait for both tasks to finish
    (void)tk_del_mtx(mtxid_counter);
    (void)tk_slp_tsk(TMO_FEVR);
    return 0;
}
