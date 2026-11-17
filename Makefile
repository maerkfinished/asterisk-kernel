CC   = i686-elf-gcc
LD   = i686-elf-ld
ASM  = nasm
QEMU = qemu-system-x86_64

CFLAGS  = -ffreestanding -m32 -O2 -Wall -Wextra
LDFLAGS = -T src/linker.ld

asm_compile     = src/boot/unlock.asm src/boot/boot.asm
asm_destination = src/boot/unlock.bin src/boot/boot.bin
compile_special = src/kernel/kernel.c src/kernel/misc_entry.asm
special_dest    = src/kernel/kernel.o src/kernel/misc_entry.o
special_final   = src/kernel/kernel.exec src/kernel/misc_entry.o

cfiles_compile  = src/kernel/libraries/string.c src/kernel/libraries/math.c src/kernel/libraries/hwreg.c src/drivers/vga.c src/drivers/ps2kbd.c src/init/init.c 
cfiles_dest     = src/kernel/libraries/string.o src/kernel/libraries/math.o src/kernel/libraries/hwreg.o src/drivers/vga.o src/drivers/ps2kbd.o src/init/init.o 

os_dest         = build/os.img


all: $(asm_destination) $(special_dest) $(cfiles_dest) $(special_final) run

%.bin: %.asm
	$(ASM) -f bin $< -o $@
%.o:   %.asm
	$(ASM) -f elf32 $< -o $@
%.o:   %.c
	$(CC) $(CFLAGS) -c $< -o $@
%.exec: %.o
	$(LD) $(LDFLAGS) -o $@ --oformat binary src/kernel/misc_entry.o $^ $(cfiles_dest)
%.bin: %.o
	$(LD) $(LDFLAGS) -o $@ --oformat binary $^
run:
	cat $(asm_destination) src/kernel/kernel.exec > $(os_dest)
	dd if=/dev/null of=$(os_dest) bs=1 count=0 seek=1474560
	$(QEMU) -drive file=$(os_dest),format=raw,if=floppy,index=0 -monitor stdio
clean:
	rm -rf src/boot/*.bin
	rm -rf src/kernel/kernel.exec
	rm -rf src/**/*.o
	rm -rf src/**/**/*.o
	rm -rf build/*.img