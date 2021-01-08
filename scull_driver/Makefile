## Comment/uncomment the following line to disable/enable debugging
#DEBUG = y

## Add your debugging flag (or not) to CFLAGS
#ifeq ($(DEBUG),y)
#  DEBFLAGS = -O -g -DSCULL_DEBUG  # go back to *.c file
## "-O" is needed to expand inlines
#else
#  DEBFLAGS = -O2
#endif

#EXTRA_CFLAGS += $(DEBFLAGS)

ifneq ($(KERNELRELEASE),)
# call from kernel build system

obj-m	:= scull_driver.o

else

# MARK: root for desktop kernel
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

endif

clean:
	rm -rf *.o *.ko *.mod.c	
