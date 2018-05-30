#makefile for Boot

#program,flags,etc

ASM		=nasm
DASM	=ndisasm
CC		=gcc
LD		=ld

ASMFLAGS		= -I "boot/"
ASMFLAGS_ELF	= -f elf
LDFLAGS			= -s -Ttext 0x30400	-m elf_i386	
CCFLAGS			= -m32	-c  -fno-builtin -fno-stack-protector -I "include/" -I "include/sys/" -O0

#This Program
BOOT_TARGET		=	boot/boot.bin boot/loader.bin
KERNEL_TARGET	=	kernel.bin
OBJ_TARGET		=	kernel.o start.o mystring.o protect.o kliba.o interrupt.o mystring_c.o kliba_c.o main.o  test.o \
global.o clock.o system_call.o system_call_asm.o process.o keyboard.o tty.o printf.o ipc.o hd.o fs/main.o fs/misc.o \
fs/open.o fs/read_write.o fs/link.o fs/disklog.o mm/main.o mm/fork.o
#default starting positong
everything:	$(BOOT_TARGET) $(KERNEL_TARGET)

build:
	dd if=boot/boot.bin of=a.img count=1 bs=512 conv=notrunc
	sudo mount -o loop -t msdos  a.img /mnt/floppy
	sudo cp -fv kernel.bin /mnt/floppy
	sudo cp -fv boot/loader.bin /mnt/floppy
	sudo umount /mnt/floppy
	bochs
clean:
	sudo rm -rf $(BOOT_TARGET) $(KERNEL_TARGET) $(OBJ_TARGET)

all: clean everything build

boot/boot.bin:	boot/boot.asm boot/include/stdvar.inc boot/include/fat12_head_info.inc \
boot/lib/ReadSector.asm boot/lib/GetNextClusByFat.asm
	$(ASM)	$(ASMFLAGS)	-o $@	$<
	
boot/loader.bin: boot/loader.asm boot/include/stdvar.inc boot/include/fat12_head_info.inc  \
boot/include/pm.inc boot/include/loadKernel.asm boot/lib/ReadSector.asm boot/lib/put_string.asm \
boot/lib/GetNextClusByFat.asm boot/lib/KillMotor.asm boot/lib/calMemSize.asm boot/lib/getMemARDS.asm
	$(ASM)	$(ASMFLAGS)	-o $@	$<
kernel.bin:$(OBJ_TARGET)
	$(LD)	$(LDFLAGS)	-o $@	$^
kernel.o:kernel/kernel.asm
	$(ASM)	$(ASMFLAGS_ELF)	-o $@	$<
start.o: kernel/start.c include/sys/const.h include/sys/protect.h include/sys/const.h \
 include/string.h include/sys/interrupt.h include/sys/global.h \
 include/sys/protect.h include/sys/process.h
	$(CC)	$(CCFLAGS)	-o $@	$<
mystring.o:lib/mystring.asm
	$(ASM)	$(ASMFLAGS_ELF)	-o $@	$<
protect.o: lib/protect.c include/sys/const.h include/sys/protect.h \
 include/sys/const.h include/string.h include/sys/global.h include/sys/protect.h \
 include/sys/process.h
	$(CC)	$(CCFLAGS)	-o $@	$<
kliba.o:lib/kliba.asm
	$(ASM)	$(ASMFLAGS_ELF)	-o $@	$<
interrupt.o: lib/interrupt.c include/sys/const.h include/sys/interrupt.h \
 include/sys/kliba.h include/string.h include/sys/global.h include/sys/protect.h \
 include/sys/const.h include/sys/process.h
	$(CC)	$(CCFLAGS)	-o $@	$<
mystring_c.o:lib/mystring_c.c include/sys/const.h
	$(CC)	$(CCFLAGS)	-o $@	$<
kliba_c.o:lib/kliba.c include/sys/system_call.h include/sys/clock.h \
 include/sys/const.h
	$(CC)	$(CCFLAGS)	-o $@	$<
main.o: kernel/main.c include/sys/kliba.h include/string.h include/sys/const.h \
 include/sys/global.h include/sys/protect.h include/sys/const.h include/sys/process.h \
 include/sys/interrupt.h include/sys/system_call.h
	$(CC)	$(CCFLAGS)	-o $@	$<
global.o: lib/global.c include/sys/const.h include/sys/protect.h include/sys/const.h \
 include/sys/process.h include/sys/protect.h
	$(CC)	$(CCFLAGS)	-o $@	$<
