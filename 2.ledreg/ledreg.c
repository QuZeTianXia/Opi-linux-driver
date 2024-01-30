#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/io.h>
#include <asm/io.h>
#include <linux/fs.h>

static int major = 0;
//static char kernel_buf[1024];
static struct class *ledreg_class;

#define MIN(a, b) (a < b ? a : b)

/*需要操作的寄存器*/
static volatile unsigned int *CRU_GATE_CON16;/*bit[14] = 0,bit[30] = 1*/
static volatile unsigned int *BUS_IOC_GPIO1A_IOMUX_SEL_L;/*bit[7:4] = 0,bit[23:20] = 1*/
static volatile unsigned int *GPIO1_SWPORT_DDR_L;/*bit[1] = 1出0入，bit[17] = 1*/
static volatile unsigned int *GPIO1_SWPORT_DR_L;/*bit[1] = 1高0低，bit[17] = 1*/


static int ledreg_open (struct inode *node, struct file *file)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

    /*1.建立地址映射*/ 
    if(!CRU_GATE_CON16)  //判断有没有映射过
    {
        CRU_GATE_CON16 = ioremap(0xFD7C0000 + 0x0840,4);
        BUS_IOC_GPIO1A_IOMUX_SEL_L = ioremap(0xFD5F8000 + 0x0020,4);
        GPIO1_SWPORT_DDR_L = ioremap(0xFEC20000 + 0x0008,4);
        GPIO1_SWPORT_DR_L = ioremap(0xFEC20000 + 0x0000,4);
    }

    /*2.初始化GPIO*/
    *CRU_GATE_CON16 = (0 << 14) | (1 << 30);//开启时钟
    *BUS_IOC_GPIO1A_IOMUX_SEL_L = (0 << 4) | (1 << 20);//设置IOMUX选择为GPIO模式
    *GPIO1_SWPORT_DDR_L = (1 << 1) | (1 << 17);//设置GPIO模式为OUTPUT//rk3588每个寄存器都有write_mask位所以不需要用|=

	return 0;
}

static int ledreg_close (struct inode *node, struct file *file)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}

static ssize_t ledreg_read (struct file *file, char __user *buf, size_t size, loff_t *offset)
{
//	int err;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}

static ssize_t ledreg_write (struct file *file, const char __user *buf, size_t size, loff_t *offset)
{
	int err;
    char status;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

    err = copy_from_user(&status, buf, MIN(1024, size));

    /*根据传入的status驱动GPIO*/
    if(status)
    {
        *GPIO1_SWPORT_DR_L = (1 << 1) | (1 << 17);
    }
    else
    {
        *GPIO1_SWPORT_DR_L = (0 << 1) | (1 << 17);
    }


	return MIN(1024, size);
}

static struct file_operations ledreg_opr={
	.owner = THIS_MODULE,
	.open  = ledreg_open,
	.release = ledreg_close,	
	.write = ledreg_write,
	.read  = ledreg_read
};


static int __init ledreg_init(void)
{
	printk("ledreg_init\r\n");

	/*注册字符设备*/
	major = register_chrdev(0, "qztxledreg", &ledreg_opr);  /* /dev/qztxledreg */
	if(major < 0)
	{
		printk("ledreg_init_error\r\n");
	}

	ledreg_class = class_create(THIS_MODULE, "ledreg_class");

	device_create(ledreg_class, NULL, MKDEV(major, 0), NULL, "qztxledreg"); /* /dev/ledreg */
	

	return 0;
}

static void __exit ledreg_exit(void)
{
	printk("ledreg_exit\r\n");

	/*注销字符设备*/

	device_destroy(ledreg_class, MKDEV(major, 0));
	class_destroy(ledreg_class);

	unregister_chrdev(major, "qztxledreg");
}

//注册模块出入口

module_init(ledreg_init)
module_exit(ledreg_exit)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Qutianze");