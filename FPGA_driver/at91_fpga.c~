/************************************************************
Copyright (C), 2014-2024, CRIRP.

FileName: 	at91_fpga.h
Author: Version : Date: Joe(zhouxiang0532@163.com)
Description: // 驱动程序
1. -------
History: // 历史修改记录
<author> <time> <version > <desc>
Joe		2014/3/18 1.0 build this module for linux (2.6.27)
***********************************************************/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/timer.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/platform_device.h>

#include <asm/uaccess.h>
#include <mach/hardware.h>				//必须
#include <mach/io.h>				    //包含at91_sys_write()
#include <mach/at91sam9263.h>			//包含各个基地址(必须)
#include <mach/at91sam9263_matrix.h>    //包含EBI_SMC2_CSR[]
#include <mach/at91_pio.h>				//包含GPIO寄存器
#include <mach/at91_pmc.h>				//包含GPIO寄存器
#include <mach/gpio.h>
#include <mach/at91sam9_smc.h>
#include <mach/sam9_smc.h>

#include "port.h"

/* undef it, just in case */
#define FPGA_DEBUG
/* This one if debugging is on, and kernel space */
#ifdef FPGA_DEBUG
	#define DPRINTK(fmt, args...)  printk("fpga: "fmt, ## args)
#else
    #define DPRINTK(fmt, args...)
#endif

/*---------------------------------------------------------
//系统外设基地址：
//0x6000,0000: NCS5；
//端口地址定义：
---------------------------------------------------------*/
#define		PHYADDR		0x60000000L //cs5
#define		OFFSETADD		0x3

//等待队列
static DECLARE_WAIT_QUEUE_HEAD(port_in_queue);

/*-----------------------0----------------------------------
//全局变量、常量定义:
---------------------------------------------------------*/
#define FPGA_NAME		"port"

int		port_major = 0;

static struct class *port_class = NULL;

static unsigned int PortLen=0x400;

//设备指针
static struct cdev	port_chrdev;

//地址参数
static unsigned long Port_VirAddr=0;
static unsigned long Port_PhyAddr=0;

/*---------------------------------------------------------
//open函数；
//---------------------------------------------------------*/
static int port_open(struct inode * s_node,struct file * s_file)
{
	DPRINTK("Here I am: %s:%i\n", __FILE__, __LINE__); 
	try_module_get(THIS_MODULE);
	return 0;
}
/*---------------------------------------------------------
//close函数；
//---------------------------------------------------------*/
static int port_close(struct inode * s_node,struct file * s_file)
{
	module_put(THIS_MODULE);
	return 0;
}
/*---------------------------------------------------------
//ioctl函数；
//---------------------------------------------------------*/
static int port_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret=0;
	port_data_t pdata;

	switch (cmd)
	{
		//读
		case IOCTL_PORT_READ:
			if(copy_from_user(&pdata, (char *)arg, sizeof(port_data_t)))
			{
				DPRINTK("Here I am : %s:%i\n", __FILE__, __LINE__); 
				return -EFAULT;
			}
			
			pdata.regData= (int) __raw_readb(Port_VirAddr + (pdata.regAddr<<2)) ;
			//DPRINTK("(Rd)gData=0x%x;gAddr=0x%x\r\n",pdata.regData,pdata.regAddr);
			if(copy_to_user((char *)arg, &pdata, sizeof(port_data_t)))
			{
				DPRINTK("Here I am : %s:%i\n", __FILE__, __LINE__); 
				return -EFAULT;
			}
		break;

		//写
		case IOCTL_PORT_WRITE:
			if(copy_from_user(&pdata, (char *)arg, sizeof(port_data_t)))
				return -EFAULT;

			__raw_writeb((char)pdata.regData, Port_VirAddr + (pdata.regAddr<<2));
			//DPRINTK("(Wr)gData=0x%x;gAddr=0x%x\r\n",pdata.regData,pdata.regAddr);
			break;
		default:
			ret = -1;
	}

	return ret;
}

//*********************************************************
//  define file operations
//*********************************************************
struct file_operations port_fops =
{
	.open		=	port_open,
	.release	=	port_close,
	.ioctl    	=  	port_ioctl,
};

