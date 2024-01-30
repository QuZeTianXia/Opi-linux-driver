#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/io.h>
#include <asm/io.h>
#include <linux/fs.h>








static int __init leddts_init(void)
{
    printk("leddts_init\r\n");

	/*注册字符设备*/
	major = register_chrdev(0, "qztxleddts", &leddts_opr);  /* /dev/qztxleddts */
	if(major < 0)
	{
		printk("leddts_init_error\r\n");
	}

	leddts_class = class_create(THIS_MODULE, "leddts_class");

	device_create(leddts_class, NULL, MKDEV(major, 0), NULL, "qztxleddts"); /* /dev/leddts */
	
    return 0;
}

static void __exit leddts_exit(void)
{
    printk("leddts_exit\r\n");

	/*注销字符设备*/

	device_destroy(leddts_class, MKDEV(major, 0));
	class_destroy(leddts_class);

	unregister_chrdev(major, "qztxleddts");
}


module_init(leddts_init)
module_exit(leddts_exit)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Qutianze");
