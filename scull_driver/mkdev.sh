#!/bin/bash

module="scull_driver"
device="scull"

devid=`cat /proc/devices | grep $device | awk '{print $device}'`
echo $devid

if [ -n "$devid" ]; then
#NOT NULL
	sudo rmmod $module
	echo "rmmod"
fi

sudo insmod $module.ko
devid=`cat /proc/devices | grep $device | awk '{print $device}'`
echo $devid
major=$(awk "\\$2= =\"$module\" {print \\$1}" /proc/devices)
echo $major

if [ -e "/dev/$device" ]; then
# [-e /dev/$device] if /dev/$dev not exist then return false
	sudo rm -rf /dev/$device
	echo "here"
else
	echo "there"
fi

sudo mknod /dev/$device c $devid 0
