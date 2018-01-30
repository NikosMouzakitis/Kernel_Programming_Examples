#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include <linux/uaccess.h>

#define BUF_LEN 16
#define test_msg "this is a test"
#define dev_name "mytest"

struct proc_dir_entry *myprocfile;
	
static char message[BUF_LEN];
static char *msg_p;

static int 
test_open(struct inode *inode, struct file *f)
{
	printk(KERN_INFO "device open\n");
	msg_p = message;

	return 0;
}

static int 
test_release(struct inode *inode,struct file *f)
{
	printk(KERN_INFO "device closed\n");
	
	return 0;
}

static ssize_t
test_write(struct file *file, const char __user *buffer, size_t size, loff_t *offset)
{
	unsigned int i;
	char __user *p = buffer;

	msg_p = message;
	
	printk(KERN_INFO "Argument size: %lu\n",size);
	printk(KERN_INFO "Argument offset: %lld\n",*offset);
	
	if ( size >= BUF_LEN)
		return -EFAULT;
	
	for (i = *offset; size > 0 && i < BUF_LEN; ++i,++p,--size,msg_p++) {
		if (__get_user(*msg_p,p))
			return -EFAULT;
	}

	printk(KERN_INFO "Dev been written\n");
	
	for (i = 0; i < 16; i++)
		printk(KERN_INFO "%c",message[i]);
	
	return p-buffer;
}

static ssize_t
test_read(struct file *file, char __user *ubuffer, size_t size, loff_t *offset)
{
	int bytes_read = 0;
	char __user *buffer = ubuffer;	
	unsigned int mycnt = 0;
	msg_p = message;

	printk(KERN_INFO "Argument size: %lu\n",size);
	printk(KERN_INFO "Argument offset: %lld\n",*offset);
	printk(KERN_INFO "Dev been readed\n");
	

	if (size > BUF_LEN) {
		printk(KERN_ALERT "Adjusting size.\n");
		size = BUF_LEN;
		printk(KERN_INFO "size: %d\n",size);
	}
	

	while (size &&  (*msg_p) ) {
		put_user( *msg_p,buffer);
		msg_p++;
		buffer++;	
		size--;
		mycnt++;
		bytes_read++;
	}
	// returning the number of bytes put into the buffer.
	return bytes_read;
}

struct file_operations fops ={
	.owner = THIS_MODULE,
	.open = test_open,
	.read = test_read,
	.write = test_write,
	.release = test_release,
};

int 
init_module(void)
{
	int ret = 0;
	int i;

	for (i = 0; i < 14; i++)
		message[i] = test_msg[i];
	message[i] = '\0';

	printk(KERN_INFO "Module in\n");

	myprocfile = proc_create(dev_name,0777,NULL,&fops);

	if (ret)
		return ret;

	printk(KERN_INFO "Registered\n");
	printk(KERN_INFO "Message initialized to : %s\n",message);

	return ret;
}

void
cleanup_module(void)
{
	remove_proc_entry(dev_name,NULL);
	printk(KERN_INFO "Module removed\n");
}	
