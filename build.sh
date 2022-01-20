#!/bin/sh
nasm -felf PenisOS/Kernel.asm -o K.o
gcc -m32 -fno-stack-protector -Wl,static-libgcc,lgcc -ffreestanding -fpermissive -c PenisOS/Kernel.cpp -w -o KC.o -I ./PenisOS 
ld -m elf_i386 -T link.ld -o kernel.bin KC.o K.o 
qemu-system-i386 -kernel ./kernel.bin -drive if=none,media=disk,index=0
rm -f *.bin
rm -f *.o

