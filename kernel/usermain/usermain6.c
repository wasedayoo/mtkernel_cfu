// Message buffer example: transfer text between two tasks

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

// Message buffer configuration
LOCAL ID mbfid_text;                          // Message buffer ID
LOCAL T_CMBF cmbf_text = {
    .mbfatr = TA_TFIFO,                       // FIFO waiting order
    .bufsz  = 64,                             // Internal buffer size
    .maxmsz = 32,                             // Maximum message size
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

// Wait for and display one message
LOCAL void task_receiver(INT stacd, void *exinf)
{
    UB text[32];                              // Receive buffer

    (void)stacd;
    (void)exinf;
    tm_putstring((const UB *)"receiver: waiting for message\n");
    (void)tk_rcv_mbf(mbfid_text, text, TMO_FEVR); // Wait for one message
    tm_printf((const UB *)"receiver: %s\n", text);
    (void)tk_del_mbf(mbfid_text);             // Delete message buffer
    tk_ext_tsk();                              // Exit receiver task
}

// Send one message after a short delay
LOCAL void task_sender(INT stacd, void *exinf)
{
    const UB text[] = "message from sender task"; // Message to send

    (void)stacd;
    (void)exinf;
    (void)tk_dly_tsk(1000U);                    // Allow receiver to enter WAIT
    tm_putstring((const UB *)"sender: send message\n");
    (void)tk_snd_mbf(mbfid_text, text, (INT)sizeof(text), TMO_FEVR); // Send text
    tk_ext_tsk();                              // Exit sender task
}

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    ID tskid;

    mbfid_text = tk_cre_mbf(&cmbf_text);      // Create message buffer
    tskid = tk_cre_tsk(&ctsk_receiver);        // Create receiver task
    (void)tk_sta_tsk(tskid, 0);                // Start receiver task
    tskid = tk_cre_tsk(&ctsk_sender);          // Create sender task
    (void)tk_sta_tsk(tskid, 0);                // Start sender task

    (void)tk_slp_tsk(TMO_FEVR);                // Sleep forever
    return 0;                                  // Unreachable
}
