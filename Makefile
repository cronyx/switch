MODULES := switch.o

ARCH := arm
CROSS_COMPILE := arm-openipc-linux-musleabi-

obj-m := $(MODULES)

KDIR := /home/cronyx/openipc/firmware/output/build/linux-4.9.37

all: modules

modules:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(shell pwd) modules

clean:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(shell pwd) clean
