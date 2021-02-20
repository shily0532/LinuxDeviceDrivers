#
#	Description:
#		Makefile for 2.6 device drivers		
#
#	Copyright (C), 2012-2022, CETC 22nd.
#
#	Author: Joe

# Comment/uncomment the following line to disable/enable debugging
#DEBUG = y

# Add your debugging flag (or not) to CFLAGS
ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DKEYPAD_DEBUG 
# "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif

EXTRA_CFLAGS += $(DEBFLAGS)

ifneq ($(KERNELRELEASE),)
# call from kernel build system

obj-m	:= at91keypad.o

else

KERNELDIR ?= /home/joe/at91_donot_delete/linux-2.6.27-stable/
PWD       := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

endif

clean:
	rm -rf *.o *.ko *.mod.c
