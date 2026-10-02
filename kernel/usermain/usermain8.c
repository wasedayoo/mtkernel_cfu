// Semaphore example: borrow and return one resource in usermain

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    T_CSEM csem_ticket = {
        .sematr  = TA_TFIFO | TA_FIRST,
        .isemcnt = 1,                         // One ticket is available
        .maxsem  = 1,
    };
    ID semid_ticket;

    semid_ticket = tk_cre_sem(&csem_ticket);  // Create semaphore

    (void)tk_wai_sem(semid_ticket, 1, TMO_FEVR);
    tm_putstring((const UB *)"main: ticket acquired\n");

    (void)tk_dly_tsk(10U);                    // Use the protected resource

    tm_putstring((const UB *)"main: ticket returned\n");
    (void)tk_sig_sem(semid_ticket, 1);
    (void)tk_del_sem(semid_ticket);           // Delete semaphore

    (void)tk_slp_tsk(TMO_FEVR);
    return 0;
}
