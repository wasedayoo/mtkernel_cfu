// Message buffer example: send and receive a short text

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    T_CMBF cmbf_text = {
        .mbfatr = TA_TFIFO,                   // FIFO waiting order
        .bufsz  = 64,                         // Internal buffer size
        .maxmsz = 32,                         // Maximum message size
    };
    const UB send_text[] = "hello from message buffer";
    UB receive_text[32];
    ID mbfid_text;

    mbfid_text = tk_cre_mbf(&cmbf_text);      // Create message buffer

    (void)tk_snd_mbf(mbfid_text, send_text,
                     (INT)sizeof(send_text), TMO_FEVR); // Send text
    (void)tk_rcv_mbf(mbfid_text, receive_text, TMO_FEVR); // Receive text
    tm_printf((const UB *)"received: %s\n", receive_text);

    (void)tk_del_mbf(mbfid_text);             // Delete message buffer
    (void)tk_slp_tsk(TMO_FEVR);
    return 0;
}
