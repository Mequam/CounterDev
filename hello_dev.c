/*  
 *  a very crappy kernel driver
 */
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/fs.h>		//needed for file_operations
#include <asm/uaccess.h>


//module documentation
#define AUTHOR "Mequam"
#define DEVNAME "baddev"
#define DESCRIPT "A simple counter device to save a number accross program execution"

MODULE_LICEANSE("GPL");
MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPT);

//this represents the number of minor devices that the driver will be able to support
//more minor devices means more memory >_>
#define DEVICE_NUM 4

static unsigned int Major = 0;

//these are the variables running the devices
static bool open[DEVICE_NUM]; 
static int counters[DEVICE_NUM];

static int dev_open(struct inode * i,struct file * f)
{
	try_module_get(THIS_MODULE); // tell the kernel that we have a dev open
	return 0;// we were sucessful
}
static int dev_close(struct inode * i,struct file * f)
{
	module_put(THIS_MODULE); // decriment the number of open devices in the kernel
	return 0; // we were sucessful so return 0
}
static ssize_t dev_write(struct file* f, const char * c,size_t length,loff_t * offset)
{
	char bc = 'A';
	get_user(bc,c);//hopefully this should work?
	printk(KERN_INFO MINOR(f.inode->i_rdev));	
	return 1; // return the number of bytes that we wrote
}
static int outread = 0;
static ssize_t dev_read(struct file* f,char * buffer,size_t length,loff_t * offset)
{
	
	
	int i = 0;
	printk(KERN_INFO "recived %d as the buffer length",length);
	while (i < 20 && i < length)
	{	
		put_user(outchar,buffer++); //incriment the buffer each time
		outread++;
		i++;
	}
	if (outread >= 40)
	{
		outread = 0;
		return 0;
	}
	else
	{
		return outread;
	} //you will allways get to the end of the file
}
struct file_operations fops = 
{
	.read = dev_read,
	.write = dev_write,
	.open = dev_open,
	.release = dev_close
};
int init_module(void)
{
	
	Major = register_chrdev(Major,DEVNAME,&fops);
	printk(KERN_INFO "registerd with the major number %d",Major);
	return 0;
}

void cleanup_module(void)
{
	unregister_chrdev(Major,DEVNAME);

}
