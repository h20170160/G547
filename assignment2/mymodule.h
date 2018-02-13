#ifndef MYMODULE_H
#define MYMODULE_H

#include <linux/ioctl.h>


//#define MYMAJOR 30
//#define MYMINOR 0

#define IOCTL_SET_CHANNEL _IOW('a', 0, char *)
#define IOCTL_SET_ALIGN _IOW('a', 1, char *)

#define DEVICE_FILE_NAME "/dev/mydevice"

#define SUCCESS 0
#define DEVICE_NAME "mydevice"

int MYMAJOR;
int MYMINOR;

#endif
