#include "header/mailbox.h"

int mailbox_call(unsigned int *mailbox) {
    unsigned int msg = ((unsigned int)((unsigned long)mailbox) & ~0xF) | MAILBOX_CHANNEL;

    // 等待 Mailbox 可寫入
    while (*MAILBOX_STATUS & MAILBOX_FULL);
    
    // 寫入 Mailbox
    *MAILBOX_WRITE = msg;

    // 等待 Mailbox 回應
    while (1) {
        while (*MAILBOX_STATUS & MAILBOX_EMPTY);
        
        if (*MAILBOX_READ == msg) {
            return mailbox[1] == 0x80000000;  // 檢查回應碼 (成功: 0x80000000)
        }
    }
    return 0;
}

