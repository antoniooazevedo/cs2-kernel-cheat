#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>

#include "headers.h"

MODULE_LICENSE("GPL");

/**
 * @brief This function is called whenever a device file is opened
 */
static int driver_open(struct inode *device_file, struct file *instance){
	printk(KERN_INFO "[+] open was called\n");
	return 0;
}

/**
 * @brief This function is called whenever a device file is closed 
 */
static int driver_close(struct inode *device_file, struct file *instance){
	printk(KERN_INFO "[+] close was called\n");
	return 0;
}

// Global variable for read/write
int32_t answer = 42;

static long int ioctl(struct file *File, unsigned cmd, unsigned long arg){
	struct greeter_struct gs;
	switch(cmd){
		case WR_VALUE:
			if (copy_from_user(&answer, (int32_t *) arg, sizeof(answer))){
				printk(KERN_ALERT "[-] error copying data from user\n");
			}
			else
			printk(KERN_INFO "[+] update the answer to %d\n", answer);
			break;

		case RD_VALUE:
			if (copy_to_user((int32_t *) arg, &answer, sizeof(answer))){
				printk(KERN_ALERT "[-] error copying data to user\n");
			}
			else
			printk(KERN_INFO "[+] the answer was copied\n");
			break;

		case GREETER:
			if (copy_from_user(&gs, (struct greeter_struct *) arg, sizeof(gs))){
				printk(KERN_ALERT "[-] error copying data from user\n");
			}
			else
			printk(KERN_INFO "[+] %d greets to %s\n", gs.repeat, gs.name);
			break;
	}
	return 0;
}

static struct file_operations fops={
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.unlocked_ioctl = ioctl
};

#define MYMAJOR 90 //Use a free device id, check it by calling cat /proc/devices

static int __init startup(void){
	int retval;
	printk(KERN_INFO "[*] Kernel module started \n");

	// register device nr 
	retval = register_chrdev(MYMAJOR, "my_dev_nr", &fops);

	if (retval == 0){
		printk(KERN_INFO "[+] registered device number - Major: %d, Minor: %d\n", MYMAJOR, 0);
	}
	else if (retval > 0){
		printk(KERN_INFO "[+] registered device number - Major: %d, Minor: %d\n", retval>>20, retval&0xfffff);
	}
	else{
		printk(KERN_ALERT "[-] could not register device number\n");
		return -1;
	}

	return 0;
}	

static void __exit cleanup(void) {
	unregister_chrdev(MYMAJOR, "my_dev_nr");
	printk(KERN_INFO "[*] Kernel module unloaded \n");
}

module_init(startup);
module_exit(cleanup);

