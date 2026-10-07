// Mutex example: a worker waits until another task releases the mutex

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

// Mutex configuration
LOCAL ID mtxid_work;                          // Mutex ID
LOCAL T_CMTX cmtx_work = {
    .mtxatr  = TA_TFIFO,                      // FIFO lock waiting order
    .ceilpri = 0,                             // Unused for TA_TFIFO
};

// Holder task configuration
LOCAL void task_holder(INT stacd, void *exinf); // Entry function
LOCAL T_CTSK ctsk_holder = {
    .itskpri = 10,                            // Initial priority
    .stksz   = 1024,                          // Stack size
    .task    = (FP)task_holder,               // Entry function
    .tskatr  = TA_HLNG | TA_RNG3,             // Task attributes
};

// Worker task configuration
LOCAL void task_worker(INT stacd, void *exinf); // Entry function
LOCAL T_CTSK ctsk_worker = {
    .itskpri = 10,                            // Initial priority
    .stksz   = 1024,                          // Stack size
    .task    = (FP)task_worker,               // Entry function
    .tskatr  = TA_HLNG | TA_RNG3,             // Task attributes
};

// Lock the mutex first, hold it briefly, and then release it
LOCAL void task_holder(INT stacd, void *exinf)
{
    (void)stacd;
    (void)exinf;

    (void)tk_loc_mtx(mtxid_work, TMO_FEVR);   // Lock mutex first
    tm_putstring((const UB *)"holder: mutex acquired\n");
    (void)tk_dly_tsk(20U);                    // Worker blocks during this delay
    tm_putstring((const UB *)"holder: release mutex\n");
    (void)tk_unl_mtx(mtxid_work);             // Wake waiting worker
    tk_ext_tsk();                              // Exit holder task
}

// Wait for the mutex, do the work, and release it
LOCAL void task_worker(INT stacd, void *exinf)
{
    (void)stacd;
    (void)exinf;
    tm_putstring((const UB *)"worker: waiting for mutex\n");
    (void)tk_loc_mtx(mtxid_work, TMO_FEVR);   // Wait until holder unlocks
    tm_putstring((const UB *)"worker: mutex acquired\n");
    (void)tk_unl_mtx(mtxid_work);             // Unlock mutex
    (void)tk_del_mtx(mtxid_work);             // Delete mutex
    tk_ext_tsk();                              // Exit worker task
}

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    ID tskid_holder;
    ID tskid_worker;

    mtxid_work = tk_cre_mtx(&cmtx_work);       // Create mutex

    tskid_holder = tk_cre_tsk(&ctsk_holder);   // Create holder task
    (void)tk_sta_tsk(tskid_holder, 0);         // Start holder task

    tskid_worker = tk_cre_tsk(&ctsk_worker);   // Create worker task
    (void)tk_sta_tsk(tskid_worker, 0);         // Start worker task

    (void)tk_slp_tsk(TMO_FEVR);                // Sleep forever
    return 0;                                  // Unreachable
}
