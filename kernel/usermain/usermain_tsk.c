// Task limit example: create tasks with arrays until the kernel limit is reached

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

#define TASK_TRIAL_COUNT  40                   // More than CNF_MAX_TSKID (= 32)
#define TASK_STACK_SIZE   1024

// Task IDs and creation information can both be stored in arrays
LOCAL ID task_ids[TASK_TRIAL_COUNT];
LOCAL T_CTSK task_configs[TASK_TRIAL_COUNT];
LOCAL volatile INT completed_tasks;

// Common entry function used by every created task
LOCAL void task_main(INT stacd, void *exinf)
{
    INT index = stacd;

    (void)exinf;
    tm_printf((const UB *)"task[%d]: started, ID = %d\n",
              index, task_ids[index]);
    ++completed_tasks;
    tk_ext_tsk();                              // Finish and enter DORMANT state
}

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    INT created_count = 0;
    INT i;
    ID result = E_OK;

    // Prepare one T_CTSK element for each task.
    for (i = 0; i < TASK_TRIAL_COUNT; ++i) {
        task_configs[i].itskpri = 10;          // Initial priority
        task_configs[i].stksz   = TASK_STACK_SIZE;
        task_configs[i].task    = (FP)task_main;
        task_configs[i].tskatr  = TA_HLNG | TA_RNG3;
    }

    // Continue creating tasks until tk_cre_tsk reports the limit.
    for (i = 0; i < TASK_TRIAL_COUNT; ++i) {
        result = tk_cre_tsk(&task_configs[i]);
        if (result <= 0) {
            break;
        }
        task_ids[i] = result;
        ++created_count;
    }

    tm_printf((const UB *)"created user tasks = %d\n", created_count);
    tm_printf((const UB *)"next tk_cre_tsk result = %d\n", result);

    // Start all tasks stored in the task-ID array.
    for (i = 0; i < created_count; ++i) {
        (void)tk_sta_tsk(task_ids[i], i);      // Pass the array index as stacd
    }

    // Wait until every task has executed task_main and become DORMANT.
    while (completed_tasks < created_count) {
        (void)tk_dly_tsk(10U);
    }

    // DORMANT tasks can be deleted, releasing their stacks and TCBs.
    for (i = 0; i < created_count; ++i) {
        (void)tk_del_tsk(task_ids[i]);
    }
    tm_printf((const UB *)"deleted user tasks = %d\n", created_count);

    (void)tk_slp_tsk(TMO_FEVR);               // Sleep forever
    return 0;
}
