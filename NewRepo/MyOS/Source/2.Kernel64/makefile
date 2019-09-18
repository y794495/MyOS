NASM64 = nasm -f win64
GCC64 = gcc -m64 -c -ffreestanding
LD64 = ld -T ../i386pep.x -nostdlib -e Main -Ttext 0x200000
OBJCOPY64 = objcopy -j .text -j .data -j .rdata -j .bss -S -O binary
make = make
ObjectDirectory = Temp
SourceDirectory = Source

all: prepare Kernel64.bin

prepare:
	@echo =================
	mkdir -p $(ObjectDirectory);

dep:
	@echo === Make Dependency File ==========
	$(make) -C $(ObjectDirectory) -f ../makefile InternalDependency
	@echo === Dependency Search Complete ====

ExecuteInternalBuild: dep
	$(make) -C $(ObjectDirectory) -f ../makefile Kernel64.exe

Kernel64.bin: ExecuteInternalBuild
	$(OBJCOPY64) $(ObjectDirectory)/Kernel64.exe $@

clean:
	rm -f *.binary
	rm -f $(ObjectDirectory)/*.*

EntryPointSourceFile = ../$(SourceDirectory)/EntryPoint.s
EntryPointObjectFile = EntryPoint.o

CSourceFiles = $(wildcard ../$(SourceDirectory)/*.c)
AssemblySourceFiles = $(wildcard ../$(SourceDirectory)/*.asm)
CObjectFiles = $(notdir $(patsubst %.c,%.o,$(CSourceFiles)))
AssemblyObjectFiles = $(notdir $(patsubst %.asm,%.o,$(AssemblySourceFiles)))

$(EntryPointObjectFile): $(EntryPointSourceFile)
	$(NASM64) -o $@ $<

%.o: ../$(SourceDirectory)/%.c
	$(GCC64) -c $<

%.o: ../$(SourceDirectory)/%.asm
	$(NASM64) -o $@ $<


InternalDependency:
	$(GCC64) -MM $(CSourceFiles)>Dependency.dep

Kernel64.exe: $(EntryPointObjectFile) $(CObjectFiles) $(AssemblyObjectFiles)
	@echo ==============entry============================
	$(LD64) -o $@ $^

ifeq (Dependency.dep, $(wildcard Dependency.dep))
include Dependency.dep
endif