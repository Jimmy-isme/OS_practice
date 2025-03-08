#include "header/dtb.h"
#include "header/uart.h"
#include "header/utils.h"
#define UNUSED(x) (void)(x)

extern char * cpio_addr;
int space = 0;

// 將 32 位元整數從小端轉換成大端
uint32_t fdt_u32_le2be (const void *addr) {
    const uint8_t *bytes = (const uint8_t *) addr;
    uint32_t ret = (uint32_t)bytes[0] << 24 | (uint32_t)bytes[1] << 16 |
                   (uint32_t)bytes[2] << 8  | (uint32_t)bytes[3];
    return ret;
}

void send_space(int n) {
    while(n--) uart_puts(" ");
}

int parse_struct (fdt_callback cb, uintptr_t cur_ptr, uintptr_t strings_ptr, uint32_t totalsize) {
    uintptr_t end_ptr = cur_ptr + totalsize;
    
    while(cur_ptr < end_ptr) {
        uint32_t token = fdt_u32_le2be((char *)cur_ptr);
        cur_ptr += 4;
        
        switch(token) {
            case FDT_BEGIN_NODE: {
                // 傳入節點名稱 (要包含結尾的 '\0')，並對齊到 4 字節
                cb(token, (char*)cur_ptr, NULL, 0);
                cur_ptr += align_up(strlen((char*)cur_ptr) + 1, 4);
                break;
            }
            case FDT_END_NODE:
                cb(token, NULL, NULL, 0);
                break;
            case FDT_PROP: {
                uint32_t len = fdt_u32_le2be((char*)cur_ptr);
                cur_ptr += 4;
                uint32_t nameoff = fdt_u32_le2be((char*)cur_ptr);
                cur_ptr += 4;
                cb(token, (char*)(strings_ptr + nameoff), (void*)cur_ptr, len);
                cur_ptr += align_up(len, 4);
                break;
            }
            case FDT_NOP:
                cb(token, NULL, NULL, 0);
                break;
            case FDT_END:
                cb(token, NULL, NULL, 0);
                return 0;
            default:
                return -1;
        }
    }
    return -1;
}

int fdt_traverse(fdt_callback cb, void * _dtb) {
    uintptr_t dtb_ptr = (uintptr_t) _dtb;
    uart_puts("\ndtb loading at: ");
    uart_send_hex(dtb_ptr);
    uart_send('\n');

    struct fdt_header* header = (struct fdt_header*) dtb_ptr;
    uint32_t magic = fdt_u32_le2be(&(header->magic));	
    if (magic != 0xd00dfeed) {
        uart_puts("The header magic is wrong\n");
        return -1;
    }
    
    uintptr_t struct_ptr = dtb_ptr + fdt_u32_le2be(&(header->off_dt_struct));
    uintptr_t strings_ptr = dtb_ptr + fdt_u32_le2be(&(header->off_dt_strings));
    uint32_t totalsize = fdt_u32_le2be(&header->totalsize);
    parse_struct(cb, struct_ptr, strings_ptr, totalsize);
    return 1;
}

// 修正：只有在屬性名稱精確匹配 "linux,initrd-start" 時才處理
void get_cpio_addr(int token, const char* name, const void* data, uint32_t size) {
    UNUSED(size);
    if (token == FDT_PROP && string_compare((char *)name, "linux,initrd-start") == 0) {
        uint32_t *addr = (uint32_t*)data;  // 取得 data 內的數值
        cpio_addr = (char*)(uintptr_t)fdt_u32_le2be(addr);
        uart_puts("cpio address is at: ");
        uart_send_hex((uintptr_t)cpio_addr);
        uart_send('\n');
    }
}

void print_dtb(int token, const char* name, const void* data, uint32_t size) {
    UNUSED(data);
    UNUSED(size);
    switch(token) {
        case FDT_BEGIN_NODE:
            uart_puts("\n");
            send_space(space);
            uart_puts((char*)name);
            uart_puts(" {\n");
            space++;
            break;
        case FDT_END_NODE:
            uart_puts("\n");
            space--;
            if(space > 0) send_space(space);
            uart_puts("}\n");
            break;
        case FDT_NOP:
            break;
        case FDT_PROP:
            send_space(space);
            uart_puts((char*)name);
            uart_puts("\n");
            break;
        case FDT_END:
            break;
    }
}

