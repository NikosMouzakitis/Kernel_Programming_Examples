#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/compat.h>
#include <linux/file.h>
#include <linux/highmem.h>
#include <linux/list.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/poll.h>
#include <linux/semaphore.h>
#include <linux/slab.h>
#include <linux/stat.h>
#include <asm/uaccess.h>

#define BUF_LEN 32
#define initial_msg "abcdefghijklmnopqrstuvwxyz"

static char message[BUF_LEN];
static char *msg_p;

static ssize_t
char_write(struct file *file, const char __user *buffer, size_t size, loff_t *offset)
{
	unsigned int i;
	char __user *p = buffer;

	//	void *p = buffer;

	msg_p = message;

	printk(KERN_INFO "SIZE: %d\tOFFSET: %d\n", size, *offset);

	if (size >= BUF_LEN)
		return -EFAULT;

/*		
	for (i = *offset; size > 0 && i < BUF_LEN; ++i,++p,--size,msg_p++)
		if (__get_user(*msg_p,p)) // copies to Kernel-space. From p --> msg_p.
			return -EFAULT;
*/
	copy_from_user(message, p, size);

	printk(KERN_INFO "Dev have been WRITTEN\n");

	return size;
	
	//return (p-buffer);
}

static ssize_t 
char_read(struct file *file, char __user *buffer, size_t size, loff_t *offset)
{
	int bytes_read = 0;
	char __user * ptr;

	msg_p = message;

	ptr = buffer;

	if (size > BUF_LEN) {
		printk(KERN_ALERT "Adjusting size\n");
		size = BUF_LEN;
		printk(KERN_ALERT "SIZE: %d\n",size);
	}

	copy_to_user(buffer, message, size);
/*
	while (size && (*msg_p) ) {
		put_user(*msg_p, ptr); // copies to user-space.From msg_p -->  ptr.
		msg_p++;
		ptr++;
		size--;
		bytes_read++;
	}
*/
	return size;
//	return bytes_read;
}

static int
char_open(struct inode *inode, struct file *f)
{
	int ret = 0;
	
	msg_p = message;

	printk(KERN_INFO "dev open\n");

	return ret;
}

static int
char_release(struct inode *inode, struct file *f)
{
	int ret = 0;

	printk(KERN_INFO "dev close\n");

	return ret;
}

int 
char_mmap(struct file *f, struct vm_area_struct *vma)
{
	int ret = 0;

	return ret;
}

struct file_operations char_dev_ops = {
	.owner = THIS_MODULE,
	.open = char_open,
	.release = char_release,
	.mmap = char_mmap,
	.read = char_read,
	.write = char_write,
};

struct miscdevice char_dev = {
	.name = "mychar",
	.fops = &char_dev_ops,
	.nodename = "mychar",
	.mode = S_IRUGO | S_IWUGO,
};

int 
module_init0(void)
{
	int ret = 0;
	int i;

	printk(KERN_INFO "Module loaded\n");
	ret = misc_register(&char_dev);

	for (i = 0; i < 26; i++)
		message[i] = initial_msg[i];
	message[i] = '\0';

	if (ret)
		return ret;
	printk(KERN_INFO "Registered\n");
	printk(KERN_INFO "buffer initialized to: %s\n",message);
	
	return ret;	
}

void
module_exit0(void)
{
	printk(KERN_INFO "Module unloaded\n");
	misc_deregister(&char_dev);
}

module_init(module_init0);
module_exit(module_exit0);

MODULE_LICENSE("GPL");
