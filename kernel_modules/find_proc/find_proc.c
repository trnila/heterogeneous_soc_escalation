#include <linux/module.h>
#include <linux/cred.h>

static int proc_addresses(char *val, const struct kernel_param *kp) {
  return scnprintf(
      val,
      PAGE_SIZE,
      "current task_struct: %px\ncred: %px",
      (void*) virt_to_phys(current),
      (void*) virt_to_phys(current->cred)
  );
}

static struct kernel_param_ops ops = {
	.get = &proc_addresses,
};
module_param_cb(proc_addresses, &ops, NULL, 0664);
MODULE_LICENSE("GPL v2");
