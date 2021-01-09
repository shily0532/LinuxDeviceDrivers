#DEBUG = y

# Add your debugging flag (or not) to CFLAGS
#ifeq ($(DEBUG),y)
#  DEBFLAGS = -O -g
# "-O" is needed to expand inlines
#else
#  DEBFLAGS = -O2
#endif

#CROSS_COMPILE	?= /usr/local/arm/arm-2007q3/bin/arm-none-linux-gnueabi-
#CC=$(CROSS_COMPILE)gcc
CC=gcc
CFLAGS= -Wall

#CFLAGS += $(DEBFLAGS)

all: scull_test

#compiler ruler
scull_test: scull_test.c
	$(CC) $(CFLAGS) -o $@ $<

clean:	
	rm -f scull_test
