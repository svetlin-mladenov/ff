ifeq ($(KERNELRELEASE),)
	#outside the kernel make file
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

all: modules

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.ko.unsigned *.mod.c .tmp_versions Module.symvers modules.order

.PHONY: all modules moduels_install clean

else
	#inside the kernel make
	obj-m := ff.o

endif

