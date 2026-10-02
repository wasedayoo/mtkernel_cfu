// Mutex example: a worker waits until main releases the mutex

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

// Mutex used by both tasks
LOCAL ID mtxid_work;

// Worker task configuration
LOCAL void task_worker(INT stacd, void *exinf);
LOCAL T_CTSK ctsk_worker = {
    .itskpri = 10,
    .stksz   = 1024,
    .task    = (FP)task_worker,
    .tskatr  = TA_HLNG | TA_RNG3,
};

// Wait for the mutex, do the work, and release it
LOCAL void task_worker(INT stacd, void *exinf)
{
    (void)stacd; (void)exinf;
    tm_putstring((const UB *)"worker: waiting for mutex\n");
    (void)tk_loc_mtx(mtxid_work, TMO_FEVR);
    tm_putstring((const UB *)"worker: mutex acquired\n");
    (void)tk_unl_mtx(mtxid_work);
    (void)tk_del_mtx(mtxid_work);
    tk_ext_tsk();
}

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    T_CMTX cmtx_work = {
        .mtxatr = TA_TFIFO,
        .ceilpri = 0,
    };
    ID tskid_worker;

    mtxid_work = tk_cre_mtx(&cmtx_work);
    (void)tk_loc_mtx(mtxid_work, TMO_FEVR);   // Main locks first

    tskid_worker = tk_cre_tsk(&ctsk_worker);
    (void)tk_sta_tsk(tskid_worker, 0);

    (void)tk_dly_tsk(20U);                    // Worker blocks on the mutex
    tm_putstring((const UB *)"main: release mutex\n");
    (void)tk_unl_mtx(mtxid_work);             // Worker can now continue

    (void)tk_slp_tsk(TMO_FEVR);
    return 0;
}
