#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/types.h>
#include <linux/device.h>

int val = 0xff;
module_param(val, int, S_IRUGO);

uint64_t pattern;

int ff_major = 0;
module_param(ff_major, int, S_IRUGO);
int ff_minor = 0;
module_param(ff_minor, int, S_IRUGO);



#define FF_MODULE_NAME "ff"

static struct class *ff_class = NULL;
static dev_t ff_devno;
/**********************************
 * General purpose macros
 **********************************/

#define DEL(resource, dtor, ...) \
	if (resource) {\
		dtor(resource, ##__VA_ARGS__); \
		resource = NULL; \
	}

/**********************************
 * Macros used to fill the user's buffer with the pattern
 **********************************/

#define __PUT(type, pattern, buff) \
	put_user((type)pattern, (type __user *)buf);

#define __PUT_AND_MOVE_BUFFER(type, pattern, buf, count) \
		__PUT(type, pattern, buf); \
		count -= sizeof(type); \
		buf += sizeof(type)

#define __FILL_USING_TYPED_PATTERN(while_or_if, type, pattern, buf, count) \
	while_or_if (count >= sizeof(type)) {\
		__PUT_AND_MOVE_BUFFER(type, pattern, buf, count); \
	}

#define FILL_USING_8_BYTE_PATTERN(should_loop, pattern, buf, count) \
		__FILL_USING_TYPED_PATTERN(should_loop, uint64_t, (pattern), buf, count);

#define FILL_USING_4_BYTE_PATTERN(should_loop, pattern, buf, count) \
		__FILL_USING_TYPED_PATTERN(should_loop, uint32_t, (pattern), buf, count);

#define FILL_USING_2_BYTE_PATTERN(should_loop, pattern, buf, count) \
		__FILL_USING_TYPED_PATTERN(should_loop, uint16_t, (pattern), buf, count);

#define FILL_USING_1_BYTE_PATTERN(should_loop, pattern, buf, count) \
		__FILL_USING_TYPED_PATTERN(should_loop, uint8_t, (pattern), buf, count);

#define FILL_MULTIPLE while
#define FILL_ONCE if

ssize_t ff_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {
	const size_t requested = count;

	FILL_USING_8_BYTE_PATTERN(FILL_MULTIPLE, pattern, buf, count);
	FILL_USING_4_BYTE_PATTERN(FILL_ONCE, pattern, buf, count);
	FILL_USING_2_BYTE_PATTERN(FILL_ONCE, pattern, buf, count);
	FILL_USING_1_BYTE_PATTERN(FILL_ONCE, pattern, buf, count);

	return requested;
}

ssize_t ff_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {
	return count;
}

struct cdev __ff_cdev;
struct cdev *ff_cdev = NULL;
struct file_operations ff_ops = {
		.owner = THIS_MODULE,
		.read = ff_read,
		.write = ff_write,
		//TODO add lseek and the others from /dev/zero (devices/char/mem???)
};

const char *ff_device_name(void) {
	return "ff";
}

int ff_create_dev_region(dev_t *region) {
	int result = -1;
	if (ff_major) {
		*region = MKDEV(ff_major, ff_minor);
		result = register_chrdev_region(*region, 1, FF_MODULE_NAME);
	} else {
		result = alloc_chrdev_region(region, ff_minor, 1, FF_MODULE_NAME);
		ff_major = MAJOR(*region);
	}

	return result;
}

int setup_ff_cdev(dev_t region) {
	int err = 0;
	struct device *device = NULL;

	ff_class = class_create(THIS_MODULE, FF_MODULE_NAME);
	if (IS_ERR(ff_class)) {
		err = PTR_ERR(ff_class);
		ff_class = NULL;
		goto fail;
	}

	ff_cdev = &__ff_cdev;
	cdev_init(ff_cdev, &ff_ops);
	ff_cdev->owner = THIS_MODULE;
	ff_cdev->ops = &ff_ops;
	err = cdev_add(ff_cdev, region, 1);
	if (unlikely(err < 0)) {
		ff_cdev = NULL;
		goto fail;
	}

	device = device_create(ff_class, NULL /*no parent device*/, region, NULL /*no additional data*/, ff_device_name());
	if (IS_ERR(device)) {
		err = PTR_ERR(device);
		device = NULL;
		goto fail;
	}

	return err;

	fail:
	if (ff_cdev) {
		cdev_del(ff_cdev);
		ff_cdev = NULL;
	}
	if (ff_class) {
		class_destroy(ff_class);
		ff_class = NULL;
	}

	return err;
}

int __init ff_init(void) {
	int result = -1;

	result = ff_create_dev_region(&ff_devno);
	if (result < 0) {
		printk(KERN_ERR "Cannot get a new chardev region");
		goto error;
	}

	result = setup_ff_cdev(ff_devno);
	if (result < 0) {
		printk(KERN_ERR "Cannot setup a the new chardev");
		goto error;
	}

	return 0;

error:
	return result;
}

void ff_cleanup(void) {
	DEL(ff_class, device_destroy, ff_devno);
	DEL(ff_cdev, cdev_del);
	DEL(ff_class, class_destroy);
	unregister_chrdev_region(ff_devno, 1);
}

void __exit ff_exit(void) {
	ff_cleanup();
}

module_init(ff_init);
module_exit(ff_exit);
MODULE_LICENSE("GPL");


