#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h> /* for put_user */
#define SUCCESS 0
#define DEVICE_NAME "chardev" // dev name in  /proc/devices.
#define BUF_LEN 80 // max lenght of message from the device.


int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *,size_t, loff_t *);
static ssize_t device_write( struct file *, const char *,size_t, loff_t *);

static int Major;
static int Device_Open = 0; // is device open? used to prevent multiple access.

static char msg[BUF_LEN]; // the msg the device will give when asked.
static char *msg_ptr;


static struct file_operations fops ={
	.read =device_read,
	.write =device_write,
	.open =device_open,
	.release =device_release
};


int init_module(void)
{
	Major = register_chrdev(0,DEVICE_NAME,&fops);

	if (Major < 0) {
		printk(KERN_INFO "Registering char device failed: %d\n",Major);
		return Major;
	}

	printk(KERN_INFO "Device assigned major number: %d\n",Major);
	printk(KERN_INFO "do mknod /dev/%s c %d 0.\n",DEVICE_NAME,Major);

	return SUCCESS;
}

void cleanup_module(void)
{
	unregister_chrdev(Major,DEVICE_NAME);

	printk(KERN_INFO "Module removed from Kernel\n");
}
static int device_open(struct inode *inode,struct file *file)
{
	static int counter = 0;
	
	if (Device_Open)
		return -EBUSY;

	Device_Open++;
	sprintf(msg,"I already told you %d times.\n",counter++);
	msg_ptr = msg;
	try_module_get(THIS_MODULE);
	
	return SUCCESS;
}

// called when a process closes the device file.
static int device_release(struct inode *inode,struct file *file)
{
	Device_Open--;
	printk(KERN_INFO "Device opened for read.\n");
	module_put(THIS_MODULE);

	return 0;
}

// called when a process which already have opened the dev file,attempts to read from it.
static ssize_t device_read(struct file *filp,char *buffer,size_t lenght,loff_t *offset)
{
	int bytes_read = 0;

	// if we are at the end of message return 0 signifying the end of file.
	if (*msg_ptr == 0)
		return 0;
	while (lenght && *msg_ptr) {
	
		/* the buffer is in user data segment
		 *not int he Kernel so "*" assignment won't work.
		 *We will use put_user which copies data from the
		 *Kernel data segment to user data segment.
		 */
		put_user(*(msg_ptr++),buffer++);
		lenght--;
		bytes_read++;
	}
	// most read functions return the number of bytes put into the buffer.
	return bytes_read;
}

// called when a process writes to dev file: f.e. echo "Hello" > /dev/dev_name
static ssize_t device_write(struct file *filp,const char *buffer,size_t len,loff_t *off)
{
	printk(KERN_ALERT "Operation not supported.\n");
	return -EINVAL;
}
