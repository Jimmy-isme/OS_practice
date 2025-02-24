ARCH = aarch64
CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
AS = aarch64-linux-gnu-as
OBJCOPY = aarch64-linux-gnu-objcopy

CFLAGS = -ffreestanding -nostdlib -nostartfiles -O2
LDFLAGS = -T linker.ld

SRC = start.S main.c uart.c gpio.c shell.c utils.c mailbox.c reboot.C
OBJS = start.o main.o uart.o gpio.o shell.o utils.o mailbox.o reboot.o

all: kernel8.img

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(AS) $< -o $@

kernel8.elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

kernel8.img: kernel8.elf
	$(OBJCOPY) -O binary kernel8.elf kernel8.img

clean:
	rm -f *.o *.elf *.img

