
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
#include <linux/interrupt.h>
#include <asm-arm/arch/hardware.h>


	



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
int second = 1;  // to control the frequency
/*****************new variable***************/
int sh_timepoint,ud_timepoint;         // the timepoint when button 1/0 is pressed. 
int human_miss; // time difference for button pressing
int brt = 0;	// brightness level representation(0-high 1-medium 2-low)
//int control_flag = 1; 


/******************************************/

static char *msg;
static int msg_len;
static int tbuf_len = 0;

char *direction;
char *status;
char *brightness;


	
irqreturn_t gpio_irq_sh(int irq, void *dev_id, struct pt_regs *regs)  //interrupt for start/hold button
{
	//no debounce, active for both edges
	//printk("Button IRQ\n");
	sh_timepoint = jiffies_to_msecs(jiffies); // get the timepoint when start/hold button is pressed.
	/*******get the button pressing time difference******/	
	if(sh_timepoint >= ud_timepoint)
		human_miss = sh_timepoint - ud_timepoint;
	else
		human_miss = ud_timepoint - sh_timepoint;
	
	/******************************************************/
	if(human_miss <= 200)        // counted as pressing two buttons simultaneously
	{
		counter = 15;
	}
	else
	{
		if(operate_status == 0)
			operate_status = 1;
		else
			operate_status = 0; 
	}


	return IRQ_HANDLED;
}

irqreturn_t gpio_irq_ud(int irq, void *dev_id, struct pt_regs *regs)   //interrupt for up/down button
{
	//no debounce, active for both edges
	//printk("Button IRQ\n");
	ud_timepoint = jiffies_to_msecs(jiffies);

	/*******get the button pressing time difference******/	
	if(sh_timepoint >= ud_timepoint)
		human_miss = sh_timepoint - ud_timepoint;
	else
		human_miss = ud_timepoint - sh_timepoint;
	
	/******************************************************/
	if(human_miss <= 200)        // counted as pressing two buttons simultaneously
	{
		counter = 15;
	}
	else{
		if(count_mode == 0)
			count_mode = 1;
		else
			count_mode = 0; 
	}

	
	return IRQ_HANDLED;
}

