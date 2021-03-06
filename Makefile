# sudo apt-get install g++ binutils libc6-dev-i386

GCCPARAMS = -m32 -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -fno-pie
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = obj/loader.o \
	obj/gdt.o \
	obj/drivers/keyboard.o \
	obj/drivers/mouse.o \
	obj/drivers/vga.o \
	obj/drivers/driver.o \
	obj/hardwarecommunication/port.o \
	obj/hardwarecommunication/pci.o \
	obj/hardwarecommunication/interrupts.o \
	obj/multitask.o \
	obj/memorymanagement.o \
	obj/interruptstubs.o \
	obj/gui/desktop.o \
	obj/gui/widget.o \
	obj/gui/window.o \
	obj/kernel.o 


obj/%.o: src/%.cpp
	mkdir -p $(@D)
	gcc $(GCCPARAMS) -c -o $@ $<

obj/%.o: src/%.s
	mkdir -p $(@D)
	as $(ASPARAMS) -o $@ $<

mykernel.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

install: mykernel.bin
	sudo cp $< /boot/mykernel.bin

mykernel.iso : mykernel.bin
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp $< iso/boot/
	echo 'set timeout=0' >> iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo '' >> iso/boot/grub/grub.cfg
	echo 'menuentry "My Operating System" {' >> iso/boot/grub/grub.cfg
	echo '	multiboot /boot/mykernel.bin' >> iso/boot/grub/grub.cfg
	echo '	boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=$@ iso
	rm -rf iso

run : mykernel.iso
	(pkill VirtualBox && sleep 1) || true
	virtualbox --startvm "my operating system" &

.PHONY: clean

clean :
	rm -rf obj  mykernel.bin mykernel.iso
