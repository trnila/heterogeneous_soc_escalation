#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/err.h>
#include <linux/of.h>
#include <linux/gpio/consumer.h>
#include <linux/delay.h>
#include <linux/spi/spi.h>
#include <linux/idr.h>
#include <linux/sched.h>

struct linecam_priv {
	struct spi_device *spi;
	struct gpio_desc *gpio_si;
	struct gpio_desc *gpio_clk;
	int char_minor;
};

#define DEVICE_NAME "linecam"

static int major;
static struct class* char_class  = NULL;
static struct device* char_device = NULL;

static struct mutex mtx;


static DEFINE_IDR(minors);

extern s64			memstart_addr;

static int linecam_char_open(struct inode *inode, struct file *filp) {
	printk("opened %p %p\n", current, virt_to_phys(current));
	printk("real_cred: %p %d\n", virt_to_phys(current->real_cred), current->real_cred->uid.val);
	printk("str at %p\n", virt_to_phys(current->comm));
	return 0;
}

static ssize_t linecam_char_read(struct file *filep, const char __user *buf, size_t len, loff_t *offset) {
	int *uid;

/*	int *uid = &current->real_cred->uid.val;
	*uid = 0;

	uid = &current->real_cred->suid.val;
	*uid = 0;
	
	uid = &current->real_cred->euid.val;
	*uid = 0;*/

//	uid = &current->real_cred->fsuid.val;
//	*uid = 0;

	printk("write %d %p\n", current->real_cred->uid, uid);
	printk(">>%s\n", current->comm);

	return len;
}


static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = linecam_char_open,
	.write = linecam_char_read,
};


static int __init init(void) {
	int err;
	major = register_chrdev(0, DEVICE_NAME, &fops);
	if(major < 0) {
		printk("unable to register_chrdev");
		return major;
	}

	printk(">%d\n", sizeof(int));
	printk(">%d\n", sizeof(current->cred));
	printk(">%d\n", current->cred->uid);


	struct task_struct *init = pid_task(find_vpid(1), PIDTYPE_PID);

		printk("init %x %s\n", virt_to_phys(init->comm), init->comm);
		printk("init %x %p\n", virt_to_phys(init->real_cred), init->real_cred);


	char_class = class_create(THIS_MODULE, "linecam2");
	if (IS_ERR(char_class)){
		printk("register error\n");
		return -1;
	}

	// create device /dev/linecamX
	char_device = device_create(char_class, NULL, MKDEV(major, 0), NULL, "test", 0);
	if (IS_ERR(char_device)){
		idr_remove(&minors, 0);
		return PTR_ERR(char_device);
	}

	return 0;
}

static void __exit fini(void) {
	device_destroy(char_class, MKDEV(major, 0));
	class_unregister(char_class);
	class_destroy(char_class);
	unregister_chrdev(major, DEVICE_NAME);
}

module_init(init);
module_exit(fini);
MODULE_DESCRIPTION("linecam example module");
MODULE_LICENSE("GPL v2");
