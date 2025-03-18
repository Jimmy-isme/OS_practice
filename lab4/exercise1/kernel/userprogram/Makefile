all: ../rootfs/user.img

user.o: user.S
	aarch64-linux-gnu-gcc -c user.S -o user.o

user.elf: user.o linker.ld
	aarch64-linux-gnu-ld user.o -T linker.ld -o user.elf

../rootfs/user.img: user.elf
	aarch64-linux-gnu-objcopy -O binary user.elf user.img
	mv user.img ../rootfs/

clean:
	rm -f user.o user.elf user.img ../rootfs/user.img