clock.o: lib/clock.c include/sys/kliba.h include/sys/global.h \
 include/sys/protect.h include/sys/const.h include/sys/process.h include/sys/clock.h
	$(CC)	$(CCFLAGS)	-o $@	$<
system_call.o: lib/system_call.c include/sys/const.h include/sys/global.h \
 include/sys/protect.h include/sys/const.h include/sys/process.h
	$(CC)	$(CCFLAGS)	-o $@	$<
system_call_asm.o:lib/system_call.asm
	$(ASM)	$(ASMFLAGS_ELF)	-o $@	$<
process.o:lib/process.c include/sys/process.h include/sys/protect.h \
 include/sys/const.h include/sys/global.h
	$(CC)	$(CCFLAGS)	-o $@	$<
keyboard.o: lib/keyboard.c include/sys/global.h include/sys/protect.h \
 include/sys/const.h include/sys/process.h
	$(CC)	$(CCFLAGS)	-o $@	$<
tty.o: kernel/tty.c include/sys/global.h include/sys/keyboard.h \
 include/sys/const.h
	$(CC)	$(CCFLAGS)	-o $@	$<
printf.o: lib/printf.c include/sys/global.h include/sys/const.h
	$(CC)	$(CCFLAGS)	-o $@	$<
ipc.o: kernel/ipc.c include/sys/const.h include/sys/protect.h include/sys/const.h \
 include/sys/tty.h include/string.h include/sys/tty.h include/sys/process.h \
 include/sys/protect.h include/sys/global.h include/sys/process.h include/sys/ipc.h
	$(CC)	$(CCFLAGS)	-o $@	$<
hd.o: kernel/hd.c include/sys/const.h include/sys/protect.h include/sys/const.h \
 include/string.h include/sys/tty.h include/sys/process.h include/sys/protect.h \
 include/sys/tty.h include/sys/global.h include/sys/process.h include/sys/interrupt.h \
 include/sys/clock.h include/sys/hd.h
	$(CC)	$(CCFLAGS)	-o $@	$<
fs/main.o: fs/main.c include/sys/process.h include/sys/protect.h include/sys/const.h \
 include/sys/tty.h include/sys/kliba.h include/sys/ipc.h include/sys/process.h \
 include/sys/const.h
	$(CC)	$(CCFLAGS)	-o $@	$<
fs/misc.o: fs/misc.c include/sys/process.h include/sys/protect.h include/sys/const.h \
 include/sys/tty.h include/sys/kliba.h include/sys/ipc.h include/sys/process.h \
 include/sys/const.h
	$(CC)	$(CCFLAGS)	-o $@	$<
fs/open.o: fs/open.c include/sys/process.h include/sys/protect.h include/sys/const.h \
 include/sys/tty.h include/sys/kliba.h include/sys/ipc.h include/sys/process.h \
 include/sys/const.h
	$(CC)	$(CCFLAGS)	-o $@	$<
fs/read_write.o: fs/read_write.c include/sys/process.h include/sys/protect.h include/sys/const.h \
 include/sys/tty.h include/sys/kliba.h include/sys/ipc.h include/sys/process.h \
 include/sys/const.h
	$(CC)	$(CCFLAGS)	-o $@	$<
fs/link.o: fs/link.c include/sys/process.h include/sys/protect.h include/sys/const.h \
 include/sys/tty.h include/sys/kliba.h include/sys/ipc.h include/sys/process.h \
 include/sys/const.h
	$(CC)	$(CCFLAGS)	-o $@	$<
fs/disklog.o: fs/disklog.c include/sys/process.h include/sys/protect.h include/sys/const.h \
 include/sys/tty.h include/sys/kliba.h include/sys/ipc.h include/sys/process.h \
 include/sys/const.h
	$(CC)	$(CCFLAGS)	-o $@	$<
mm/main.o: mm/main.c include/sys/process.h include/sys/protect.h include/sys/const.h \
 include/sys/tty.h include/sys/kliba.h include/sys/ipc.h include/sys/process.h \
 include/sys/const.h
	$(CC)	$(CCFLAGS)	-o $@	$<
mm/fork.o: fs/fork.c include/sys/process.h include/sys/protect.h include/sys/const.h \
 include/sys/tty.h include/sys/kliba.h include/sys/ipc.h include/sys/process.h \
 include/sys/const.h
	$(CC)	$(CCFLAGS)	-o $@	$<
test.o:lib/test.c 
	$(CC)	$(CCFLAGS)	-o $@	$<