irqreturn_t gpio_irq_br(int irq, void *dev_id, struct pt_regs *regs)   //interrupt for brightness 
{
	if(brt == 2)
		brt = 0; 
	else 
		brt++;
	
	//if(control_flag = 1){
	if(counter%2 == 1){
	switch(brt){
		case 0:
			PWM_PWDUTY0 = 0xc7;
//			printk(KERN_ALERT "high\n");
			break;
		case 1: 
			PWM_PWDUTY0 = 0x30;
//			printk(KERN_ALERT "medium\n");
			break;
		case 2:
			PWM_PWDUTY0 = 0x5;
//			printk(KERN_ALERT "low\n");
			break;
		default:
			PWM_PWDUTY0 = 0;
		}
	}
	
	return IRQ_HANDLED;
}



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
	
	// define interrupts and its according buttons
	pxa_gpio_mode(113 | GPIO_IN);	
	pxa_gpio_mode(101 | GPIO_IN);
	pxa_gpio_mode(117 | GPIO_IN);

	int irq_sh = IRQ_GPIO(113);
	printk("irq_sh is %d\n", irq_sh);
	int irq_ud = IRQ_GPIO(101);
	printk("irq_ud is %d\n", irq_ud);
	int irq_br = IRQ_GPIO(117);
	printk("irq_br is %d\n",irq_br);
	if (request_irq(irq_sh, &gpio_irq_sh, SA_INTERRUPT | SA_TRIGGER_RISING,
				"mygpio_sh", NULL) != 0 ) {
                printk ( "irq not acquired \n" );
                return -1;
        }else{
                printk ( "irq_sh %d acquired successfully \n", irq_sh );
	}

	if (request_irq(irq_ud, &gpio_irq_ud, SA_INTERRUPT | SA_TRIGGER_RISING,
				"mygpio1_ud", NULL) != 0 ) {
                printk ( "irq_sh not acquired \n" );
                return -1;
        }else{
                printk ( "irq_ud %d acquired successfully \n", irq_ud );
	}
		
	if (request_irq(irq_br, &gpio_irq_br, SA_INTERRUPT | SA_TRIGGER_RISING,
				"mygpio1_br", NULL) != 0 ) {
                printk ( "irq_br not acquired \n" );
                return -1;
        }else{
                printk ( "irq_br %d acquired successfully \n", irq_br );
	}
	


	// define and initialize the ports of input and output
	
	/* definition*/
	gpio_direction_input(113);
	gpio_direction_input(101);
	gpio_direction_input(117);
	
	gpio_direction_output(28,1);
	gpio_direction_output(29,1);
	gpio_direction_output(30,1);
	gpio_direction_output(16,1);
	
	/*initialize*/
	gpio_set_value(28,1);
	gpio_set_value(29,1);
	gpio_set_value(30,1);
	//gpio_set_value(16,1);
	 
	/* initialize pwm stuff*/
	pxa_set_cken(CKEN0_PWM0,1);  // set the clock
	pxa_gpio_mode(GPIO16_PWM0_MD);
	PWM_PERVAL0 = 0xc8;
	PWM_PWDUTY0 = 0xfff;
	

	sh_timepoint = jiffies_to_msecs(jiffies); // get the timepoint when start/hold button is pressed.
	ud_timepoint = jiffies_to_msecs(jiffies);

	// setup a timer 
	setup_timer(mytimer, timer_handler, 0);      // setup new timer
	mytimer->expires=jiffies + (second*HZ);
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

	/*turn off the leds*/
	gpio_set_value(29,0);
	gpio_set_value(30,0);
	gpio_set_value(16,0);
	gpio_set_value(28,0);

	// Free interrupt numbers
	free_irq(IRQ_GPIO(101), NULL);
	free_irq(IRQ_GPIO(113), NULL);
	free_irq(IRQ_GPIO(117), NULL);

	PWM_PWDUTY0 = 0;


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
	//char point5[10];
	//printk(KERN_ALERT "It is reading\n");
	//point5[0] = (char)second;
	//point5[1] = '.';
	//point5[2] = '5';
	//point5[3] = '\0';
	tbuf = kmalloc(5024*sizeof(char), GFP_KERNEL);
	memset(tbuf, 0, 5024);
	
	if(direction)
		kfree(direction);
	if(status)
		kfree(status);
	if(brightness)
		kfree(brightness);
	if(tbuf)
		kfree(tbuf);


	direction = kmalloc(sizeof(char)*strlen(tbuf)+1, GFP_KERNEL);
	memset(direction, 0, strlen(direction));

	status = kmalloc(sizeof(char)*strlen(tbuf)+1, GFP_KERNEL);
	memset(status, 0, strlen(status));

	brightness = kmalloc(sizeof(char)*strlen(tbuf)+1, GFP_KERNEL);
	memset(brightness, 0, strlen(brightness));

	
	if(operate_status == 1)		
		strcpy(status,"running");
	else
		strcpy(status,"stopped");

	if(count_mode == 1)
		strcpy(direction,"down");
	else
		strcpy(direction,"up");
	if(brt == 0)
		strcpy(brightness, "high");
	else if (brt == 1)
		strcpy(brightness, "medium");
	else 
		strcpy(brightness, "low");
		

	
	
	//if(second%2 == 0)   {// integer period
	sprintf(tbuf,"%d %d %s %s %s\n", counter,second,direction,status,brightness);
//	printk(KERN_ALERT "%d %d %s %s %s\n", counter,second,direction,status,brightness);
	//else {
	//sprintf(tbuf,"Counter value: %d\n Counter period in seconds:%s\n Counter direction:%s\n Counter state:%s\n", counter,point5,direction,status);
	//}

	//count = strlen(tbuf);
	
	//count = sizeof(tbuf);
	//tbuf_len = strlen(tbuf);
	//*f_pos += tbuf_len; 

	/************************* This part needs to be modified *******/
	*f_pos = 0;
	count = 50*strlen(tbuf);
	
	if (copy_to_user(buf, tbuf, count))
			return -EFAULT;

	*f_pos += count;
	count = *f_pos;

	return count;


	/***********************************************************/
	


