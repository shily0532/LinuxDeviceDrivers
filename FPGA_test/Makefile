# Comment/uncomment the following line to disable/enable debugging
#DEBUG = y

# Add your debugging flag (or not) to CFLAGS
ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g
# "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif

CROSS_COMPILE	?= /home/joe/at91_donot_delete/arm-2007q3/bin/arm-none-linux-gnueabi-
CC=$(CROSS_COMPILE)gcc
CFLAGS= -Wall

CFLAGS += $(DEBFLAGS)

all: testPortIN

#compiler ruler

testPortIN: testPortIN.c
	$(CC) $(CFLAGS) -o $@ $<

clean:	
	rm -f testPortIN
