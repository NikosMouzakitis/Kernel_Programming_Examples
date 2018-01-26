#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

//Declaring some datatypes
//monitor them thought
//running ' dmesg -w '
/*------------------------*/
// Variable i can now take a parameter at insertion time
// like ' sudo insmod ex-2.ko i=12 '
// Otherwise it will continue keeping its value 3
// like it is already declared in the code.

int i = 3;

module_param(i,int,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

static int __init hello(void)
{

	printk(KERN_INFO "---- Module inserted into Kernel.\n");
	
	printk(KERN_INFO " i: %d\n",i);

	return 0;
}

static void hello_o(void)
{
	printk(KERN_INFO "---- Module removed.\n");
}

module_init(hello);
module_exit(hello_o);
