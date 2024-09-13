#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");

// Buffer for read/write data
static char buf[255];
static int bufp;

// Device data
static dev_t dev_nr;
static struct class *dev_class;
static struct cdev device; 

#define DRIVER_NAME "dummydriver"
#define DRIVER_CLASS "MyDriverClass"

/**
 * @brief Read data from the buffer
 */ 
static ssize_t driver_read(struct file *File, char *um_buf, size_t count, loff_t *offs) {
	int to_copy, not_copied, delta;

	// Get amount of data to copy
	to_copy = umin(count, bufp);

	// Copy data to user buffer
	not_copied = copy_to_user(um_buf, buf, to_copy);

	// delta is the number of bytes
	delta = to_copy - not_copied;

	return delta;
}

/**
 * @brief write data to the buffer
 */
static ssize_t driver_write(struct file *File, const char *um_buf, size_t count, loff_t *offs) {
	int to_copy, not_copied, delta;

	// Get amount of data to copy
	to_copy = min(count, sizeof(buf));

	// Copy data to user buffer
	not_copied = copy_from_user(buf, um_buf, to_copy);
	bufp = to_copy;

	// delta is the number of bytes
	delta = to_copy - not_copied;

	return delta;
}

/**
 * @brief This function is called whenever a device file is opened
 */
static int driver_open(struct inode *device_file, struct file *instance){
	printk(KERN_INFO "[read_write] open was called\n");
	return 0;
}

/**
 * @brief This function is called whenever a device file is closed 
 */
static int driver_close(struct inode *device_file, struct file *instance){
	printk(KERN_INFO "[read_write] close was called\n");
	return 0;
}

static struct file_operations fops={
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.read = driver_read,
	.write = driver_write
};

static int __init startup(void){
	int retval;
	printk(KERN_INFO "Hello from the kernel!\n");

	// Allocate device nr
	if (alloc_chrdev_region(&dev_nr, 0, 1, DRIVER_NAME) < 0) {
		printk(KERN_ALERT "[read_write] device nr couldnt be allocated\n");
		return -1;
	}

	printk(KERN_INFO "[read_write] device nr major: %d, minor: %d\n", dev_nr>>20, dev_nr && 0xfffff);

	// Create device class
	if ((dev_class = class_create(DRIVER_CLASS)) == NULL) {
		printk(KERN_ALERT "[read_write] device class could not be created\n");
		goto ClassError;
	}

	// Create device file
	if(device_create(dev_class, NULL, dev_nr, NULL, DRIVER_NAME) == NULL){
		printk(KERN_ALERT "[read_write] device file could not be created\n");
		goto FileError;
	}

	// Initialize device file
	cdev_init(&device, &fops);	
	
	// Registering device to kernel
	if (cdev_add(&device, dev_nr, 1) == -1){
		printk(KERN_ALERT "[read_write] registering of device to kernel failed\n");
		goto AddError;
	}

	return 0;
AddError:
	device_destroy(dev_class, dev_nr);
FileError:
	class_destroy(dev_class);
ClassError:
	unregister_chrdev(dev_nr, DRIVER_NAME);
	return -1;
}	

static void __exit cleanup(void) {
	cdev_del(&device);
	device_destroy(dev_class, dev_nr);
	class_destroy(dev_class);
	unregister_chrdev(dev_nr, DRIVER_NAME);
	printk(KERN_INFO "Goodbye kernel!\n");
}

module_init(startup);
module_exit(cleanup);