//	if(tbuf)       // kfree tbuf 
//      kfree(tbuf);

	
//	return tbuf_len; 

}



/************* ways of using write***********


c1,c2,c3  ----- speed level of counting 
s(0-f)  ----- set the counter value to 0~15 
b0,b1,b2 ----- control the brightness of the light by pwm


cat "c1" > /dev/mygpio


cat "sa" > /dev/mygpio

cat "b0" > /dev/mygpio

	
**********************************************/
static ssize_t mygpio_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)        
{
	
	char opcode, s_value;
	int i_value;
	int asc_value;
	printk(KERN_ALERT "It is writing\n");
	msg = kmalloc(128*sizeof(char), GFP_KERNEL);
	memset(msg, 0, 128);
	
	i_value = 0;
	if (copy_from_user(msg + *f_pos, buf, count))     // get the instruction and store it into msg
		return -EFAULT;
	
	opcode = msg[0];
	printk(KERN_ALERT "1:opcode is %c\n", opcode);
	s_value  = msg[1];	
	asc_value = (int)s_value;
	printk(KERN_ALERT "2:asc_value is %d\n", asc_value);

	if((s_value <= '9') && (s_value >= '0'))   // ASCII convert '1' to 1 , 1~9
		i_value = asc_value - 48;
	else if((s_value <= 'f') && (s_value >= 'a'))    // ASII convert 'a' to a , a~f
		i_value = asc_value - 87;
	
	if(opcode == 'c'){   				//f1~f8
		if((i_value >= 1) && (i_value <= 3)){
			second = i_value;
		printk(KERN_ALERT "3 second is %d\n", second);
		}
	}
	else if(opcode == 's'){
		if((i_value >= 1) && (i_value <= 15)){       //v1~vf
			if(operate_status == 0)
				counter = i_value;
			else
				counter = i_value -1; 
			//mod_timer(mytimer,jiffies + (second * HZ));
		}
		printk(KERN_ALERT "4 counter is %d\n", counter);
	}
	else if(opcode == 'b'){
		if((i_value >= 0) && (i_value <= 2)){
			brt = i_value;	
			switch(brt){
		case 0:
			PWM_PWDUTY0 = 0xc7;
			printk(KERN_ALERT
			"high\n");
			break;
		case 1: 
			PWM_PWDUTY0 = 0x30;
			printk(KERN_ALERT
			"medium\n");
			break;
		case 2:
			PWM_PWDUTY0 = 0x5;
			printk(KERN_ALERT
			"low\n");
			break;
		default:
			PWM_PWDUTY0 = 0;
		}
			
		printk(KERN_ALERT "5 brightness is %d\n", brt);
		}
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
	mod_timer(mytimer,jiffies + (second * HZ));
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
			//pxa_gpio_set_value(16,1);
			if(brt == 0)
				PWM_PWDUTY0 = 0xc7;
			else if(brt == 1)
				PWM_PWDUTY0 = 0x30;
			else
				PWM_PWDUTY0 = 0x5;
			break;
			
		case 2:
			pxa_gpio_set_value(28,0);
			pxa_gpio_set_value(29,0);
			pxa_gpio_set_value(30,1);
			//pxa_gpio_set_value(16,0);
			PWM_PWDUTY0 = 0;
			break;
		
		case 3:
			pxa_gpio_set_value(28,0);
			pxa_gpio_set_value(29,0);
			pxa_gpio_set_value(30,1);
			//pxa_gpio_set_value(16,1);
			if(brt == 0)
				PWM_PWDUTY0 = 0xc7;
			else if(brt == 1)
				PWM_PWDUTY0 = 0x30;
			else
				PWM_PWDUTY0 = 0x5;
			break;
		
		case 4:
			pxa_gpio_set_value(28,0);
			pxa_gpio_set_value(29,1);
			pxa_gpio_set_value(30,0);
			//pxa_gpio_set_value(16,0);
			PWM_PWDUTY0 = 0;
			break;
			
		case 5:
			pxa_gpio_set_value(28,0);
			pxa_gpio_set_value(29,1);
			pxa_gpio_set_value(30,0);
			//pxa_gpio_set_value(16,1);
			if(brt == 0)
				PWM_PWDUTY0 = 0xc7;
			else if(brt == 1)
				PWM_PWDUTY0 = 0x30;
			else
				PWM_PWDUTY0 = 0x5;
			break;
		
		case 6:
			pxa_gpio_set_value(28,0);
			pxa_gpio_set_value(29,1);
			pxa_gpio_set_value(30,1);
			//pxa_gpio_set_value(16,0);
			PWM_PWDUTY0 = 0;
			break;
			
		case 7:
			pxa_gpio_set_value(28,0);
			pxa_gpio_set_value(29,1);
			pxa_gpio_set_value(30,1);
			//pxa_gpio_set_value(16,1);
			if(brt == 0)
				PWM_PWDUTY0 = 0xc7;
			else if(brt == 1)
				PWM_PWDUTY0 = 0x30;
			else
				PWM_PWDUTY0 = 0x5;
			break;
			
		case 8:
			pxa_gpio_set_value(28,1);
			pxa_gpio_set_value(29,0);
			pxa_gpio_set_value(30,0);
			//pxa_gpio_set_value(16,0);
			PWM_PWDUTY0 = 0;
			break;
			
			
		case 9:
			pxa_gpio_set_value(28,1);
			pxa_gpio_set_value(29,0);
			pxa_gpio_set_value(30,0);
			//pxa_gpio_set_value(16,1);
			if(brt == 0)
				PWM_PWDUTY0 = 0xc7;
			else if(brt == 1)
				PWM_PWDUTY0 = 0x30;
			else
				PWM_PWDUTY0 = 0x5;
			break;
			
		case 10:
			pxa_gpio_set_value(28,1);
			pxa_gpio_set_value(29,0);
			pxa_gpio_set_value(30,1);
			//pxa_gpio_set_value(16,0);
			PWM_PWDUTY0 = 0;
			break;
			
		case 11:
			pxa_gpio_set_value(28,1);
			pxa_gpio_set_value(29,0);
			pxa_gpio_set_value(30,1);
			//pxa_gpio_set_value(16,1);
			if(brt == 0)
				PWM_PWDUTY0 = 0xc7;
			else if(brt == 1)
				PWM_PWDUTY0 = 0x30;
			else
				PWM_PWDUTY0 = 0x5;
			break;
			
		case 12:
			pxa_gpio_set_value(28,1);
			pxa_gpio_set_value(29,1);
			pxa_gpio_set_value(30,0);
			//pxa_gpio_set_value(16,0);
			PWM_PWDUTY0 = 0;
			break;
			
		case 13:
			pxa_gpio_set_value(28,1);
			pxa_gpio_set_value(29,1);
			pxa_gpio_set_value(30,0);
			//pxa_gpio_set_value(16,1);
			if(brt == 0)
				PWM_PWDUTY0 = 0xc7;
			else if(brt == 1)
				PWM_PWDUTY0 = 0x30;
			else
				PWM_PWDUTY0 = 0x5;
			break;
			
		case 14:
			pxa_gpio_set_value(28,1);
			pxa_gpio_set_value(29,1);
			pxa_gpio_set_value(30,1);
			//pxa_gpio_set_value(16,0);
			PWM_PWDUTY0 = 0;
			break;
			
		case 15:
			pxa_gpio_set_value(28,1);
			pxa_gpio_set_value(29,1);
			pxa_gpio_set_value(30,1);
			//pxa_gpio_set_value(16,1);
			if(brt == 0)
				PWM_PWDUTY0 = 0xc7;
			else if(brt == 1)
				PWM_PWDUTY0 = 0x30;
			else
				PWM_PWDUTY0 = 0x5;
			break;
			
		default:
			pxa_gpio_set_value(28,1);
			pxa_gpio_set_value(29,1);
			pxa_gpio_set_value(30,1);
			//pxa_gpio_set_value(16,1);
			PWM_PWDUTY0 = 0;
		
	}
		
			
}



