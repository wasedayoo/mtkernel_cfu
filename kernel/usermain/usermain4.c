// Event flag example: clear one status bit

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    T_CFLG cflg_status = {
        .flgatr  = TA_TFIFO | TA_WSGL,
        .iflgptn = 0U,
    };
    UINT pattern;
    ID flgid_status;

    flgid_status = tk_cre_flg(&cflg_status);   // Create event flag

    (void)tk_set_flg(flgid_status, 0x03U);     // Set status bits 0 and 1
    tm_putstring((const UB *)"status: bits 0 and 1 are set\n");

    // tk_clr_flg keeps the bits that are 1 in the mask.
    (void)tk_clr_flg(flgid_status, ~0x01U);    // Clear only bit 0
    (void)tk_wai_flg(flgid_status, 0x02U,
                     TWF_ORW, &pattern, TMO_POL);
    tm_printf((const UB *)"status: remaining pattern = 0x%x\n", pattern);

    (void)tk_del_flg(flgid_status);            // Delete event flag
    (void)tk_slp_tsk(TMO_FEVR);
    return 0;
}
