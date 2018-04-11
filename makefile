#makefile for Boot

#program,flags,etc

ASM		=nasm
DASM	=ndisasm
CC		=gcc
LD		=ld

ASMFLAGS 		= -I	"boot/include/ boot/lib/"

#This Program
BOOT_TARGET	=	boot/boot.bin boot/loader.bin

#default starting positong
everything:	$(BOOT_TARGET)

build:
	dd if=boot/boot.bin of=a.img count=1 bs=512 conv=notrunc
	sudo mount -o loop a.img /mnt/floppy
	sudo cp -fv boot/loader.bin /mnt/floppy
	sudo umount /mnt/floppy
	bochsdbg
clean:
	rf -f $(BOOT_TARGET)

all: clean everything build

boot/boot.bin:	boot/boot.asm boot/include/stdvar.inc boot/include/fat12_head_info.inc boot/lib/ReadSector.asm boot/lib/put_string.asm boot/lib/GetNextClusByFat.asm
	$(ASM)	$(ASMFLAGS) -o $@	$<
	
boot/loader.bin: boot/loader.asm
	$(ASM)	$(ASMFLAGS) -o $@	$<
	
	#dd if=mbr.bin of=disk.img count=1 bs=512
	#dd if=core.bin of=disk.img bs=512 seek=1
#	dd if=user.bin of=disk.img bs=512 seek=50
#	dd if=diskdata.txt of=disk.img bs=512 seek=100
