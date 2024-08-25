#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>
#include <linux/sched.h>       // For task_struct and process management
#include <linux/mm.h>          // For memory management structures
#include <linux/pid.h>         // For pid and task management
#include <linux/slab.h>        // For kmalloc and kfree

#include "headers.h"

MODULE_LICENSE("GPL");

/**
 * @brief This function is called whenever a device file is opened
 */
static int driver_open(struct inode* device_file, struct file* instance) {
	printk(KERN_INFO "[+] open was called\n");
	return 0;
}

/**
 * @brief This function is called whenever a device file is closed
 */
static int driver_close(struct inode* device_file, struct file* instance) {
	printk(KERN_INFO "[+] close was called\n");
	return 0;
}

// Global variable for read/write
int32_t answer = 42;

static long int ioctl(struct file* File, unsigned cmd, unsigned long arg) {
	struct greeter_struct gs;
	struct proc_mem pm;
	struct task_struct* task;
	int retval = 0;
	void* kernel_buffer;

	switch (cmd) {
	case WR_VALUE:
		if (copy_from_user(&answer, (int32_t*)arg, sizeof(answer))) {
			printk(KERN_ALERT "[-] error copying data from user\n");
		}
		else
			printk(KERN_INFO "[+] update the answer to %d\n", answer);
		break;

	case RD_VALUE:
		if (copy_to_user((int32_t*)arg, &answer, sizeof(answer))) {
			printk(KERN_ALERT "[-] error copying data to user\n");
		}
		else
			printk(KERN_INFO "[+] the answer was copied\n");
		break;

	case GREETER:
		if (copy_from_user(&gs, (struct greeter_struct*)arg, sizeof(gs))) {
			printk(KERN_ALERT "[-] error copying data from user\n");
		}
		else
			printk(KERN_INFO "[+] %d greets to %s\n", gs.repeat, gs.name);
		break;

	case RD_PROC_MEM:
	case WR_PROC_MEM:
		if (copy_from_user(&pm, (struct proc_mem*)arg, sizeof(pm))) {
			printk(KERN_ALERT "[-] error copying data from user\n");
			return -EFAULT;
		}

		task = pid_task(find_vpid(pm.pid), PIDTYPE_PID);
		if (!task) {
			printk(KERN_ALERT "[-] could not find the process with PID %d\n", pm.pid);
			return -ESRCH;
		}

		kernel_buffer = kmalloc(pm.length, GFP_KERNEL);
		if (!kernel_buffer) {
			printk(KERN_ALERT "[-] memory allocation failed\n");
			return -ENOMEM;
		}

		if (cmd == RD_PROC_MEM) {
			retval = access_process_vm(task, (unsigned long)pm.address, kernel_buffer, pm.length, FOLL_FORCE);
			if (retval <= 0) {
				printk(KERN_ALERT "[-] error reading memory from process\n");
				kfree(kernel_buffer);
				return -EFAULT;
			}

			if (copy_to_user(pm.buffer, kernel_buffer, pm.length)) {
				printk(KERN_ALERT "[-] error copying data to user\n");
				kfree(kernel_buffer);
				return -EFAULT;
			}

			printk(KERN_INFO "[+] read %zu bytes from process %d at address %p\n", pm.length, pm.pid, pm.address);
			kfree(kernel_buffer);
		}
		else if (cmd == WR_PROC_MEM){
			if (copy_from_user(kernel_buffer, pm.buffer, pm.length)) {
				printk(KERN_ALERT "[-] error copying data from user\n");
				kfree(kernel_buffer);
				return -EFAULT;
			}

			retval = access_process_vm(task, (unsigned long)pm.address, kernel_buffer, pm.length, FOLL_FORCE | FOLL_WRITE);
			if (retval <= 0) {
				printk(KERN_ALERT "[-] error reading memory from process\n");
				kfree(kernel_buffer);
				return -EFAULT;
			}

			printk(KERN_INFO "[+] read %zu bytes from process %d at address %p\n", pm.length, pm.pid, pm.address);
			kfree(kernel_buffer);
		}
		break;
	}

	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.unlocked_ioctl = ioctl
};

#define MYMAJOR 90 //Use a free device id, check it by calling cat /proc/devices

// sudo insmod cs2.ko
// sudo mknod /dev/comdevice c 90 0

// sudo rmmod cs2

static int __init startup(void) {
	int retval;
	printk(KERN_INFO "[*] Kernel module started \n");

	// register device nr 
	retval = register_chrdev(MYMAJOR, "kmdevice", &fops);

	if (retval == 0) {
		printk(KERN_INFO "[+] registered device number - Major: %d, Minor: %d\n", MYMAJOR, 0);
	}
	else if (retval > 0) {
		printk(KERN_INFO "[+] registered device number - Major: %d, Minor: %d\n", retval >> 20, retval & 0xfffff);
	}
	else {
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
