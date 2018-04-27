#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/init.h>// macros used to markup functions e.g. __init, __exit
#include <linux/module.h>// Core header for loading LKMs into the kernel
#include <linux/kernel.h>// Contains types, macros, functions for the kernel
#include <linux/device.h>// Header to support the kernel Driver Model
#include <linux/uaccess.h>
#include <linux/usb.h> //for usb stuffs
#include <linux/slab.h>
#include <linux/leds.h> //for led
#include <linux/delay.h>



static struct usb_device *device;
static struct usb_class_driver class;

struct led_classdev led;
     uint8_t brightness;

////////////////////////////////////////////////////

static void control_led(struct led_classdev *led,
                enum led_brightness brightness)
{
	int err,err1;
   if(brightness==0){
		printk(KERN_INFO "LED_OFF\n");
		err = usb_control_msg(device,
        usb_sndctrlpipe(device, 0),
        0x00,
        USB_TYPE_VENDOR | USB_DIR_OUT,
        0, 0,
        NULL, 0,
        1000);
    	printk(KERN_INFO "\nwriting000 %d\n", err);
		if (err < 0)
    	    dev_err(&device->dev,"%s: error sending control request: %d\n", __func__, err); 
		mdelay(100);
		
		err1 = usb_control_msg(device,
        usb_sndctrlpipe(device, 0),
        0x02,
        USB_TYPE_VENDOR | USB_DIR_OUT,
        0, 0,
        NULL, 0,
        1000);
    	printk(KERN_INFO "\nwriting000 %d\n", err);
		if (err1 < 0)
    	    dev_err(&device->dev,"%s: error sending control request: %d\n", __func__, err1); 
		printk(KERN_INFO "\n0002\n");  
	}

	else if(brightness==1){
		printk(KERN_INFO "LED ON\n");
		err = usb_control_msg(device,
        usb_sndctrlpipe(device, 0),
        0x01,
        USB_TYPE_VENDOR | USB_DIR_OUT,
        0, 0,
        NULL, 0,
        1000);
    	printk(KERN_INFO "\nwriting0004 %d\n", err);
		if (err < 0)
    	    dev_err(&device->dev,
                        "%s: error sending control request: %d\n",
                        __func__, err);

		mdelay(100);	
		err1 = usb_control_msg(device,
        usb_sndctrlpipe(device, 0),
        0x03,
        USB_TYPE_VENDOR | USB_DIR_OUT,
        0, 0,
        NULL, 0,
        1000);
    	printk(KERN_INFO "\nwriting0004 %d\n", err);
		if (err1 < 0)
    	    dev_err(&device->dev,
                        "%s: error sending control request: %d\n",
                        __func__, err);
		printk(KERN_INFO "\n0005\n");  
	}
}

//////////////////////////////////////////////////////////

static int led_open(struct inode *i, struct file *f)
{
	printk(KERN_INFO "UsbtoLed device loaded\n");
	return 0;
}

//////////////////////////////////////////////////////////////

static int led_close(struct inode *i, struct file *f)
{
	printk(KERN_INFO "UsbtoLed device closed\n");
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

static struct file_operations fops =
{
	.owner = THIS_MODULE,
	.open = led_open,
	.release = led_close,
};
//////////////////////////////////////////////////////////////////////////


static void led_disconnect(struct usb_interface *interface)
{
	
	usb_deregister_dev(interface, &class);
	printk(KERN_INFO "led drive removed\n");
}



static struct usb_device_id led_table[] =
{
	{ USB_DEVICE(0x16c0, 0x05dc) },
	{} /* Terminating entry */
};


static int led_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	int retval;
	int err;
	device = interface_to_usbdev(interface);

	class.name = "usb/led%d";
	class.fops = &fops;
	if ((retval = usb_register_dev(interface, &class)) < 0)
	{
		/* Something prevented us from registering this driver */
		printk(KERN_ERR "\nNot able to get a minor for this device.\n");
	}
	else
	{
		printk(KERN_INFO "\nMinor obtained: %d\n", interface->minor);
	}
	printk(KERN_ALERT "\n4|%d|\n",err);
	return retval;

}

MODULE_DEVICE_TABLE (usb, led_table);

static struct usb_driver led_driver =
{
	.name = "led_driver",
	.id_table = led_table,
	.probe = led_probe,
	.disconnect = led_disconnect,
};

static int __init led_init(void)
{
	int val;
	val= usb_register(&led_driver);

	led.name = kasprintf(GFP_KERNEL, "demo-led:r:dummy");
   	led.brightness_set = control_led;
   	led.brightness = LED_OFF;
   	led.max_brightness = 1;

	led_classdev_register(NULL, &led);
	printk(KERN_INFO "LED device connected \n");
	return 0;
}

static void __exit led_exit(void)
{

	led_classdev_unregister(&led);
	printk(KERN_INFO "LED device unloaded \n");
	usb_deregister(&led_driver);
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Poulami Pal");
MODULE_DESCRIPTION("USB led Registration Driver");









