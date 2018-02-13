#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>              /* open */
#include <unistd.h>             /* exit */
#include <sys/ioctl.h>          /* ioctl */
#include "mytimer0.h"


/*ioctl function*/
int ioctl_delay(int file_desc, char *message)
{
	int ret_val;
    ret_val = ioctl(file_desc, IOCTL_DELAY, message);

        //printf("ioctl_delay||%d||%d||\n", ret_val,*message);
    if (ret_val < 0) {
        exit(-1);
    }
    return 0;
}
/*Converting Delay. Calls ioctl functionS*/
void delay (int i,int file_desc){

	char buf[15];
	char a;
	int d,t=0;

	while(t<15){
	buf[t] = 0;
	t++;
	}
	t=0;	

	do{
	d=i%10;
	i=i/10;
	a=(char)d;
	buf[t] = a;
	t++;
	}while(i>0);

	ioctl_delay(file_desc,buf);
}

int main()
{int i,j;
    	int file_desc, ret_val=0;
	char a;
	int d,t=0,readbuf[2];

	ret_val = getpid();
       	printf("Process ID is: %d\n", ret_val);
/*Openning file*/
    	file_desc = open(DEVICE_FILE_NAME, 0);
    	if (file_desc < 0) {
        	printf("Can't open device file.\n");
        	exit(-1);
    	}
	else{  	
		printf("Openning File: %s\n", DEVICE_FILE_NAME);
    	}
/*Employing delay of 1.5 second*/
	printf("Start delay\n");
	delay(1500,file_desc);
	printf("Stop delay\n");
/*reading file 100 times*/
for(int k =0;k<100;k++){
	ret_val = read(file_desc,readbuf, 1);
	if (ret_val < 0) {
        	printf("Can't read device file: %s\n", DEVICE_FILE_NAME);
        	exit(-1);
    	}
	else{  	
		printf("Value Read MSB:%d LSB:%d Value:%d\n", readbuf[1], readbuf[0], ((readbuf[1]*256)+ readbuf[0]));
    	}
}
/*Closinf file*/       	
	close(file_desc);
    	return 0;
}
