
/* Necessary includes for device drivers */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */
#include <asm/system.h> /* cli(), *_flags */
#include <asm/uaccess.h> /* copy_from/to_user */
#include <linux/timer.h> /* timer support */
#include <linux/string.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>

#include <linux/jiffies.h>
#include <linux/vmalloc.h>
#include <asm/arch/pxa-regs.h>
#include <asm/hardware.h>
#include <asm/uaccess.h>
#include <linux/ctype.h>
#include <asm/arch/gpio.h>


	



MODULE_LICENSE("Dual BSD/GPL");

static int mygpio_open(struct inode *inode, struct file *filp);
static int mygpio_release(struct inode *inode, struct file *filp);
static ssize_t mygpio_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
static ssize_t mygpio_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
static int mygpio_init(void);
static void mygpio_exit(void);
static void timer_handler(unsigned long data);
static void led_print(int counter);



/*
 * The file operations for the pipe device
 * (some are overlayed with bare scull)
 */
struct file_operations mygpio_fops = {
	read	: mygpio_read,
	write	: mygpio_write,
	open	: mygpio_open,
	release : mygpio_release
};

/* Declaration of the init and exit functions */
module_init(mygpio_init);
module_exit(mygpio_exit);

static int mygpio_major = 61;
static struct timer_list *mytimer;

//global variable define 
int counter = 15;
int count_mode = 0;  //up counter when 0 and down counter when 1
int operate_status = 0; // released(hold the current counter value) when 0 and pressed(count) when 1;    
int second = 2;  // to control the frequency

static char *msg;
static int msg_len;

char *direction;
char *status;


	





static int mygpio_init(void){

	int result = 0;
	/* Registering device */
	result = register_chrdev(mygpio_major, "mygpio", &mygpio_fops);


	if (result < 0){
		printk(KERN_ALERT
			"mygpio: cannot obtain major number %d\n", mygpio_major);
		return result;
	}

	/* Allocating buffers */
	mytimer = (struct timer_list *) kmalloc(sizeof(struct timer_list *), GFP_KERNEL);

	/* Check if all right */
	if (!mytimer)
	{
		printk(KERN_ALERT "Insufficient kernel memory\n");
		result = -ENOMEM;
		goto fail;
	}
	
	// define and initialize the ports of input and output
	
	/* definition*/
	gpio_direction_input(17);
	gpio_direction_input(101);
	
	gpio_direction_output(28,1);
	gpio_direction_output(29,1);
	gpio_direction_output(30,1);
	gpio_direction_output(31,1);
	
	/*initialize*/
	gpio_set_value(29,1);
	gpio_set_value(30,1);
	gpio_set_value(31,1);
	gpio_set_value(32,1);

	
	// setup a timer 
	setup_timer(mytimer, timer_handler, 0);      // setup new timer
	mytimer->expires=jiffies + (second/2*HZ);
	add_timer(mytimer);
	
	
	
	//printk("fasync_example loaded.\n");
	return 0;
	
	

fail:
	mygpio_exit();
	return result;
	
}

static void mygpio_exit(void)
{
	/* Freeing the major number */
	unregister_chrdev(mygpio_major, "mytimer");
	if (mytimer)
		kfree(mytimer);
	
	del_timer(mytimer);
	
	printk(KERN_ALERT "Removing mygpio module\n");
	
	
	

}


static int mygpio_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int mygpio_release(struct inode *inode, struct file *filp)
{
	return 0;
}



static ssize_t mygpio_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{

	char *tbuf;  // to store all the information 
	char point5[10];
	//printk(KERN_ALERT "It is reading\n");
	sprintf(point5, "%d.5\n",second);
//	point5[0] = (char)second;
//	point5[1] = 'p';
//	point5[2] = '5';
//	point5[3] = '\0';
	
	tbuf = kmalloc(5024*sizeof(char), GFP_KERNEL);
	memset(tbuf, 0, 5024);
	
	
	if(direction)
		kfree(direction);
	if(status)
		kfree(status);
	if(tbuf)
		kfree(tbuf);


	direction = kmalloc(sizeof(char)*strlen(tbuf)+1, GFP_KERNEL);
	memset(direction, 0, strlen(direction));

	status = kmalloc(sizeof(char)*strlen(tbuf)+1, GFP_KERNEL);
	memset(status, 0, strlen(status));

	
	if(operate_status == 1)		
		strcpy(status,"running");
	else
		strcpy(status,"stopped");

	if(count_mode == 1)
		strcpy(direction,"down");
	else
		strcpy(direction,"up");

	
	
	if(second%2 == 0)   {// integer period
	sprintf(tbuf,"Counter value: %d\n Counter period in seconds:%d\n Counter direction:%s\n Counter state:%s\n", counter,(second / 2), direction,status);}
	else {
	sprintf(tbuf,"Counter value: %d\n Counter period in seconds:%s\n Counter direction:%s\n Counter state:%s\n", counter,point5,direction,status);
	}

	count = strlen(tbuf);
	
	//count = sizeof(tbuf);
	
	if (copy_to_user(buf, tbuf, count))
			return -EFAULT;

//	if(tbuf)       // kfree tbuf 
//      kfree(tbuf);
	*f_pos += count;
	count = *f_pos;

	return count;

//	*f_pos += count; 
//	return count; 

}

