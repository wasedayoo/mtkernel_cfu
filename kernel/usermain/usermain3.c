// Event flag example: wait until two sensors are ready

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

#define SENSOR_A_READY  0x01U
#define SENSOR_B_READY  0x02U

// Event flag configuration
LOCAL ID flgid_sensor;
LOCAL T_CFLG cflg_sensor = {
    .flgatr  = TA_TFIFO | TA_WSGL,
    .iflgptn = 0U,
};

// Task entry functions
LOCAL void task_sensor_a(INT stacd, void *exinf);
LOCAL void task_sensor_b(INT stacd, void *exinf);
LOCAL void task_monitor(INT stacd, void *exinf);

// Task configurations
LOCAL T_CTSK ctsk_sensor_a = {
    .itskpri = 10, .stksz = 1024,
    .task = (FP)task_sensor_a, .tskatr = TA_HLNG | TA_RNG3,
};
LOCAL T_CTSK ctsk_sensor_b = {
    .itskpri = 10, .stksz = 1024,
    .task = (FP)task_sensor_b, .tskatr = TA_HLNG | TA_RNG3,
};
LOCAL T_CTSK ctsk_monitor = {
    .itskpri = 10, .stksz = 1024,
    .task = (FP)task_monitor, .tskatr = TA_HLNG | TA_RNG3,
};

// Notify that sensor A is ready
LOCAL void task_sensor_a(INT stacd, void *exinf)
{
    (void)stacd; (void)exinf;
    (void)tk_dly_tsk(10U);
    tm_putstring((const UB *)"sensor A: ready\n");
    (void)tk_set_flg(flgid_sensor, SENSOR_A_READY);
    tk_ext_tsk();
}

// Notify that sensor B is ready
LOCAL void task_sensor_b(INT stacd, void *exinf)
{
    (void)stacd; (void)exinf;
    (void)tk_dly_tsk(20U);
    tm_putstring((const UB *)"sensor B: ready\n");
    (void)tk_set_flg(flgid_sensor, SENSOR_B_READY);
    tk_ext_tsk();
}

// Continue only after both ready bits have been set
LOCAL void task_monitor(INT stacd, void *exinf)
{
    UINT pattern;

    (void)stacd; (void)exinf;
    tm_putstring((const UB *)"monitor: waiting for both sensors\n");
    (void)tk_wai_flg(flgid_sensor,
                     SENSOR_A_READY | SENSOR_B_READY,
                     TWF_ANDW | TWF_CLR, &pattern, TMO_FEVR);
    tm_putstring((const UB *)"monitor: both sensors are ready\n");
    (void)tk_del_flg(flgid_sensor);
    tk_ext_tsk();
}

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    ID tskid;

    flgid_sensor = tk_cre_flg(&cflg_sensor);   // Create event flag

    tskid = tk_cre_tsk(&ctsk_monitor);
    (void)tk_sta_tsk(tskid, 0);
    tskid = tk_cre_tsk(&ctsk_sensor_a);
    (void)tk_sta_tsk(tskid, 0);
    tskid = tk_cre_tsk(&ctsk_sensor_b);
    (void)tk_sta_tsk(tskid, 0);

    (void)tk_slp_tsk(TMO_FEVR);
    return 0;
}
