#include "header/cpio.h"
#include "header/uart.h"
#include "header/utils.h"

#define CPIO_MAGIC "070701"  // CPIO newc 格式的 magic number

char* cpio_addr = (char*)0x8000000;  // QEMU 預設載入 cpio 的位址

// 轉換 16 進制字串為數值
unsigned int hex_to_int(const char *hex, int size) {
    unsigned int val = 0;
    for (int i = 0; i < size; i++) {
        char c = hex[i];
        val <<= 4;
        if (c >= '0' && c <= '9') val |= (c - '0');
        else if (c >= 'A' && c <= 'F') val |= (c - 'A' + 10);
        else if (c >= 'a' && c <= 'f') val |= (c - 'a' + 10); // 高位數跟低位數不相交，|的功能變成了+
    }
    return val;
}

// 搜尋特定檔案
char *findFile(char *name) {
    char *addr = cpio_addr;

    while (string_compare((char *)(addr + sizeof(struct cpio_header)), "TRAILER!!!") != 0) {
        struct cpio_header *header = (struct cpio_header *)addr;

        // 讀取檔名
        char *filename = addr + sizeof(struct cpio_header);

        // 檢查是否是我們要的檔案
        if (string_compare(filename, name) == 0) {
            return addr;  // 找到檔案，回傳該 CPIO Header 起始位址
        }

        // 讀取 Header 內的數據
        unsigned int namesize = hex_to_int(header->c_namesize, sizeof(header->c_namesize));
        unsigned int filesize = hex_to_int(header->c_filesize, sizeof(header->c_filesize));

        // 計算下一個檔案的地址 (對齊 4-byte)
        unsigned int headerPathname_size = sizeof(struct cpio_header) + namesize;
        headerPathname_size = (headerPathname_size + 3) & ~3;
        filesize = (filesize + 3) & ~3;

        addr += (headerPathname_size + filesize); // 移動到下一個檔案
    }

    return 0; // 找不到檔案
}

// 列出所有檔案
void cpio_ls() {
    char *addr = cpio_addr;

    uart_puts("Files in initramfs:\n");

    while (string_compare((char *)(addr + sizeof(struct cpio_header)), "TRAILER!!!") != 0) {
        struct cpio_header *header = (struct cpio_header *)addr;

        // 讀取檔名
        char *filename = addr + sizeof(struct cpio_header);
        uart_puts(filename);
        uart_puts("\n");

        // 計算下一個檔案的地址
        unsigned int namesize = hex_to_int(header->c_namesize, sizeof(header->c_namesize));
        unsigned int filesize = hex_to_int(header->c_filesize, sizeof(header->c_filesize));

        unsigned int headerPathname_size = sizeof(struct cpio_header) + namesize;
        headerPathname_size = (headerPathname_size + 3) & ~3;
        filesize = (filesize + 3) & ~3;

        addr += (headerPathname_size + filesize);
    }
}

// 顯示特定檔案內容
void cpio_cat(char *filename) {
    char *target = findFile(filename);
    if (target) {
        struct cpio_header *header = (struct cpio_header *)target;

        unsigned int namesize = hex_to_int(header->c_namesize, sizeof(header->c_namesize));
        unsigned int filesize = hex_to_int(header->c_filesize, sizeof(header->c_filesize));

        unsigned int headerPathname_size = sizeof(struct cpio_header) + namesize;
        headerPathname_size = (headerPathname_size + 3) & ~3;
        filesize = (filesize + 3) & ~3;

        char *file_content = target + headerPathname_size;

        uart_puts("File content:\n");
        for (unsigned int i = 0; i < filesize; i++) {
            uart_send(file_content[i]);
        }
        uart_puts("\n");
    } else {
        uart_puts("File not found.\n");
    }
}

