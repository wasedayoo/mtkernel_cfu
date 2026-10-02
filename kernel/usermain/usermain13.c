// Message buffer example: transfer text between two tasks

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

// Message buffer configuration
LOCAL ID mbfid_text;
LOCAL T_CMBF cmbf_text = {
    .mbfatr = TA_TFIFO,
    .bufsz  = 64,
    .maxmsz = 32,
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

// Wait for and display one message
LOCAL void task_receiver(INT stacd, void *exinf)
{
    UB text[32];

    (void)stacd; (void)exinf;
    tm_putstring((const UB *)"receiver: waiting for message\n");
    (void)tk_rcv_mbf(mbfid_text, text, TMO_FEVR);
    tm_printf((const UB *)"receiver: %s\n", text);
    (void)tk_del_mbf(mbfid_text);
    tk_ext_tsk();
}

// Send one message after a short delay
LOCAL void task_sender(INT stacd, void *exinf)
{
    const UB text[] = "message from sender task";

    (void)stacd; (void)exinf;
    (void)tk_dly_tsk(20U);
    tm_putstring((const UB *)"sender: send message\n");
    (void)tk_snd_mbf(mbfid_text, text, (INT)sizeof(text), TMO_FEVR);
    tk_ext_tsk();
}

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    ID tskid;

    mbfid_text = tk_cre_mbf(&cmbf_text);      // Create message buffer
    tskid = tk_cre_tsk(&ctsk_receiver);
    (void)tk_sta_tsk(tskid, 0);
    tskid = tk_cre_tsk(&ctsk_sender);
    (void)tk_sta_tsk(tskid, 0);

    (void)tk_slp_tsk(TMO_FEVR);
    return 0;
}
