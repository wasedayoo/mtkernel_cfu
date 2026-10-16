// FENCE.I example: generate a small function in RAM and execute it

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

#define INITIAL_RESULT          1
#define GENERATED_RESULT       42
#define RISCV_ADDI_A0_ZERO_1   0x00100513U
#define RISCV_ADDI_A0_ZERO_42  0x02a00513U
#define RISCV_RET              0x00008067U

typedef INT (*GENERATED_FUNCTION)(void);

// Executable RAM image: initially returns 1, then is rewritten to return 42
LOCAL UW generated_code[2] __attribute__((aligned(4))) = {
    RISCV_ADDI_A0_ZERO_1,
    RISCV_RET,
};

// Code-generation task configuration
LOCAL void task_codegen(INT stacd, void *exinf); // Entry function
LOCAL T_CTSK ctsk_codegen = {
    .itskpri = 10,                             // Initial priority
    .stksz   = 1024,                           // Stack size
    .task    = (FP)task_codegen,               // Entry function
    .tskatr  = TA_HLNG | TA_RNG3,              // Task attributes
};

// Write two RISC-V instructions into RAM, synchronize, and execute them
LOCAL void task_codegen(INT stacd, void *exinf)
{
    GENERATED_FUNCTION generated_function;
    INT initial_result;
    INT result;

    (void)stacd;
    (void)exinf;

    tm_putstring((const UB *)"FENCE.I stale-code test start\n");

    /* Execute the preloaded version so that it can remain in the I-cache. */
    generated_function = (GENERATED_FUNCTION)(void *)generated_code;
    initial_result = generated_function();
    tm_printf((const UB *)"initial generated function returned %d\n",
              initial_result);

    if (initial_result != INITIAL_RESULT) {
        tm_putstring((const UB *)"FAIL: initial generated code was not executed\n");
        tk_ext_tsk();
    }

    /* Rewrite the same address.  FENCE.I must discard the stale instructions. */
    generated_code[0] = RISCV_ADDI_A0_ZERO_42; // addi a0, zero, 42

    /* GCC emits FENCE.I for this instruction-cache synchronization request. */
    __builtin___clear_cache((char *)&generated_code[0],
                            (char *)&generated_code[2]);

    result = generated_function();             // Execute rewritten RAM code

    tm_printf((const UB *)"rewritten generated function returned %d\n", result);
    if (result == GENERATED_RESULT) {
        tm_putstring((const UB *)"PASS: rewritten code executed after FENCE.I\n");
    } else if (result == INITIAL_RESULT) {
        tm_putstring((const UB *)"FAIL: stale code executed; FENCE.I was ineffective\n");
    } else {
        tm_putstring((const UB *)"FAIL: rewritten code returned wrong value\n");
    }

    tk_ext_tsk();                               // Exit code-generation task
}

// usermain function
WEAK_FUNC EXPORT INT usermain(void)
{
    ID tskid_codegen;

    tskid_codegen = tk_cre_tsk(&ctsk_codegen); // Create code-generation task
    (void)tk_sta_tsk(tskid_codegen, 0);         // Start task

    (void)tk_slp_tsk(TMO_FEVR);                 // Sleep forever
    return 0;                                   // Unreachable
}
