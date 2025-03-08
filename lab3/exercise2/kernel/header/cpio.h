#ifndef _CPIO_H_
#define _CPIO_H_

extern char* cpio_addr;  // CPIO 的起始地址

void cpio_ls();          // 列出所有檔案
void cpio_cat(char* filename);  // 顯示特定檔案內容
char* cpio_findFile(char* name); // 搜尋特定檔案
char *cpio_findFileAddress(char *filename);
unsigned int cpio_get_file_size(char *filename);

// CPIO 檔案格式的 Header 結構
struct cpio_header {
    char c_magic[6];    // Magic number: "070701"
    char c_ino[8];      // Inode number
    char c_mode[8];     // File mode (permissions + type)
    char c_uid[8];      // User ID
    char c_gid[8];      // Group ID
    char c_nlink[8];    // Number of links
    char c_mtime[8];    // Modification time
    char c_filesize[8]; // File size
    char c_devmajor[8]; 
    char c_devminor[8];
    char c_rdevmajor[8];  
    char c_rdevminor[8];
    char c_namesize[8]; // Pathname size
    char c_check[8];    // Checksum (always 0)
};

#endif
