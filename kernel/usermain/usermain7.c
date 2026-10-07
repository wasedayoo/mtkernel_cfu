// Mailbox example: pass a message object between two tasks

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

// Application message: mailbox sends its address, not a copy
typedef struct {
    T_MSG header;                              // Mailbox message header
    INT value;                                // Application data
} APP_MESSAGE;

// Mailbox data
LOCAL ID mbxid_data;                          // Mailbox ID
LOCAL APP_MESSAGE message = {
    .value = 5678,                            // Value to send
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

// Receive the address of the message object
LOCAL void task_receiver(INT stacd, void *exinf)
{
    T_MSG *received_message;                  // Received message address

    (void)stacd;
    (void)exinf;
    tm_putstring((const UB *)"receiver: waiting for mailbox\n");
    (void)tk_rcv_mbx(mbxid_data, &received_message, TMO_FEVR); // Wait for pointer
    tm_printf((const UB *)"receiver: value = %d\n",
              ((APP_MESSAGE *)received_message)->value);
    (void)tk_del_mbx(mbxid_data);             // Delete mailbox
    tk_ext_tsk();                              // Exit receiver task
}

// Send the message object's address
LOCAL void task_sender(INT stacd, void *exinf)
{
    (void)stacd;
    (void)exinf;
    (void)tk_dly_tsk(1000U);                    // Allow receiver to enter WAIT
    tm_putstring((const UB *)"sender: send mailbox message\n");
    (void)tk_snd_mbx(mbxid_data, &message.header); // Send message address
    tk_ext_tsk();                              // Exit sender task
}

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    T_CMBX cmbx_data = {
        .mbxatr = TA_TFIFO | TA_MFIFO,        // FIFO tasks and messages
    };
    ID tskid;

    mbxid_data = tk_cre_mbx(&cmbx_data);      // Create mailbox
    tskid = tk_cre_tsk(&ctsk_receiver);        // Create receiver task
    (void)tk_sta_tsk(tskid, 0);                // Start receiver task
    tskid = tk_cre_tsk(&ctsk_sender);          // Create sender task
    (void)tk_sta_tsk(tskid, 0);                // Start sender task

    (void)tk_slp_tsk(TMO_FEVR);                // Sleep forever
    return 0;                                  // Unreachable
}