static ssize_t mygpio_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
	
	char opcode, s_value;
	int i_value;
	int asc_value;
	printk(KERN_ALERT "It is writing\n");
	//char *tprint;
	msg = kmalloc(128*sizeof(char), GFP_KERNEL);
	memset(msg, 0, 128);

	if (copy_from_user(msg + *f_pos, buf, count))     // get the instruction and store it into msg
		return -EFAULT;
	
	opcode = msg[0];
	printk(KERN_ALERT "1:opcode is %c\n", opcode);
	s_value  = msg[1];	
	asc_value = (int)s_value;
	printk(KERN_ALERT "2:asc_value is %d\n", asc_value);

	if((s_value <= '9') && (s_value >= '1'))   // ASCII convert '1' to 1 , 1~9
		i_value = asc_value - 48;
	else if((s_value <= 'f') && (s_value >= 'a'))    // ASII convert 'a' to a , a~f
		i_value = asc_value - 87;
	
	if(opcode == 'f'){   				//f1~f8
		if((i_value >= 1) && (i_value <= 8)){
			second = i_value;
		printk(KERN_ALERT "3 second is %d\n", second);
		}
	}
	else if(opcode == 'v'){
		if((i_value >= 1) && (i_value <= 15)){       //v1~vf
			counter = i_value;
		}
		printk(KERN_ALERT "4 counter is %d\n", counter);
	}
	
	
	*f_pos += count;
	msg_len = *f_pos;

	return count;
}

void timer_handler(unsigned long data)
{
	//operate_status = pxa_gpio_get_value(17);
	//operate_status = gpio_get_value(17);
	//operate_status = 1;
	//count_mode = pxa_gpio_get_value(101);
	//count_mode = pxa_gpio_get_value(101);
	//count_mode = gpio_get_value(101);
	if(pxa_gpio_get_value(17))
		operate_status = 1;
	else
		operate_status = 0;
	if(pxa_gpio_get_value(101))
		count_mode = 1;
	else
		count_mode = 0;
	
	if((operate_status == 1) && (count_mode == 0)){       // hold the button and up count case 
		if(counter == 15)
			counter = 1;
		else 
			counter ++;
	}
	else if((operate_status == 1) && (count_mode ==1)){  // hold the button and down count case 
		if(counter == 1)
			counter = 15;
		else
			counter -- ;
	}
	mod_timer(mytimer,jiffies + (second * HZ /2));
	//setup_timer(mytimer, timer_handler, 0);      // setup new timer
	/********not yet */
	//mytimer->expires=jiffies + (second/2*HZ);
	/********* not yet */
	//add_timer(mytimer);
	
	led_print(counter);
	


}

void led_print(int counter)
{
	
	
	
	switch(counter){
		case 1:
			pxa_gpio_set_value(28,0);
			pxa_gpio_set_value(29,0);
			pxa_gpio_set_value(30,0);
			pxa_gpio_set_value(31,1);
			break;
			
		case 2:
			pxa_gpio_set_value(28,0);
			pxa_gpio_set_value(29,0);
			pxa_gpio_set_value(30,1);
			pxa_gpio_set_value(31,0);
			break;
		
		case 3:
			pxa_gpio_set_value(28,0);
			pxa_gpio_set_value(29,0);
			pxa_gpio_set_value(30,1);
			pxa_gpio_set_value(31,1);
			break;
		
		case 4:
			pxa_gpio_set_value(28,0);
			pxa_gpio_set_value(29,1);
			pxa_gpio_set_value(30,0);
			pxa_gpio_set_value(31,0);
			break;
			
		case 5:
			pxa_gpio_set_value(28,0);
			pxa_gpio_set_value(29,1);
			pxa_gpio_set_value(30,0);
			pxa_gpio_set_value(31,1);
			break;
		
		case 6:
			pxa_gpio_set_value(28,0);
			pxa_gpio_set_value(29,1);
			pxa_gpio_set_value(30,1);
			pxa_gpio_set_value(31,0);
			break;
			
		case 7:
			pxa_gpio_set_value(28,0);
			pxa_gpio_set_value(29,1);
			pxa_gpio_set_value(30,1);
			pxa_gpio_set_value(31,1);
			break;
			
		case 8:
			pxa_gpio_set_value(28,1);
			pxa_gpio_set_value(29,0);
			pxa_gpio_set_value(30,0);
			pxa_gpio_set_value(31,0);
			break;
			
			
		case 9:
			pxa_gpio_set_value(28,1);
			pxa_gpio_set_value(29,0);
			pxa_gpio_set_value(30,0);
			pxa_gpio_set_value(31,1);
			break;
			
		case 10:
			pxa_gpio_set_value(28,1);
			pxa_gpio_set_value(29,0);
			pxa_gpio_set_value(30,1);
			pxa_gpio_set_value(31,0);
			break;
			
		case 11:
			pxa_gpio_set_value(28,1);
			pxa_gpio_set_value(29,0);
			pxa_gpio_set_value(30,1);
			pxa_gpio_set_value(31,1);
			break;
			
		case 12:
			pxa_gpio_set_value(28,1);
			pxa_gpio_set_value(29,1);
			pxa_gpio_set_value(30,0);
			pxa_gpio_set_value(31,0);
			break;
			
		case 13:
			pxa_gpio_set_value(28,1);
			pxa_gpio_set_value(29,1);
			pxa_gpio_set_value(30,0);
			pxa_gpio_set_value(31,1);
			break;
			
		case 14:
			pxa_gpio_set_value(28,1);
			pxa_gpio_set_value(29,1);
			pxa_gpio_set_value(30,1);
			pxa_gpio_set_value(31,0);
			break;
			
		case 15:
			pxa_gpio_set_value(28,1);
			pxa_gpio_set_value(29,1);
			pxa_gpio_set_value(30,1);
			pxa_gpio_set_value(31,1);
			break;
			
		default:
			pxa_gpio_set_value(28,1);
			pxa_gpio_set_value(29,1);
			pxa_gpio_set_value(30,1);
			pxa_gpio_set_value(31,1);
		
	}
		
			
}



