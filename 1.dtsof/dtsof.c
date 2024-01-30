#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>



static int __init dtsof_init(void)
{

    struct device_node *fan_node = NULL;//找到的节点
    struct property    *fan_compat = NULL;//保存找到的属性
    int ret;//返回值
    const char *str;//保存找到的字符串

    /*1. find the node "pwm-fan", path : /pwm-fan*/
    fan_node = of_find_node_by_path("/pwm-fan");
    if(fan_node == NULL)
    {
        printk("no fan_pwm node!");
        return -1;
    }

    /*2.开始获取属性*/
    fan_compat = of_find_property(fan_node,	"compatible", NULL);
    if(fan_compat == NULL)
    {
        printk("no fan_compat!");
        return -1;
    }
    else
    {
        printk("compatible=%s\r\n",(char*)fan_compat->value);
    }



    /*3.用字符串的形式获取属性值*/
    ret = of_property_read_string(fan_node,"status",&str);
    if(ret < 0)
    {
        printk("no fan_compat!");
        return -1;
    }
    else 
    {
        printk("status=%s\r\n",(char*)str);
    }

    /*4.读取数字属性*/
    

    return 0;
}
  

static void __exit dtsof_exit(void)
{

}


/*注册模块出入口*//*出入平安*/
module_init(dtsof_init);
module_exit(dtsof_exit);
MODULE_LICENSE("GPL");