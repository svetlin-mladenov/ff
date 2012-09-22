#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/cdev.h>
#include <linux/fs.h>

int val = 0xff;
module_param(val, int, S_IRUGO);

int ff_major = 0;
module_param(ff_major, int, S_IRUGO);
int ff_minor = 0;
module_param(ff_minor, int, S_IRUGO);

ssize_t ff_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {

}

ssize_t ff_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {

}

struct cdev ff_cdev;
struct file_operations ff_ops = {
		.owner = THIS_MODULE,
		.read = ff_read,
		.write = ff_write,
};

const char *ff_device_name() {
	return "ff";
}

dev_t ff_create_dev_region(dev_t *region) {
	int result = -1;
	if (ff_major) {
		*region = MKDEV(ff_major, ff_minor);
		result = register_chrdev_region(*region, 1, ff_device_name());
	} else {
		result = alloc_chrdev_region(region, ff_minor, 1, ff_device_name());
		ff_major = MAJOR(region);
	}

	return result;
}

int setup_ff_cdev(dev_t region) {
	cdev_init(&ff_dev, )

}

int __init ff_init(void) {
	dev_t dev;
	int result = -1;

	result = ff_create_dev_region(&dev);
	if (result < 0) {
		printk(KERN_ERR "Cannot get a new chardev region");
		goto error;
	}

	result = setup_ff_cdev();
	if (result < 0) {
		printk(KERN_ERR "Ccannot setup a the new chardev");
		goto error;
	}

	return 0;

error:
	return result;
}

void __exit ff_exit(void) {
}

module_init(ff_init);
module_exit(ff_exit);
MODULE_LICENSE("GPL");


