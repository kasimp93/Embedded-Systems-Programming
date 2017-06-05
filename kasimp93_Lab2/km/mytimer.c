/***************************************/
/* EC-535 Lab-2 */
/* Muhammad Kasim Patel */
/* U75595108 */
/* Kernel Module */
/* mytimer.c */
/***************************************/

/*Necessary includes for device drivers */
#include <linux/init.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/jiffies.h>
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

MODULE_LICENSE("Dual BSD/GPL");


struct timername 
{
unsigned long total_time;
char mytimer_name[128];
int flag;
}tn[20];

/* Declaration of memory.c functions */
static int mytimer_open(struct inode *inode, struct file *filp);
static int mytimer_release(struct inode *inode, struct file *filp);
static ssize_t mytimer_read(struct file *filp,char *buf, size_t count, loff_t *f_pos);
static ssize_t mytimer_write(struct file *filp,const char *buf, size_t count, loff_t *f_pos);
static void mytimer_exit(void);
static int mytimer_init(void);
static struct timer_list my_timer[20];
void  my_timer_callback(unsigned long data);

/* Structure that declares the usual file */
/* access functions */
struct file_operations mytimer_fops = {
	read: mytimer_read,
	write: mytimer_write,
	open: mytimer_open,
	release: mytimer_release
};

/* Declaration of the init and exit functions */

module_init(mytimer_init);
module_exit(mytimer_exit);
int i=0;
int T=20;

static unsigned capacity = 128;
static int mytimer_major = 61;
static char *mytimer_buffer;
static int mytimer_len;

void my_timer_callback(unsigned long data)
{
printk("%s\n",tn[data].mytimer_name);
tn[data].flag = 0;
del_timer(&my_timer[data]);
}

/*initializing timer module*/
static int mytimer_init(void)
{
	int result;

	/* Registering device */
	result = register_chrdev(mytimer_major, "mytimer", &mytimer_fops);
	if (result < 0)
	{
		printk(KERN_ALERT
			"mytimer: cannot obtain major number %d\n", mytimer_major);
		return result;
	}

	/* Allocating mytimer for the buffer */
	mytimer_buffer = kmalloc(capacity, GFP_KERNEL); 
	if (!mytimer_buffer)
	{ 
		printk(KERN_ALERT "Insufficient kernel memory\n"); 
		result = -ENOMEM;
		goto fail; 
	} 
	memset(mytimer_buffer, 0, capacity);
	mytimer_len = 0;

	printk(KERN_ALERT "Inserting mytimer module\n"); 
	return 0;

fail: 
	mytimer_exit(); 
	return result;
}


static void mytimer_exit(void)
{
	
	unregister_chrdev(mytimer_major, "mytimer");

	if (mytimer_buffer)
	{
		kfree(mytimer_buffer);
	}
}



static int mytimer_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int mytimer_release(struct inode *inode, struct file *filp)
{	
	return 0;
}

static ssize_t mytimer_read(struct file *filp, char *buf,size_t count, loff_t *f_pos)
{ 
	int mytimer_elapsed;
	char temp_buf[128];
	int j;
	int x=0;
	char t[20];
	char temp[1280];	
	if (*f_pos >= mytimer_len)
        {
                return 0;
        }
        if (count > mytimer_len - *f_pos)
                count = mytimer_len - *f_pos;

	for(j=0;j<T;j++)
        {
                if(tn[j].flag == 0)
		{
			x=x+1;
			continue;
		}
		else
		{
			mytimer_elapsed =((tn[j].total_time - jiffies)/HZ);
                        strcpy(temp_buf,tn[j].mytimer_name);
                        strcat(temp_buf," ");
                        sprintf(t,"%d ",mytimer_elapsed);
                        strcat(temp_buf,t);
                        strcat(temp,temp_buf);
                        strcat(temp,"\n");
		}
	}
	if(T<i && x!=0)
	{
		for(j=T;j<T+x;j++)
		{
			if(tn[j].flag == 1)
			{
				mytimer_elapsed =((tn[j].total_time - jiffies)/HZ);
                        	strcpy(temp_buf,tn[j].mytimer_name);
	                        strcat(temp_buf," ");
        	                sprintf(t,"%d ",mytimer_elapsed);
                	        strcat(temp_buf,t);
	                        strcat(temp,temp_buf);
        	                strcat(temp,"\n");
			}
		}
	}
		
		count = strlen(temp); 
               	if(copy_to_user(buf,temp + *f_pos, count))
                {
                       return -EFAULT;
                }
	memset(temp,0,1280);
	T = i;
	*f_pos += count;
        return count;
}	

static ssize_t mytimer_write(struct file *filp,const char *buf,size_t count,loff_t *f_pos)
{
	int temp,m;
	int flag=0;
	char tbuf[128];
	unsigned long time_jiffies;
	int time_sec;
	char *name;
	char *tbptr = tbuf;
	if (*f_pos >= capacity)
	{
		return -ENOSPC;
	}

	if (copy_from_user(mytimer_buffer + *f_pos, buf, count))
	{
		return -EFAULT;
	}
	for (temp = *f_pos; temp < count + *f_pos; temp++)
		tbptr += sprintf(tbptr, "%c", mytimer_buffer[temp]);
	
	time_sec = (int)simple_strtoul(tbuf,&name,0);
if(*name == '\0')
{
	T = time_sec;
}
else
{
	for(m=0;m<=i;m++)
        {
                if(strcmp(name,tn[m].mytimer_name)==0 && tn[m].flag==1)
                {
                        del_timer(&my_timer[m]);
                        strcpy(tn[m].mytimer_name,name);
                        setup_timer(&my_timer[m],my_timer_callback,m);
                        time_jiffies = jiffies + time_sec*HZ;
			tn[m].flag = 1;
                        tn[m].total_time = time_jiffies;
                        mod_timer(&my_timer[m],time_jiffies);
                        printk("Timer %s was updated!\n",tn[m].mytimer_name);
			flag=1;
                }
	}
	if(flag == 1)
	{
		flag = 0;
	}	
	else
        {
		strcpy(tn[i].mytimer_name,name);
	        setup_timer(&my_timer[i],my_timer_callback,i);
       		tn[i].flag = 1;
        	time_jiffies = jiffies + time_sec*HZ;
        	tn[i].total_time = time_jiffies;
	        mod_timer(&my_timer[i],time_jiffies);
		i++;
     	}
}
	*f_pos += count;
	mytimer_len = *f_pos;
	return count;
}
