# micro T-Kernel API examples for SimRV

`usermain2.c` through `usermain19.c` are small, independent application
examples written in the same style as `usermain0.c`.

| Source | Application behavior |
| --- | --- |
| `usermain2.c` | Notify one task with an event flag |
| `usermain3.c` | Wait until two sensor-ready bits are set |
| `usermain4.c` | Clear one event-flag status bit |
| `usermain5.c` | Broadcast an event to two waiting tasks |
| `usermain6.c` | Notify a waiting task with a semaphore |
| `usermain7.c` | Manage a pool of two resources with a semaphore |
| `usermain8.c` | Borrow and return one semaphore-controlled resource |
| `usermain9.c` | Protect one shared value with a mutex |
| `usermain10.c` | Let two tasks update a shared counter safely |
| `usermain11.c` | Make a worker wait for a mutex held by main |
| `usermain12.c` | Send and receive text through a message buffer |
| `usermain13.c` | Transfer text between two tasks |
| `usermain14.c` | Send a message object through a mailbox |
| `usermain15.c` | Pass a mailbox message between two tasks |
| `usermain16.c` | Run a one-shot alarm handler |
| `usermain17.c` | Cancel a scheduled alarm |
| `usermain18.c` | Start and stop a periodic cyclic handler |
| `usermain19.c` | Start a cyclic heartbeat with `TA_STA` |

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