/*
 * SMC timings for the XXXX.
 * Note: These timings were calculated for MASTER_CLOCK = 100000000 according to the XXXX timings.
 */
static struct sam9_smc_config __initdata fpga_smc_config = 
{
	.ncs_read_setup		= 2,
	.nrd_setup		= 2,
	.ncs_write_setup	= 2,
	.nwe_setup		= 2,

	.ncs_read_pulse		= 24,
	.nrd_pulse		= 24,
	.ncs_write_pulse	= 24,
	.nwe_pulse		= 24,

	.read_cycle		= 30,
	.write_cycle		= 30,
	
	.mode			= AT91_SMC_READMODE | AT91_SMC_WRITEMODE | AT91_SMC_EXNWMODE_DISABLE | AT91_SMC_BAT_WRITE | AT91_SMC_DBW_8,
	.tdf_cycles		= 2,
};

//*********************************************************
//初始化模块函数；
//*********************************************************
static int __init port_init(void)
{
    int ret;
    dev_t devt = 0;

	/* Figure out our device number. */
    if (port_major)
    {
        devt = MKDEV(port_major, 0);
        ret = register_chrdev_region(devt, 1, FPGA_NAME);
    }
    else
    {
        ret = alloc_chrdev_region(&devt, 0, 1, FPGA_NAME);
        port_major = MAJOR(devt);
    }
    if ( ret<0 )
    {
        printk(KERN_WARNING "FPGA_MODULE_NAME: unable to get major %d\n", port_major);
        return ret;
    }


	// NCS4 (防止该空间处于未知状态)	
	at91_set_A_periph(AT91_PIN_PD6, 0);    /* NCS4 */
	/* Configure chip-select 4 (tl16c554) */
	sam9_smc_configure(4, &fpga_smc_config); /* ?? */

	 //定义EBI控制寄存器的读写参数
	// NCS5 	
	at91_set_A_periph(AT91_PIN_PD7, 0);    /* NCS5 */
	/* Configure chip-select 5  */
	sam9_smc_configure(5, &fpga_smc_config); /* ?? */

	Port_PhyAddr=PHYADDR;
	//申请内存区域，以检测该地址空间是否被使用；
	if (!request_mem_region(Port_PhyAddr, PortLen, FPGA_NAME))
	{
		DPRINTK("Error request 0x%lx address! \r\n", Port_PhyAddr);
		return -1;
	}

	//进行内存区域的映射，把物理地址映射为逻辑地址；
	Port_VirAddr = (unsigned long)ioremap_nocache(Port_PhyAddr, PortLen);

	DPRINTK("(probe)Port_PhyAddr=0x%x; Port_VirAddr=0x%x\r\n", Port_PhyAddr, Port_VirAddr);
	
	cdev_init(&port_chrdev, &port_fops); /* initialize your device settings */
	port_chrdev.owner = THIS_MODULE;	//可能用不到
	port_chrdev.ops = &port_fops;	//可能用不到
	
	ret = cdev_add(&port_chrdev, devt, 1); /* register your device to the kernel */
	if (ret)
	{
		DPRINTK("fail to register driver for %s!\n",FPGA_NAME);
		return ret;
	}

	
	port_class = class_create(THIS_MODULE, FPGA_NAME);
	device_create(port_class, NULL, devt, NULL, FPGA_NAME); /* add module to /dev/port */

    return ret;
}

//*********************************************************
//清除模块函数；
//*********************************************************
static void __exit port_cleanup(void)
{
	if(Port_VirAddr>0)
	{
		//释放申请的内存区域；
		iounmap((void *)Port_VirAddr);
		release_mem_region(Port_PhyAddr,PortLen);
		Port_VirAddr = 0;
	}
		
	cdev_del(&port_chrdev);

	class_destroy(port_class);

	unregister_chrdev_region(MKDEV(port_major, 0), 1);

	printk("%s removed!\n",FPGA_NAME);

}


module_init(port_init);
module_exit(port_cleanup);

MODULE_AUTHOR("Joe");
MODULE_DESCRIPTION("AT91-FPGA Driver (PORT)");
MODULE_LICENSE("GPL");
