#ifndef SYSCALL_H
#define SYSCALL_H

#define SYS_GETPID  0
#define SYS_UART_READ  1
#define SYS_UART_WRITE  2
#define SYS_EXEC  3
#define SYS_FORK  4
#define SYS_EXIT  5
#define SYS_MBOX_CALL  6
#define SYS_KILL  7

void syscall_handler();

#endif

