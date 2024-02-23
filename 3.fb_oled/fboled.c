#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/io.h>
#include <asm/io.h>
#include <linux/fs.h>
#include "OLED_Font.h"

static int major = 0;
//static char kernel_buf[1024];
static struct class *ledreg_class;


#define MIN(a, b) (a < b ? a : b)


/*需要操作的寄存器*/
static volatile unsigned int *CRU_GATE_CON16;/*bit[14] = 0,bit[30] = 1*//*开启GPIO1时钟*/
static volatile unsigned int *BUS_IOC_GPIO1A_IOMUX_SEL_L;/*bit[7:4] = 0,bit[23:20] = 1*//*GPIO1_A1为IO模式*/
/*bit[3:0] = 0,bit[19:16] = 1*//*GPIO1_A0为IO模式*/
static volatile unsigned int *GPIO1_SWPORT_DDR_L;/*bit[1] = 1出0入，bit[17] = 1*//*GPIO1_A1为OUTPUT模式*/
/*bit[0] = 1出0入，bit[16] = 1*//*GPIO1_A0为OUTPUT模式*/
static volatile unsigned int *GPIO1_SWPORT_DR_L;/*bit[1] = 1高0低，bit[17] = 1*//*GPIO1_A1的输出*/
/*bit[0] = 1高0低，bit[16] = 1*//*GPIO1_A0的输出*/


#ifndef OLED_Function
#define OLED_Function

unsigned char OLED_GRAM[144][8];

void OLED_W_SCL(int x)
{
	*GPIO1_SWPORT_DR_L = (x << 1) | (1 << 17);
}
void OLED_W_SDA(int x)
{
	*GPIO1_SWPORT_DR_L = (x << 0) | (1 << 16);
}

/*引脚初始化*/
void OLED_I2C_Init(void)
{
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
  * @brief  I2C开始
  * @param  无
  * @retval 无
  */
void OLED_I2C_Start(void)
{
	OLED_W_SDA(1);
	OLED_W_SCL(1);
	OLED_W_SDA(0);
	OLED_W_SCL(0);
}

/**
  * @brief  I2C停止
  * @param  无
  * @retval 无
  */
void OLED_I2C_Stop(void)
{
	OLED_W_SDA(0);
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
  * @brief  I2C发送一个字节
  * @param  Byte 要发送的一个字节
  * @retval 无
  */
void OLED_I2C_SendByte(unsigned char Byte)
{
	unsigned char i;
	for (i = 0; i < 8; i++)
	{
		OLED_W_SDA(Byte & (0x80 >> i));
		OLED_W_SCL(1);
		OLED_W_SCL(0);
	}
	OLED_W_SCL(1);	//额外的一个时钟，不处理应答信号
	OLED_W_SCL(0);
}

/**
  * @brief  OLED写命令
  * @param  Command 要写入的命令
  * @retval 无
  */
void OLED_WriteCommand(unsigned char Command)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x00);		//写命令
	OLED_I2C_SendByte(Command); 
	OLED_I2C_Stop();
}

/**
  * @brief  OLED写数据
  * @param  Data 要写入的数据
  * @retval 无
  */
void OLED_WriteData(unsigned char Data)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x40);		//写数据
	OLED_I2C_SendByte(Data);
	OLED_I2C_Stop();
}

/**
  * @brief  OLED设置光标位置
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
void OLED_SetCursor(unsigned char Y, unsigned char X)
{
	OLED_WriteCommand(0xB0 | Y);					//设置Y位置
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//设置X位置高4位
	OLED_WriteCommand(0x00 | (X & 0x0F));			//设置X位置低4位
}

/**
  * @brief  OLED清屏
  * @param  无
  * @retval 无
  */
void OLED_Clear(void)
{  
	unsigned char i, j;
	for (j = 0; j < 8; j++)
	{
		OLED_SetCursor(j, 0);
		for(i = 0; i < 128; i++)
		{
			OLED_WriteData(0x00);
		}
	}
}

/**
  * @brief  OLED显示一个字符
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Char 要显示的一个字符，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowChar(unsigned char Line, unsigned char Column, char Char)
{      	
	unsigned char i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);			//显示上半部分内容
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);		//显示下半部分内容
	}
}

/**
  * @brief  OLED显示字符串
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowString(unsigned char Line, unsigned char Column, char *String)
{
	unsigned char i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, Column + i, String[i]);
	}
}

/**
  * @brief  OLED次方函数
  * @retval 返回值等于X的Y次方
  */
