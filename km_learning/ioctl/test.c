#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "headers.h"

int main(){
	int answer;
	struct greeter_struct gs = {4, "string"};

	int dev = open("/dev/device_nr", O_RDONLY);
	if (dev == -1){
		printf("Not successful\n");
		return -1;
	}

	ioctl(dev, RD_VALUE, &answer);
	printf("Answer value: %d\n", answer);

	answer=123;

	ioctl(dev, WR_VALUE, &answer);
	ioctl(dev, RD_VALUE, &answer);
	printf("Answer value: %d\n", answer);

	ioctl(dev, GREETER, &gs);

	printf("Success!\n");
	close(dev);
	return 0;
}

