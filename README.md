# Homogeneous SoC process escalation firmwares

The simplest ARM Cortex-M4 firmwares to escalate process privileges on the Linux operating system.

Build the firmwares and the victim process with:
```sh
$ make
```

1. copy particular `victim` program to the board
2. run `victim` under the user with uid 1000.
3. run the `escalate` firmware according to the board instruction

## NXP i.MX8MQ

## STM32MP1

1. copy `firmware/escalation.stm32mp1.elf` to the board's `/lib/firmware/rproc-m4-fw`
```
pc$ scp firmware/escalation.stm32mp1.elf stm:/lib/firmware/rproc-m4-fw
```
2. start the core via remoteproc
```c
stm$ echo start > /sys/class/remoteproc/remoteproc0/state
```
3. (optionally) watch the progress from the trace log
```c
stm$ watch tail /sys/kernel/debug/remoteproc/remoteproc0/trace0
```
