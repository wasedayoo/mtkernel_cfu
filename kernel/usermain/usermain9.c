// Mutex example: protect one shared value

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    T_CMTX cmtx_counter = {
        .mtxatr  = TA_TFIFO,                  // FIFO lock waiting order
        .ceilpri = 0,
    };
    INT shared_counter = 0;
    ID mtxid_counter;

    mtxid_counter = tk_cre_mtx(&cmtx_counter); // Create mutex

    (void)tk_loc_mtx(mtxid_counter, TMO_FEVR); // Lock mutex
    ++shared_counter;                           // Access shared data
    tm_printf((const UB *)"counter = %d\n", shared_counter);
    (void)tk_unl_mtx(mtxid_counter);            // Unlock mutex

    (void)tk_del_mtx(mtxid_counter);            // Delete mutex
    (void)tk_slp_tsk(TMO_FEVR);
    return 0;
}
