// Event flag example: set, clear, wait for, and delete a flag

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

// Event flag configuration
LOCAL ID flgid_status;                        // Event flag ID
LOCAL T_CFLG cflg_status = {
    .flgatr  = TA_TFIFO | TA_WSGL,            // FIFO, one waiting task
    .iflgptn = 0U,                            // Initial pattern: no event
};

// Status task configuration
LOCAL void task_status(INT stacd, void *exinf); // Entry function
LOCAL T_CTSK ctsk_status = {
    .itskpri = 10,                            // Initial priority
    .stksz   = 1024,                          // Stack size
    .task    = (FP)task_status,               // Entry function
    .tskatr  = TA_HLNG | TA_RNG3,             // Task attributes
};

// Set two bits, clear bit 0, and confirm that bit 1 remains set
LOCAL void task_status(INT stacd, void *exinf)
{
    UINT pattern;

    (void)stacd;
    (void)exinf;

    (void)tk_set_flg(flgid_status, 0x03U);     // Set status bits 0 and 1
    tm_putstring((const UB *)"status: bits 0 and 1 are set\n");

    // tk_clr_flg keeps the bits that are 1 in the mask.
    (void)tk_clr_flg(flgid_status, ~0x01U);    // Clear only bit 0
    (void)tk_wai_flg(flgid_status, 0x02U,
                     TWF_ORW, &pattern, TMO_POL);
    tm_printf((const UB *)"status: remaining pattern = 0x%x\n", pattern);

    (void)tk_del_flg(flgid_status);            // Delete event flag
    tk_ext_tsk();                              // Exit status task
}

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    ID tskid_status;

    flgid_status = tk_cre_flg(&cflg_status);   // Create event flag

    tskid_status = tk_cre_tsk(&ctsk_status);   // Create status task
    (void)tk_sta_tsk(tskid_status, 0);         // Start status task

    (void)tk_slp_tsk(TMO_FEVR);                // Sleep forever
    return 0;                                  // Unreachable
}
