# micro T-Kernel API examples for SimRV

`usermain2.c` through `usermain9.c` are small, independent application
examples written in the same style as `usermain0.c`.

| Source | Application behavior |
| --- | --- |
| `usermain2.c` | Notify one task with an event flag |
| `usermain3.c` | Set and clear event-flag status bits |
| `usermain4.c` | Notify a waiting task with a semaphore |
| `usermain5.c` | Make a worker wait for a mutex held by another task |
| `usermain6.c` | Transfer text between two tasks |
| `usermain7.c` | Pass a mailbox message between two tasks |
| `usermain8.c` | Run one alarm and cancel another scheduled alarm |
| `usermain9.c` | Start and stop a periodic cyclic handler |

Event flags, semaphores, mutexes, message buffers, and mailboxes are ready for
use as soon as their `tk_cre_*` call succeeds. They do not have a separate
start API. Tasks need `tk_sta_tsk`, alarms need `tk_sta_alm`, and a cyclic
handler without `TA_STA` needs `tk_sta_cyc`.

The micro T-Kernel API name is `tk_del_mtx`; `tk_del_mtax` is a typo.

## Build one example

Run the following from `mtkernel-simrv`, replacing `2` with the desired
example number:

```sh
make -B APP_SOURCE=../mtkernel_cfu/kernel/usermain/usermain2.c all
```

## Run one example

```sh
../SimRV/build/rv32-release/SimRV \
  -b -m build/mtkernel-simrv.elf \
  --misa rv32imac --ia --cli --steps 120000000
```

Examples that contain delays or timer handlers need more simulated
instructions than examples that only call an object API directly.
