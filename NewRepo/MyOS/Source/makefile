make = make
all: BootLoader Kernel32 Kernel64 MyOS.img

BootLoader:
	@echo
	@echo ================ Build BootLoader ==================
	@echo

	$(make) -C 0.BootLoader

	@echo
	@echo ================= Build Complete ===================
	@echo

Kernel32:
	@echo
	@echo =============== Build 32bit Kernel =================
	@echo

	$(make) -C 1.Kernel32

	@echo
	@echo ================ Build Complete ====================
	@echo


Kernel64:
	@echo
	@echo =============== Build 64bit Kernel =================
	@echo

	$(make) -C 2.Kernel64
	
	@echo
	@echo =================Build Complete =====================
	@echo

MyOS.img: 0.BootLoader/BootLoader.bin 1.Kernel32/Kernel32.bin 2.Kernel64/Kernel64.bin
	@echo
	@echo ============== Disk Image Build Start ==============
	@echo

	./Utility/ImageMaker.exe $^
	@echo
	@echo ================ All Build Complete ================
	@echo

clean:
	make -C 0.BootLoader clean
	make -C 1.Kernel32 clean
	make -C 2.Kernel64 clean
	rm -f MyOS.img