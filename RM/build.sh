nasm -fbin RM/boot.asm -o os.bin


qemu-system-i386 -hda os.bin

rm *.bin 