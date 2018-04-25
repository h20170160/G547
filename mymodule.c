#include <linux/module.h> 
#include <linux/kernel.h>
#include <linux/init.h> // for printk()
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/list.h>
#include <linux/proc_fs.h>
#include <linux/param.h>

#include "mymodule.h"


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Poulami Pal");
MODULE_DESCRIPTION("writing a device driver");


#define MAJOR_NO 90
#define MINOR_NO 3


char channel[] = {'1'};
struct cdev * mycdev;
dev_t mychardevice;
struct class * myclass;
struct timer_list mytimer;
void wfunc(unsigned long i){printk("doing nothing %lu\n",i);}
//-------------------------------------------------------------------------------------------------
static int device_open(struct inode *inode, struct file *file)
{
	int i =0;
	init_timer(&mytimer);
	mytimer.expires= jiffies+(HZ/2);
	mytimer.data=0;
	mytimer.function=wfunc;//(jiffies);
	printk(KERN_INFO "device_open\n");
	add_timer(&mytimer);
	while(i<99999){
		i++;
		printk(KERN_INFO "device_open %d\n",i);
	}
	try_module_get(THIS_MODULE);
    	return SUCCESS;
}
//-------------------------------------------------------------------------------------------------
static int device_release(struct inode *inode, struct file *file)
{
printk(KERN_INFO "device_release\n");
    	module_put(THIS_MODULE);
    	return SUCCESS;
}
//-------------------------------------------------------------------------------------------------
static ssize_t device_read(struct file *file, char __user * buffer, size_t length, loff_t * offset)
{
	//unsigned long k = jiffies + 2*HZ;
	//while(time_before(jiffies,k));
	printk(KERN_INFO "device_read %lu\n",jiffies);
    	return SUCCESS;
}
//-------------------------------------------------------------------------------------------------------
static struct file_operations Fops = {
	.owner = THIS_MODULE,
        .read = device_read,
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
