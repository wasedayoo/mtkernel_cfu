// Mailbox example: send a message object without copying it

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

// Application message: T_MSG must be the first member
typedef struct {
    T_MSG header;
    INT value;
} APP_MESSAGE;

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    T_CMBX cmbx_data = {
        .mbxatr = TA_TFIFO | TA_MFIFO,        // FIFO tasks and messages
    };
    APP_MESSAGE send_message = {
        .value = 1234,
    };
    T_MSG *received_message;
    ID mbxid_data;

    mbxid_data = tk_cre_mbx(&cmbx_data);      // Create mailbox

    (void)tk_snd_mbx(mbxid_data, &send_message.header); // Send pointer
    (void)tk_rcv_mbx(mbxid_data, &received_message, TMO_FEVR);
    tm_printf((const UB *)"received value: %d\n",
              ((APP_MESSAGE *)received_message)->value);

    (void)tk_del_mbx(mbxid_data);             // Delete mailbox
    (void)tk_slp_tsk(TMO_FEVR);
    return 0;
}
