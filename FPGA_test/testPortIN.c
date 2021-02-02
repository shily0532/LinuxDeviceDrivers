/*
 * FileName: scull_test.c
 *
 * Description:  FPGA Read/Write interface
 * Author: Version : Date: Joe Zhou
 *
 * <author> <time> <version > <desc>
 * Joe 01/02/2021 1.0 build this module for linux
 */

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "port.h"

/**
 * OpenPortDev() - open fpga device
 * @dev: name of device
 *
 * This function should be used by drivers openning
 * devices. Return handle fd(>=0) when success, or -1 failure.
 */
int OpenPortDev(char *dev)
{
	int fd = -1;

	if ((fd = open(dev, O_RDWR)) < 0) {
		printf("open %s error\n\r", dev);
		return -1;
	}
	return fd;
}

/**
 * ClosePortDev() - close fpga device
 * @fd: handle
 *
 * This function should be used by drivers closing
 * devices. 
 */
int ClosePortDev(int fd)
{
	close(fd);
	return 0;

}

/**
 * InPortw() - 8-bit Read from device
 * @fd: handle
 * @addr: specified address
 *
 * This function should be used by drivers reading
 * devices. Return register 16-bit value, or -1 failure.
 */
inline short InPortw(int fd, int addr)
{
	port_data_t pdata;

    pdata.regAddr = addr;
	if(ioctl(fd,IOCTL_PORT_READ,&pdata))
		return -1;
	printf("(InPortW)addr=%x,gData=%x\n",pdata.regAddr,pdata.regData);
    return (pdata.regData&0xFF);
}

/**
 * OutPortw() - 8-bit Write to device
 * @fd: handle
 * @addr: specified address
 * @data: data to be written
 *
 * This function should be used by drivers writing
 * devices. Return 0 if success, or -1 failure.
 */
inline int OutPortw(int fd, int addr, int data)
{
	port_data_t pdata;

    pdata.regAddr = addr ;
    pdata.regData = data;

	if(ioctl(fd,IOCTL_PORT_WRITE,&pdata))
		return -1;
	
    return 0;
}

#define PORTDEV		"/dev/port"

int main()
{
	int fd = -1;
	int outvalue=0xFF;

	fd = OpenPortDev(PORTDEV);
	if (fd < 0) 
	{
		perror("open " PORTDEV " error");
		exit(0);
	}
	while(1)
	{
		InPortw(fd,0x28);
		sleep(2);

		OutPortw(fd,0x2b,0x52);
		InPortw(fd,0x2b);

		sleep(2);
	}
	ClosePortDev(fd);

	return 0;
}
