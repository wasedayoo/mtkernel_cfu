// Semaphore example: notify a waiting task

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

// Semaphore configuration
LOCAL ID semid_ready;                         // Semaphore ID
LOCAL T_CSEM csem_ready = {
    .sematr  = TA_TFIFO | TA_FIRST,           // FIFO waiting order
    .isemcnt = 0,                             // Initially unavailable
    .maxsem  = 1,                             // Maximum count
};

// Receiver task configuration
LOCAL void task_receiver(INT stacd, void *exinf); // Entry function
LOCAL T_CTSK ctsk_receiver = {
    .itskpri = 10,                            // Initial priority
    .stksz   = 1024,                          // Stack size
    .task    = (FP)task_receiver,             // Entry function
    .tskatr  = TA_HLNG | TA_RNG3,             // Task attributes
};

// Sender task configuration
LOCAL void task_sender(INT stacd, void *exinf); // Entry function
LOCAL T_CTSK ctsk_sender = {
    .itskpri = 10,                            // Initial priority
    .stksz   = 1024,                          // Stack size
    .task    = (FP)task_sender,               // Entry function
    .tskatr  = TA_HLNG | TA_RNG3,             // Task attributes
};

// Wait until one semaphore count is supplied
LOCAL void task_receiver(INT stacd, void *exinf)
{
    (void)stacd;
    (void)exinf;
    tm_putstring((const UB *)"receiver: waiting\n");
    (void)tk_wai_sem(semid_ready, 1, TMO_FEVR); // Wait for one count
    tm_putstring((const UB *)"receiver: notification received\n");
    (void)tk_del_sem(semid_ready);             // Delete semaphore
    tk_ext_tsk();                              // Exit receiver task
}

// Supply one semaphore count
LOCAL void task_sender(INT stacd, void *exinf)
{
    (void)stacd;
    (void)exinf;
    (void)tk_dly_tsk(1000U);                    // Allow receiver to enter WAIT
    tm_putstring((const UB *)"sender: signal semaphore\n");
    (void)tk_sig_sem(semid_ready, 1);          // Supply one semaphore count
    tk_ext_tsk();                              // Exit sender task
}

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    ID tskid;

    semid_ready = tk_cre_sem(&csem_ready);     // Create semaphore
    tskid = tk_cre_tsk(&ctsk_receiver);        // Create receiver task
    (void)tk_sta_tsk(tskid, 0);                // Start receiver task
    tskid = tk_cre_tsk(&ctsk_sender);          // Create sender task
    (void)tk_sta_tsk(tskid, 0);                // Start sender task

    (void)tk_slp_tsk(TMO_FEVR);                // Sleep forever
    return 0;                                  // Unreachable
}
