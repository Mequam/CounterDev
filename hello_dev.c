/*  
 *  a very simple device driver that keeps track of the number of times that it is read
 */

#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/fs.h>		/*needed for file_operations*/
#include <asm/uaccess.h>  	/*needed for get_user*/
#include <cstdlib> 		/*needed to convert between strings and intagers*/

//module documentation
#define AUTHOR "Mequam"
#define DEVNAME "baddev"
#define DESCRIPT "A simple counter device to save a number accross program execution"
#define LICENSE "GPL"

MODULE_LICENSE(LICENSE);
MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPT);

//this represents the number of minor devices that the driver will be able to support
//more minor devices means more memor
#define DEVICE_NUM 4

static unsigned int Major = 0;

//these are the variables running the devices
static bool open[DEVICE_NUM]; 
static int counter[DEVICE_NUM];

static int dev_open(struct inode * i,struct file * f)
{
	int minor = MINOR(i->i_rdev);
	if (MINOR(i->i_rdev) < 0 || minor >= DEVICE_NUM)
	{
		//make sure that they cannot open a device with a minor number that the driver does not support
		printk(KERN_ERR "invalid minor number!");
		return -1;//permision denied!
	}
	//we can safely check the minor number now
	if (open[minor])
	{
		printk(KERN_ERR "device is already open!");
		return -EBUSY; // we are busy
	}
	open[minor] = true; //set the open to true	
	try_module_get(THIS_MODULE);//we can safely open the device 
	return 0;// we were sucessful
}
static int dev_close(struct inode * i,struct file * f)
{
	module_put(THIS_MODULE); // decriment the number of open devices in the kernel
	return 0; // we were sucessful so return 0
}
static ssize_t dev_write(struct file* f, const char * c,size_t length,loff_t * offset)
{
	//char bc = 'A';
	//get_user(bc,c);//hopefully this should work?
	int minor = MINOR(f->f_inode->i_rdev);	 
	printk(KERN_INFO "the minor number is %d",minor); 
	return 1; // return the number of bytes that we wrote
}

static ssize_t dev_read(struct file* f,char * buffer,size_t length,loff_t * offset)
{
	//turn the number stored in the counter into a string that we can output to userspace (we could probably just return the intager if we wanted too)
	int minor = MINOR(f->f_inode->i_rdev); // we should know that the minor number is safe becuse the open function checked it	
	char buff[20];
	snprintf(buff,20,"%d",counter[minor]);
	int read = 0;
	while (read < length && read < 20 && buff[i] != '\0')
	{
		
	}
	counter[minor] += 1; 
	printk(KERN_INFO "recived %d as the buffer length",length);
	return 0;	
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
	/*
		this is an overglorified for loop
		for (int i = 0; i < DEVICE_NUM,i++)
	*/
	int i = 0;
	while (i < DEVICE_NUM)	
	{
		//initilize the counters
		open[i] = false;
		counter[i] = 0;
		i++;
	}
	printk(KERN_INFO "sucessfully initilized counters");
	return 0;
}

void cleanup_module(void)
{
	unregister_chrdev(Major,DEVNAME);

}
