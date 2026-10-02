// Mailbox example: pass a message object between two tasks

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

// Application message: mailbox sends its address, not a copy
typedef struct {
    T_MSG header;
    INT value;
} APP_MESSAGE;

LOCAL ID mbxid_data;
LOCAL APP_MESSAGE message = {
    .value = 5678,
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

// Receive the address of the message object
LOCAL void task_receiver(INT stacd, void *exinf)
{
    T_MSG *received_message;

    (void)stacd; (void)exinf;
    tm_putstring((const UB *)"receiver: waiting for mailbox\n");
    (void)tk_rcv_mbx(mbxid_data, &received_message, TMO_FEVR);
    tm_printf((const UB *)"receiver: value = %d\n",
              ((APP_MESSAGE *)received_message)->value);
    (void)tk_del_mbx(mbxid_data);
    tk_ext_tsk();
}

// Send the message object's address
LOCAL void task_sender(INT stacd, void *exinf)
{
    (void)stacd; (void)exinf;
    (void)tk_dly_tsk(20U);
    tm_putstring((const UB *)"sender: send mailbox message\n");
    (void)tk_snd_mbx(mbxid_data, &message.header);
    tk_ext_tsk();
}

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    T_CMBX cmbx_data = {
        .mbxatr = TA_TFIFO | TA_MFIFO,
    };
    ID tskid;

    mbxid_data = tk_cre_mbx(&cmbx_data);      // Create mailbox
    tskid = tk_cre_tsk(&ctsk_receiver);
    (void)tk_sta_tsk(tskid, 0);
    tskid = tk_cre_tsk(&ctsk_sender);
    (void)tk_sta_tsk(tskid, 0);

    (void)tk_slp_tsk(TMO_FEVR);
    return 0;
}
