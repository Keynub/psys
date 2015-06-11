QEMU = /usr/libexec/qemu-kvm
QEMUOPTS = -m 256 -kernel kernel/kernel.bin

VINAGRE = vinagre
VINAGREOPTS = localhost:5901

#QEMUOPTS = -no-kvm -net nic -net user,tftp="`pwd`",bootfile=$(PXE) -boot n -cpu pentium -rtc base=localtime -m 64M -S -s

.PHONY: clean all

all:
	$(MAKE) -C user/ all VERBOSE=$(VERBOSE)
	$(MAKE) -C kernel/ kernel.bin VERBOSE=$(VERBOSE)

clean:
	$(MAKE) clean -C kernel/
	$(MAKE) clean -C user/

run:
	$(QEMU) $(QEMUOPTS)

vinagre:
	$(VINAGRE) $(VINAGREOPTS)

