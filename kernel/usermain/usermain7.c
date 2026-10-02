// Semaphore example: manage a pool containing two resources

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

// Resource-count semaphore configuration
LOCAL ID semid_resource;
LOCAL T_CSEM csem_resource = {
    .sematr  = TA_TFIFO | TA_FIRST,
    .isemcnt = 2,                             // Two resources are available
    .maxsem  = 2,
};

// Each worker borrows and then returns one resource
LOCAL void task_worker(INT stacd, void *exinf)
{
    (void)exinf;
    (void)tk_wai_sem(semid_resource, 1, TMO_FEVR);
    tm_printf((const UB *)"worker %d: resource acquired\n", stacd);
    (void)tk_dly_tsk(20U);
    tm_printf((const UB *)"worker %d: resource released\n", stacd);
    (void)tk_sig_sem(semid_resource, 1);
    tk_ext_tsk();
}

// Worker task configuration
LOCAL T_CTSK ctsk_worker = {
    .itskpri = 10,
    .stksz   = 1024,
    .task    = (FP)task_worker,
    .tskatr  = TA_HLNG | TA_RNG3,
};

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    ID worker1;
    ID worker2;
    ID worker3;

    semid_resource = tk_cre_sem(&csem_resource);

    worker1 = tk_cre_tsk(&ctsk_worker);
    (void)tk_sta_tsk(worker1, 1);
    worker2 = tk_cre_tsk(&ctsk_worker);
    (void)tk_sta_tsk(worker2, 2);
    worker3 = tk_cre_tsk(&ctsk_worker);
    (void)tk_sta_tsk(worker3, 3);              // Waits until a resource is returned

    (void)tk_dly_tsk(100U);
    (void)tk_del_sem(semid_resource);
    (void)tk_slp_tsk(TMO_FEVR);
    return 0;
}
