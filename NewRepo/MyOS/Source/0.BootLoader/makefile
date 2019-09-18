all: BootLoader

BootLoader : Source/BootLoader.asm
	nasm -f bin -o BootLoader.bin Source/BootLoader.asm

clean:
	rm -f Temp/BootLoader.bin