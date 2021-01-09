/************************************************************

FileName: scull_test.c
Author: Version : Date: Joe Zhou
Description: // simple character utility loading localities

<author> <time> <version > <desc>
Joe 09/01/2021 1.0 build this module for linux
***********************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define DEV_TEST	"/dev/scull"


int main(int argc,char **argv)
{
	int fd = -1;
	int i, numbers = 0;
	int *wrBuffer, *rdBuffer;

	numbers = atoi(argv[1]);	// get the number of data to be written

	printf("Numbers to be tested is: %d\n", numbers);

	fd = open(DEV_TEST, O_RDWR);// open the device
	if (fd < 0) {
		perror("open " DEV_TEST " failed");
		exit(0);
	}

	wrBuffer = (int *)malloc(numbers*sizeof(int));	//allocate memory
	for( i=0; i<numbers; i++)
		*(wrBuffer+i)=i;

	write(fd, wrBuffer, numbers*sizeof(int));	// write to devices

	// Read back to verify
	lseek(fd, 0L, SEEK_SET);
	rdBuffer = (int *)malloc(numbers*sizeof(int));
	memset(rdBuffer, 0, numbers*sizeof(int));
	read(fd, rdBuffer, numbers*sizeof(int));

	for(i=0; i<numbers; i++)
	{
		printf("%d\t",*(rdBuffer+i));
		if((i+1)%10==0)
			printf("\n");
	}

	close(fd);

	return 0;
}
