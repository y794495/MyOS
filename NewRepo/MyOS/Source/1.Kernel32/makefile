NASM32 = nasm
GCC32 = gcc -c -m32 -ffreestanding -g
LD32 = ld -mi386pe -T ../i386pe.x -nostdlib -e Main -Ttext 0x10200
OBJCOPY32 = objcopy -j .text -j .data -j .rdata -j .bss -S -O binary
make = make
ObjectDirectory = Temp
SourceDirectory = Source


all: prepare Kernel32.bin

prepare:
	mkdir -p $(ObjectDirectory)

$(ObjectDirectory)/EntryPoint.bin: $(SourceDirectory)/EntryPoint.s
	$(NASM32) -o $@ $<

dep:
	@echo ========== Make Dependency File =============
	$(make) -C $(ObjectDirectory) -f ../makefile InternalDependency
	@echo ========== Dependency Search Compile=========

ExecuteInternalBuild: dep
	@echo ===========ExecuteInternalBuild==============
	$(make) -C $(ObjectDirectory) -f ../makefile Kernel32.exe

$(ObjectDirectory)/Kernel32.exe.bin: ExecuteInternalBuild
	@echo =============Kernel32.exe.bin================
	$(OBJCOPY32) $(ObjectDirectory)/Kernel32.exe $@

Kernel32.bin: $(ObjectDirectory)/EntryPoint.bin $(ObjectDirectory)/Kernel32.exe.bin
	@echo ==============Kernel32=======================
	cat $^ > $@


clean:
	rm -f *.bin
	rm -f $(ObjectDirectory)/*.*

CEntryPointObjectFile = Main.o
CSourceFiles = $(wildcard ../$(SourceDirectory)/*.c)
AssembleySourceFiles = $(wildcard ../$(SourceDirectory)/*.asm)
CObjectFiles = $(subst Main.o, , $(notdir $(patsubst %.c,%.o,$(CSourceFiles))))
AssembleyObjectFiles = $(notdir $(patsubst %.asm,%.o,$(AssembleySourceFiles)))
#subst는 substitution의 약자. notdir은 경로만 제외한 문자열
#이 함수가 하는일은 text라는 문자열 중에서 from에 해당하는 문자열을 to에 해당하는 문자열로 대체.
%.o: ../$(SourceDirectory)/%.c
	$(GCC32) -c $<

%.o: ../$(SourceDirectory)/%.asm
	@echo ============Assemble=====================
	$(NASM32) -f win32 --prefix _ -o $@ $<
	
InternalDependency:
	@echo ===============InternalDependency===================
	$(GCC32) -MM $(CSourceFiles) > Dependency.dep

Kernel32.exe: $(CEntryPointObjectFile) $(CObjectFiles) $(AssembleyObjectFiles)
	@echo ===============Kernel32.exe==========================
	$(LD32) -o $@ $^

ifeq (Dependency.dep, $(wildcard Dependency.dep))
include Dependency.dep
endif