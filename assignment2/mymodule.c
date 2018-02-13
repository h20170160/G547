#include <linux/module.h> 
#include <linux/kernel.h>
#include <linux/init.h> // for printk()
#include <linux/fs.h>
#include <linux/types.h>
#include <asm/uaccess.h>        /* for get_user and put_user */
#include <linux/time.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/list.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/random.h>

#include "mymodule.h"


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Poulami Pal");
MODULE_DESCRIPTION("writing a device driver");


#define MAJOR_NO 90
#define MINOR_NO 3
//int MYMAJOR;
//int MYMINOR;

char channel[] = {'1'};
struct cdev * mycdev;
dev_t mychardevice;
struct class * myclass;

//-------------------------------------------------------------------------------------------------
static int device_open(struct inode *inode, struct file *file)
{
printk(KERN_INFO "device_open\n");
    //try_module_get(THIS_MODULE);
    return SUCCESS;
}
//-------------------------------------------------------------------------------------------------
static int device_release(struct inode *inode, struct file *file)
{
printk(KERN_INFO "device_release\n");
    //module_put(THIS_MODULE);
    return SUCCESS;
}
//-------------------------------------------------------------------------------------------------
static ssize_t device_read(struct file *file, char __user * buffer, size_t length, loff_t * offset)
{
	int d;
	int c=1;
	int i=0;
	char y='0';
	char x[]="0000";
	printk(KERN_INFO "device_read\n");
	
	d=get_random_int()%1024;
/*	goto label;
d0:	x[0] =y;
	if(d>10){
	i++;
	goto label;}
	else goto here;
d1:	x[1] =y;
	if(d>10){
	i++;
	goto label;}
	else goto here;
d2:	x[2] =y;
	if(d>10){
	i++;
	goto label;}
	else goto here;
d3:	x[3] =y;
	if(d>10){
	i++;
	}
	
here:*/	if(d<0)
		d=0;
	printk(KERN_INFO "device_read %d\n", d);
     	put_user(d,buffer++);
	return SUCCESS;
/*label:	c=d%10;
	if(c==0)
		y='0';
	if(c==1)
		y='1';
	if(c==2)
		y='2';
	if(c==3)
		y='3';
	if(c==4)
		y='4';
	if(c==5)
		y='5';
	if(c==6)
		y='6';
	if(c==7)
		y='7';
	if(c==8)
		y='8';
	if(c==9)
		y='9';
d=d/10;
	if(i==0)	
		goto d0;
	if(i==1)	
		goto d1;
	if(i==2)	
		goto d2;
	if(i==3)	
		goto d3;
	return SUCCESS;*/
}
//-------------------------------------------------------------------------------------------------
long device_ioctl(struct file *file,	/* ditto */
                  unsigned int ioctl_num,        	/* number and param for ioctl */
                  unsigned long ioctl_param)
{
	char *temp1;
	int d;
	printk(KERN_INFO "device_ioctl\n");

    switch (ioctl_num) {
    case IOCTL_SET_CHANNEL:
        temp1 = (char *)ioctl_param;
	get_random_bytes(&d, sizeof(char));
	channel[0]=(char)d;
	get_user(channel[0], temp1);
	printk(KERN_INFO "channel number selected: %c\n",temp1[0]);
	break;

    case IOCTL_SET_ALIGN:
	printk(KERN_INFO "allignment: do nothing %c\n",'p');
	break;
    }

    return 5;
}
//-------------------------------------------------------------------------------------------------------
static struct file_operations Fops = {
	.owner = THIS_MODULE,
        .read = device_read,
        .unlocked_ioctl = device_ioctl,
        .open = device_open,
        .release = device_release      /* a.k.a. close */
};
//-------------------------------------------------------------------------------------------------------
int  mod_init(void){
int y,result;
   	mychardevice=MKDEV(MAJOR_NO,MINOR_NO);

	printk(KERN_INFO "Created device with <MAJOR,MINOR>: <%d,%d>\n",MAJOR(mychardevice),MINOR(mychardevice));	
	

	

	mycdev = cdev_alloc();

    	if(!mycdev) {
		printk(KERN_ERR "failed to alloc cdev\n");
    		return -1;
    	}
    	
	if ((myclass = class_create(THIS_MODULE, "myclass")) == NULL)
  	{
    		unregister_chrdev_region(mychardevice, 1);
    		return -1;
  	}
    	if (device_create(myclass, NULL, mychardevice, NULL,"mydevice") == NULL)
  	{
   	 	class_destroy(myclass);
    		unregister_chrdev_region(mychardevice, 1);
    		return -1;
  	}
	
	cdev_init(mycdev, &Fops);
    	result = cdev_add(mycdev, mychardevice, 1);
    	if(result < 0) {
		printk(KERN_ERR "failed to add cdev\n");
	}
	
	y=cdev_add(mycdev,mychardevice,1);
        printk(KERN_ALERT "--%d--\n",y);
	return 0;
}
//-------------------------------------------------------------------------------------------------------
void mod_exit(void){
	printk(KERN_INFO "In exit module.\n");
	if(MAJOR(mychardevice)>0){
		cdev_del(mycdev);
		unregister_chrdev_region(mychardevice,1);
  		device_destroy(myclass, mychardevice);
  		class_destroy(myclass);
		printk(KERN_INFO "Device unregistered!\n");
	}
	else{
		printk(KERN_INFO "No device to unregister!\n");	
	}
}
//-------------------------------------------------------------------------------------------------------
module_init(mod_init);
module_exit(mod_exit);
//-------------------------------------------------------------------------------------------------------
