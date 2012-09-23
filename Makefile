ifeq ($(KERNELRELEASE),)
	#outside the kernel make file
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

all: modules

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install

TESTS_SOURCES = tests/ff_tests.c tests/tests_main.c
ff.tests: $(TESTS_SOURCES)
	gcc $(TESTS_SOURCES) -o ff.tests -lcheck

check: ff.tests
	./ff.tests

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.ko.unsigned *.mod.c .tmp_versions Module.symvers modules.order ff.tests

load:
	ln -s `pwd`/ff.rules /etc/udev/rules.d/70-ff.rules
	insmod ff.ko

unload:
	rmmod ff.ko
	rm /etc/udev/rules.d/70-ff.rules

.PHONY: all modules moduels_install clean check load unload

else
	#inside the kernel make
	obj-m := ff.o

endif