unsigned int OLED_Pow(unsigned int X, unsigned int Y)
{
	unsigned int Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/**
  * @brief  OLED显示数字（十进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~4294967295
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowNum(unsigned char Line, unsigned char Column, unsigned int Number, unsigned char Length)
{
	unsigned char i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED显示数字（十进制，带符号数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：-2147483648~2147483647
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowSignedNum(unsigned char Line, unsigned char Column, int Number, unsigned char Length)
{
	unsigned char i;
	unsigned int Number1;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+');
		Number1 = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED显示数字（十六进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
  * @param  Length 要显示数字的长度，范围：1~8
  * @retval 无
  */
void OLED_ShowHexNum(unsigned char Line, unsigned char Column, unsigned int Number, unsigned char Length)
{
	unsigned char i, SingleNumber;
	for (i = 0; i < Length; i++)							
	{
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			OLED_ShowChar(Line, Column + i, SingleNumber + '0');
		}
		else
		{
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

/**
  * @brief  OLED显示数字（二进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~1111 1111 1111 1111
  * @param  Length 要显示数字的长度，范围：1~16
  * @retval 无
  */
void OLED_ShowBinNum(unsigned char Line, unsigned char Column, unsigned int Number, unsigned char Length)
{
	unsigned char i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
	}
}

void OLED_DrawPoint(unsigned char x,unsigned char y,unsigned char t)
{
	unsigned char i,m,n;
	i=y/8;
	m=y%8;
	n=1<<m;
	if(t){OLED_GRAM[x][i]|=n;}
	else
	{
		OLED_GRAM[x][i]=~OLED_GRAM[x][i];
		OLED_GRAM[x][i]|=n;
		OLED_GRAM[x][i]=~OLED_GRAM[x][i];
	}
}
void OLED_ShowPicture(unsigned char x,unsigned char y,unsigned char sizex,unsigned char sizey,unsigned char BMP[],unsigned char mode)
{
	unsigned int j=0;
	unsigned char i,n,temp,m;
	unsigned char x0=x,y0=y;
	sizey=sizey/8+((sizey%8)?1:0);
	for(n=0;n<sizey;n++)
	{
		 for(i=0;i<sizex;i++)
		 {
				temp=BMP[j];
				j++;
				for(m=0;m<8;m++)
				{
					if(temp&0x01)OLED_DrawPoint(x,y,mode);
					else OLED_DrawPoint(x,y,!mode);
					temp>>=1;
					y++;
				}
				x++;
				if((x-x0)==sizex)
				{
					x=x0;
					y0=y0+8;
				}
				y=y0;
     }
	 }
}

/**
  * @brief  OLED初始化
  * @param  无
  * @retval 无
  */
void OLED_Init(void)
{
	unsigned int i, j;
	
	for (i = 0; i < 1000; i++)			//上电延时
	{
		for (j = 0; j < 1000; j++);
	}
	
	OLED_I2C_Init();			//端口初始化
	
	OLED_WriteCommand(0xAE);	//关闭显示
	
	OLED_WriteCommand(0xD5);	//设置显示时钟分频比/振荡器频率
	OLED_WriteCommand(0x80);
	
	OLED_WriteCommand(0xA8);	//设置多路复用率
	OLED_WriteCommand(0x3F);
	
	OLED_WriteCommand(0xD3);	//设置显示偏移
	OLED_WriteCommand(0x00);
	
	OLED_WriteCommand(0x40);	//设置显示开始行
	
	OLED_WriteCommand(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置
	
	OLED_WriteCommand(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

	OLED_WriteCommand(0xDA);	//设置COM引脚硬件配置
	OLED_WriteCommand(0x12);
	
	OLED_WriteCommand(0x81);	//设置对比度控制
	OLED_WriteCommand(0xCF);

	OLED_WriteCommand(0xD9);	//设置预充电周期
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB);	//设置VCOMH取消选择级别
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4);	//设置整个显示打开/关闭

	OLED_WriteCommand(0xA6);	//设置正常/倒转显示

	OLED_WriteCommand(0x8D);	//设置充电泵
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);	//开启显示
		
	OLED_Clear();				//OLED清屏
}

#endif

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
    *CRU_GATE_CON16 = (0 << 14) | (1 << 30);//开启GPIO1时钟
    *BUS_IOC_GPIO1A_IOMUX_SEL_L = (0 << 4) | (1 << 20) | (0 << 0) | (1 << 16);//设置IOMUX选择GPIO1_A1和GPIO1_A0为GPIO模式
    *GPIO1_SWPORT_DDR_L = (1 << 1) | (1 << 17) | (1 << 0) | (1 << 16);//设置GPIO1_A1和GPIO1_A0模式为OUTPUT//rk3588每个寄存器都有write_mask位所以不需要用|=

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

	OLED_Init();
	OLED_ShowString(1,0,"Hello World!");
    /*根据传入的status驱动GPIO*/
//    if(status)
//   {
//        //*GPIO1_SWPORT_DR_L = (1 << 1) | (1 << 17);
//		GPIO1_A1_HIGH();
//
//    }
//    else
//    {
//       // *GPIO1_SWPORT_DR_L = (0 << 1) | (1 << 17);
//	   	GPIO1_A1_LOW();
//    }


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