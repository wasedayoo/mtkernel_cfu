// Semaphore example: notify a waiting task

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

// Semaphore configuration
LOCAL ID semid_ready;
LOCAL T_CSEM csem_ready = {
    .sematr  = TA_TFIFO | TA_FIRST,           // FIFO waiting order
    .isemcnt = 0,                             // Initially unavailable
    .maxsem  = 1,                             // Maximum count
};

// Task entry functions
LOCAL void task_receiver(INT stacd, void *exinf);
LOCAL void task_sender(INT stacd, void *exinf);

// Task configurations
LOCAL T_CTSK ctsk_receiver = {
    .itskpri = 10, .stksz = 1024,
    .task = (FP)task_receiver, .tskatr = TA_HLNG | TA_RNG3,
};
LOCAL T_CTSK ctsk_sender = {
    .itskpri = 10, .stksz = 1024,
    .task = (FP)task_sender, .tskatr = TA_HLNG | TA_RNG3,
};

// Wait until one semaphore count is supplied
LOCAL void task_receiver(INT stacd, void *exinf)
{
    (void)stacd; (void)exinf;
    tm_putstring((const UB *)"receiver: waiting\n");
    (void)tk_wai_sem(semid_ready, 1, TMO_FEVR);
    tm_putstring((const UB *)"receiver: notification received\n");
    (void)tk_del_sem(semid_ready);
    tk_ext_tsk();
}

// Supply one semaphore count
LOCAL void task_sender(INT stacd, void *exinf)
{
    (void)stacd; (void)exinf;
    (void)tk_dly_tsk(20U);
    tm_putstring((const UB *)"sender: signal semaphore\n");
    (void)tk_sig_sem(semid_ready, 1);
    tk_ext_tsk();
}

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    ID tskid;

    semid_ready = tk_cre_sem(&csem_ready);     // Semaphore is usable immediately
    tskid = tk_cre_tsk(&ctsk_receiver);
    (void)tk_sta_tsk(tskid, 0);
    tskid = tk_cre_tsk(&ctsk_sender);
    (void)tk_sta_tsk(tskid, 0);

    (void)tk_slp_tsk(TMO_FEVR);
    return 0;
}
