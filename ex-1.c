#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

static int arg __initdata = 3;
static int i;

static int __init hello3(void)
{
	arg++;

	printk(KERN_INFO "Module inserted into kernel. Argument: %d \n",arg);


	for (i = 0; i < 10; i++)
		printk(KERN_INFO "%d\n",i);

	return 0;
}

static void __exit hello3_exit(void)
{
	printk(KERN_INFO "Module removed.\n");
}

module_init(hello3);
module_exit(hello3_exit);
