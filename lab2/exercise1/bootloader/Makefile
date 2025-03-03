SRCS_C = $(wildcard *.c)
SRCS_S = $(wildcard *.S)
OBJS_C = $(patsubst %.c, build/%.o, $(SRCS_C))  # 將 .c 轉換為 build/*.o
OBJS_S = $(patsubst %.S, build/%.o, $(SRCS_S))  # 將 .S 轉換為 build/*.o
CFLAGS = -ffreestanding -nostdlib -nostartfiles -O2
LDFLAGS = -T linker.ld

BUILD_DIR = build

all: bootloader.img

# 確保 build/ 目錄存在
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# 編譯 .c 檔案，輸出到 build/
build/%.o: %.c | $(BUILD_DIR)
	aarch64-linux-gnu-gcc $(CFLAGS) -c $< -o $@

# 編譯 .S 檔案，輸出到 build/
build/%.o: %.S | $(BUILD_DIR)
	aarch64-linux-gnu-gcc $(CFLAGS) -c $< -o $@

# 連結所有 .o 檔案，產生 kernel8.img
bootloader.img: $(OBJS_S) $(OBJS_C)
	aarch64-linux-gnu-ld $(OBJS_S) $(OBJS_C) $(LDFLAGS) -o build/bootloader.elf
	aarch64-linux-gnu-objcopy -O binary build/bootloader.elf bootloader.img

clean:
	rm -rf build bootloader.img

