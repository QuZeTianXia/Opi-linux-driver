#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/device.h>


#include <linux/fs.h>

static int major = 0;
static char kernel_buf[1024];
static struct class *chrdevbase_class;

#define MIN(a, b) (a < b ? a : b)

static int chrdevbase_open (struct inode *node, struct file *file)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}

static int chrdevbase_close (struct inode *node, struct file *file)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}

static ssize_t chrdevbase_read (struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	int err;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	err = copy_to_user(buf, kernel_buf, MIN(1024, size));
	return MIN(1024, size);
}

static ssize_t chrdevbase_write (struct file *file, const char __user *buf, size_t size, loff_t *offset)
{
	int err;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	err = copy_from_user(kernel_buf, buf, MIN(1024, size));
	return MIN(1024, size);
}

static struct file_operations chrdevbase_opr={
	.owner = THIS_MODULE,
	.open  = chrdevbase_open,
	.release = chrdevbase_close,	
	.write = chrdevbase_write,
	.read  = chrdevbase_read
};


static int __init chrdevbase_init(void)
{
	printk("chrdevbase_init\r\n");

	/*注册字符设备*/
	major = register_chrdev(0, "qztxchrdev", &chrdevbase_opr);  /* /dev/qztxchrdev */
	if(major < 0)
	{
		printk("chrdevbase_init_error\r\n");
	}

	chrdevbase_class = class_create(THIS_MODULE, "chrdevbase_class");

	device_create(chrdevbase_class, NULL, MKDEV(major, 0), NULL, "qztxchrdev"); /* /dev/hello */
	

	return 0;
}

static void __exit chrdevbase_exit(void)
{
	printk("chrdevbase_exit\r\n");

	/*注销字符设备*/

	device_destroy(chrdevbase_class, MKDEV(major, 0));
	class_destroy(chrdevbase_class);

	unregister_chrdev(major, "qztxchrdev");
}

//注册模块出入口

module_init(chrdevbase_init)
module_exit(chrdevbase_exit)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Qutianze");