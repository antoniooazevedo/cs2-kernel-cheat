#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");

/**
 * @brief This function is called whenever a device file is opened
 */
static int driver_open(struct inode *device_file, struct file *instance){
	printk(KERN_INFO "[device_nr] open was called\n");
	return 0;
}

/**
 * @brief This function is called whenever a device file is closed 
 */
static int driver_close(struct inode *device_file, struct file *instance){
	printk(KERN_INFO "[device_nr] close was called\n");
	return 0;
}

static struct file_operations fops={
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close
};

#define MYMAJOR 90 //Use a free device id, check it by calling cat /proc/devices

static int __init startup(void){
	int retval;
	printk(KERN_INFO "Hello from the kernel!\n");

	// register device nr 
	retval = register_chrdev(MYMAJOR, "my_dev_nr", &fops);

	if (retval == 0){
		printk(KERN_INFO "[device_nr] registered device number - Major: %d, Minor: %d\n", MYMAJOR, 0);
	}
	else if (retval > 0){
		printk(KERN_INFO "[device_nr] registered device number - Major: %d, Minor: %d\n", retval>>20, retval&0xfffff);
	}
	else{
		printk(KERN_ALERT "[device_nr] could not register device number\n");
		return -1;
	}

	return 0;
}	

static void __exit cleanup(void) {
	unregister_chrdev(MYMAJOR, "my_dev_nr");
	printk(KERN_INFO "Goodbye kernel!\n");
}

module_init(startup);
module_exit(cleanup);

