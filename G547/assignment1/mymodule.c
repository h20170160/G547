#include <linux/module.h> 
#include <linux/kernel.h>
#include <linux/init.h> // for printk()


static int mod_init(void){
	printk(KERN_INFO "Hello!\n");
	return 0;
}


static void mod_exit(void){
	printk(KERN_INFO "Bye!\n");
}


module_init(mod_init);
module_exit(mod_exit);
