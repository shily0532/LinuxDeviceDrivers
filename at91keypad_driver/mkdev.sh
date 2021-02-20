#!/bin/sh

module="at91keypad"
device="keypad"

devid=`cat /proc/devices | grep $device | awk '{print $device}'`
echo $devid

if [ -n "$devid" ]; then
#NOT NULL
	rmmod $module
	echo "rmmod"
fi

insmod $module.ko
devid=`cat /proc/devices | grep $device | awk '{print $device}'`
echo $devid
major=$(awk "\$2==\"$device\" {print \$1}" /proc/devices)
echo "am i right?"
echo $major

if [ -e "/dev/$device" ]; then
# [-e /dev/$device] if /dev/$dev not exist then return false
	rm -rf /dev/$device
	echo "here"
else
	echo "there"
fi

mknod /dev/$device c $major 0
