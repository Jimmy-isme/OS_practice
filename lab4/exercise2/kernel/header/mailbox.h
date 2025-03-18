#ifndef _MAILBOX_H_
#define _MAILBOX_H_

// Mailbox Channels
#define MAILBOX_BASE        0x3F00B880  // Raspberry Pi 3 Mailbox 地址
#define MAILBOX_READ        ((volatile unsigned int*)(MAILBOX_BASE + 0x00))
#define MAILBOX_STATUS      ((volatile unsigned int*)(MAILBOX_BASE + 0x18))
#define MAILBOX_WRITE       ((volatile unsigned int*)(MAILBOX_BASE + 0x20))

#define MAILBOX_EMPTY       0x40000000  // Mailbox Status: 空
#define MAILBOX_FULL        0x80000000  // Mailbox Status: 滿
#define MAILBOX_CHANNEL     8           // 信箱通道 8 (Property Interface)

// Mailbox Tags
#define TAG_GET_BOARD_REVISION  0x00010002  // 取得主機板版本
#define TAG_GET_MEMORY          0x00010005  // 取得記憶體大小

int mailbox_call(unsigned int *mailbox);

#endif

