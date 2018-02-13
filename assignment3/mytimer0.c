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
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#include <asm/io.h>
#include <linux/delay.h>
#include "mytimer0.h"



#define CWR		0x43
#define COUNTER0	0x40 
#define COUNTER1	0x41
#define COUNTER2	0x42


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Poulami Pal");
MODULE_DESCRIPTION("writing a device driver");


#define MYMAJOR 90
#define MYMINOR 3



struct cdev * mycdev;
dev_t mychardevice;
struct class * myclass;


//-------------------------------------------------------------------------------------------------
static int device_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "device_open\n");
		
		outb_p(0x30, CWR);
		outb_p(0xAA, COUNTER0);
		outb_p(0x04, COUNTER0);

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
	int t;
	int ch[2];
	printk(KERN_INFO "device_read \n");
	outb_p(0x00, CWR);
	t = inb(COUNTER0);
	ch[0] = t;
		printk(KERN_INFO "do nothing %d %c\n",(t),ch[0]);
	t = inb(COUNTER0);
	ch[1] = t;
		printk(KERN_INFO "do nothing %d %c\n",(t),ch[1]);
     	copy_to_user(buffer, ch,8);
	return SUCCESS;
}
//-----------------------------------------------------------------------------------------------------
long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
{
	u8 t;
	int i=0,n=0;
	char *temp1;
	char channeltemp[15];
	int channel[15];
	int chan=0;
        temp1 = (char *)ioctl_param;
	copy_from_user(channeltemp, (const void *)ioctl_param,15);
	for(i=0;i<15;i++){
	channel[i] = (int)(channeltemp[i]);
	if(channel[i]<=0)
		channel[i]=0;
    	}

	for(i=0,n=14;i<15;i++,n--){
		chan=chan*10+(channel[n]);
    	}

	printk(KERN_INFO "do nothing %d\n",chan);
	
	switch (ioctl_num) {
    	case IOCTL_DELAY:
		label:
			outb_p(0x30, CWR);
			outb_p(0xAA, COUNTER0);
			outb_p(0x04, COUNTER0);
			t=0x00;
			while((t&0x80)!=0x80){
				outb_p(0xE2, CWR);
				t = inb(COUNTER0);
			}
			if(chan>0) {chan--;goto label;if((chan%100)==0)printk(KERN_INFO "do nothing %d\n",chan);}
			else break;

	default:
		printk(KERN_INFO "do nothing %c\n",'p');
		break;
    }

    return SUCCESS;
}
//-------------------------------------------------------------------------------------------------------
static struct file_operations Fops = {
	.owner = THIS_MODULE,
        .read = device_read,
        .open = device_open,
        .unlocked_ioctl = device_ioctl,
        .release = device_release,
};
//-------------------------------------------------------------------------------------------------------
int  mod_init(void){
	int cdevaddresult;
   	mychardevice=MKDEV(MYMAJOR,MYMINOR);

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
    	if (device_create(myclass, NULL, mychardevice, NULL,"timer0") == NULL)
  	{
   	 	class_destroy(myclass);
    		unregister_chrdev_region(mychardevice, 1);
    		return -1;
  	}
	
	cdev_init(mycdev, &Fops);
    	cdevaddresult = cdev_add(mycdev, mychardevice, 1);
    	if(cdevaddresult < 0) {
		printk(KERN_ERR "failed to add cdev\n");
    		return -1;
	}
	
        printk(KERN_ALERT "--%d--\n",cdevaddresult);

	printk(KERN_INFO "Created device with <MAJOR,MINOR>: <%d,%d>\n",MAJOR(mychardevice),MINOR(mychardevice));	
	
	printk(KERN_INFO "Counter set\n");

	return 0;
}
//-------------------------------------------------------------------------------------------------------
void mod_exit(void){

	printk(KERN_INFO "In exit module.\n");
	if(MAJOR(mychardevice)>0){
		cdev_del(mycdev);
		printk(KERN_INFO "Device unregistered!\n");
		unregister_chrdev_region(mychardevice,1);
		printk(KERN_INFO "Device unregistered!\n");
  		device_destroy(myclass, mychardevice);
		printk(KERN_INFO "Device unregistered!\n");
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
