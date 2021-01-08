/************************************************************

FileName: Power_check.c
Author: Version : Date: Joe Zhou
Description: // simple character utility loading localities

<author> <time> <version > <desc>
Joe 26/10/2020 1.0 build this module for linux
***********************************************************/

#include <linux/module.h>
#include <linux/kernel.h>   // printk container_of
#include <linux/init.h>
#include <linux/sched.h>    // current
#include <linux/types.h>    // dev_t MAJOR MINOR MKDEV size_t
#include <linux/fs.h>       // register_chrdev_region unregister alloc file_operations file inode ioctl.h
#include <linux/cdev.h>     // cdev cdev_init
#include <linux/slab.h>
#include <linux/fcntl.h>    // O_ACCMODE

//#include <asm/uaccess.h>    //Mark: error here, should be changed to <linux/uaccess.h>
                                           //or we should use raw_copy_*_user or _copy_*_user
#include <linux/uaccess.h>  // copy_*_user
//#include <asm/semaphore.h>  // struct semaphore


/* This one if debugging is on, and kernel space */
#define SCULL_DEBUG
#ifdef  SCULL_DEBUG
    #define DPRINTK(fmt, args...) printk("SCULL: "fmt, ## args)
#else
    #define DPRINTK(fmt, args...)
#endif

#define SCULL_NAME      "scull"
#define SCULL_MAJOR     0
#define USER_BUFF_SIZE  PAGE_SIZE    //128

//struct
struct scull_device
{
    dev_t devt;
    struct cdev chrdev;
    struct semaphore sem;
    struct class *powerc_class;
    char *user_buff;
    unsigned long size;
};
struct scull_device *scull_dev;   // global variable
int    scull_major = 0;

//*********************************************************
//  return to initial
//*********************************************************
int scull_trim(struct scull_device *dev)
{
    if(dev)
    {
        if(dev->user_buff)
            kfree(dev->user_buff);
        dev->user_buff = NULL;
        dev->size = 0;
    }
    return 0;
}

//*********************************************************
//  open device
//*********************************************************
int scull_open(struct inode *inode, struct file *filp)
{
    struct scull_device *dev;

    dev = container_of(inode->i_cdev, struct scull_device, chrdev);   // (ptr, type, member)
    filp->private_data = dev;

    if( (filp->f_flags & O_ACCMODE) == O_WRONLY )   //if open was write-only
    {
        if (down_interruptible(&dev->sem))
            return -ERESTARTSYS;
        scull_trim(dev);
        up(&dev->sem);
    }
    return 0;
}

//*********************************************************
//  release device
//*********************************************************
int scull_release(struct inode * inode,struct file * filp)
{
    return 0;
}

//*********************************************************
//  read data from device
//*********************************************************
ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    struct scull_device *dev = filp->private_data;
    ssize_t retval = 0;

    if (down_interruptible(&dev->sem)) // P function
        return -ERESTARTSYS;

    if(*f_pos >= dev->size) // if current read pos is greater than device size, return
        goto out;           // which means there's no data available

    if(*f_pos + count > dev->size)  // only read the left part
        count = dev->size - *f_pos;

    if( !dev->user_buff )   // user buffer is empty/uninitialize
        goto out;

    if( copy_to_user(buf, dev->user_buff + *f_pos, count) ) // (to, from, number)
    {
        retval = -EFAULT;
        goto out;
    }
    *f_pos += count;
    retval = count;

out:
    up(&dev->sem); // V function
    return retval;

}

//*********************************************************
//  write data to device
//*********************************************************
ssize_t scull_write(struct file *file, const char __user *buf, size_t count, loff_t *f_pos)
{
    struct scull_device *dev = file->private_data;
    ssize_t retval = -ENOMEM;

    if (down_interruptible(&dev->sem)) // P function
        return -ERESTARTSYS;

    if(!dev->user_buff)
    {
        dev->user_buff = kmalloc(USER_BUFF_SIZE, GFP_KERNEL);
        if(!dev->user_buff) // failed to allocate bytes from memory
            goto out;
        memset(dev->user_buff, 0, USER_BUFF_SIZE);  // initialize buffer
    }

    if( *f_pos >= USER_BUFF_SIZE )
        goto out;

    if( *f_pos + count > USER_BUFF_SIZE )   // write limited up to USER_BUFF_SIZE
        count = USER_BUFF_SIZE - *f_pos;

    if( copy_from_user( dev->user_buff + *f_pos, buf, count))   // (to, from, number)
    {
        retval = -EFAULT;
        goto out;
    }
    *f_pos += count;
    retval = count;

    if(dev->size < *f_pos)  // update the size
        dev->size = *f_pos;

out:
    up(&dev->sem); // V function
    return retval;
}

//*********************************************************
//  define file operations
//*********************************************************
struct file_operations scull_fops = {
    .owner =    THIS_MODULE,
    .open  =    scull_open,
    .release =  scull_release,
    .read =    	scull_read,
    .write =    scull_write,
};

//*********************************************************
//  char device registration
//*********************************************************
static void scull_setup_cdev(struct scull_device *dev)
{
    int err, devno = MKDEV(scull_major, 0);

    cdev_init(&dev->chrdev, &scull_fops);
    dev->chrdev.owner = THIS_MODULE;
    dev->chrdev.ops   = &scull_fops;

    err = cdev_add(&dev->chrdev, devno, 1 );
    if(err)
        printk(KERN_WARNING "Error %d adding scull0", err);
}

//*********************************************************
//  clean up module
//*********************************************************
void __exit scull_cleanup(void)
{
    dev_t devno = MKDEV(scull_major, 0);
    if(scull_dev)
    {
        scull_trim(scull_dev);
        cdev_del(&scull_dev->chrdev);
        kfree(scull_dev);
    }
    unregister_chrdev_region(devno, 1);
}

//*********************************************************
//  initialize
//*********************************************************
static int __init scull_init(void)
{	
    int ret;
    dev_t devt = 0;

    /* Figure out our device number. */
    if (scull_major)
    {
        devt = MKDEV(scull_major, 0);
        ret = register_chrdev_region(devt, 1, SCULL_NAME);
    }
    else
    {
        ret = alloc_chrdev_region(&devt, 0, 1, SCULL_NAME);
        scull_major = MAJOR(devt);
    }
    if ( ret<0 )
    {
        printk(KERN_WARNING "SCULL_MODULE_NAME: unable to get major %d\n", scull_major);
        return ret;
    }

    scull_dev = kmalloc( sizeof(struct scull_device), GFP_KERNEL);  // allocate size
    if( !scull_dev )
    {
        ret = -ENOMEM;
        goto fail;
    }
    memset( scull_dev, 0, sizeof(struct scull_device) );
    sema_init( &scull_dev->sem, 1);

    scull_setup_cdev(scull_dev);

    return 0;

fail:
    scull_cleanup();
    return ret;

}

module_init(scull_init);
module_exit(scull_cleanup);

MODULE_AUTHOR("Joe");
MODULE_DESCRIPTION("SCULL Driver ");
MODULE_LICENSE("GPL");
	  
