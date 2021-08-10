#!/bin/sh
nasm -felf PenisOS/Kernel.asm -o K.o
gcc -m32 -fno-stack-protector -fpermissive -c PenisOS/Kernel.cpp -o KC.o
ld -m elf_i386 -T link.ld -o kernel.bin K.o KC.o 

qemu-system-i386 -kernel ./kernel.bin

rm -f *.bin
rm -f *.o