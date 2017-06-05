/***************************************/
/* EC-535 Lab-3 */
/* Muhammad Kasim Patel */
/* U75595108 */
/* Kernel Module */
/* mytimer.c */
/***************************************/

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
#include <linux/vmalloc.h>

MODULE_LICENSE("Dual BSD/GPL");

#define MAX_COOKIE_LENGTH       PAGE_SIZE


struct timername 
{
unsigned long total_time;
int time_sec;
char timer_name[128];
}tn;
int flag = 0;
int pid;	
char name_cmm[20];	
char star[128];
char update[500];
static int next_fortune;
unsigned long init_load;

/* Declaration of Functions */
static int fasync_example_fasync(int fd, struct file *filp, int mode);
static struct proc_dir_entry *proc_entry;
static int timer_open(struct inode *inode, struct file *filp);
static int timer_release(struct inode *inode, struct file *filp);
static int timer_proc_read(char *page, char **start, off_t off,int count, int *eof, void *data);
void  my_timer_handler(unsigned long data);
static ssize_t timer_write(struct file *filp,const char *buf, size_t count, loff_t *f_pos);
static ssize_t timer_read(struct file *filp, char *buf,size_t count, loff_t *f_pos);
static void timer_exit(void);
static int timer_init(void);
static struct timer_list my_timer;


/*
 * The file operations for the pipe device
 * (some are overlayed with bare scull)
 */
struct file_operations timer_fops = {	
	write: timer_write,
	open: timer_open,
	release: timer_release,
	fasync: fasync_example_fasync,
	read: timer_read
};


module_init(timer_init);
module_exit(timer_exit);


struct fasync_struct *async_queue; 

static unsigned capacity = 128;
char *t;
static int timer_major = 61;
static char *timer_buffer;
static int timer_len;
char *temp_name;



static int timer_init(void)
{
	int result;
    	proc_entry = create_proc_entry( "mytimer", 0644, NULL );
    	
	if (proc_entry == NULL) 
	{
      		return  -ENOMEM;
    	}
	else
	{
     		next_fortune = 0;
      		proc_entry->read_proc = timer_proc_read;
      		proc_entry->owner = THIS_MODULE;
    	}
	
	temp_name = (char*)vmalloc(MAX_COOKIE_LENGTH);	 
	t = kmalloc(256, GFP_KERNEL);


	
	result = register_chrdev(timer_major, "mytimer", &timer_fops);
	if (result < 0)
	{
		return result;
	}
	
	
	
	timer_buffer = kmalloc(capacity, GFP_KERNEL); 
	
		
	if (!timer_buffer)
	{ 
		result = -ENOMEM;
		goto fail; 
	} 
	init_load = jiffies;	
	memset(timer_buffer, 0, capacity);
	timer_len = 0;

	return 0;

fail: 
	timer_exit(); 
	return result;
}


static void timer_exit(void)
{
	
	unregister_chrdev(timer_major, "mytimer");
  	remove_proc_entry("mytimer", &proc_root);
	vfree(temp_name);
	
	if (timer_buffer)
	{
		kfree(timer_buffer);
	}
}


static int timer_open(struct inode *inode, struct file *filp)
{
	return 0;
}


static int timer_release(struct inode *inode, struct file *filp)
{
	fasync_example_fasync(-1, filp, 0);
	return 0;
}




static ssize_t timer_write(struct file *filp,const char *buf,size_t count,loff_t *f_pos)
{
	int temp;
	char tbuf[128];
	unsigned long time_jiffies;
	int time_sec;
	char *name;
	char *tbptr = tbuf;
	
	
	if (*f_pos >= capacity)
	{
		return -ENOSPC;
	}

	if (copy_from_user(timer_buffer + *f_pos, buf, count))
	{
		return -EFAULT;
	}

	for (temp = *f_pos; temp < count + *f_pos; temp++)
		tbptr += sprintf(tbptr, "%c", timer_buffer[temp]);
	
	time_sec = (int)simple_strtoul(tbuf,&name,0);
	tn.time_sec = time_sec;

	if(strcmp(name,tn.timer_name)==0 && (flag ==0))
	{
		strcpy(name_cmm,current->comm);
		pid = current->pid;
		strcpy(star,"update");
		sprintf(update,"Timer %s has been reset to %d seconds!",name,time_sec);
		time_jiffies = jiffies + time_sec*HZ;
		tn.total_time = time_jiffies;
        	mod_timer(&my_timer,time_jiffies);
		*f_pos += count;
		timer_len = *f_pos;
		return count;
	}
	
	if(strcmp(name,tn.timer_name)!=0 && (flag ==1))
	{
		strcpy(update,"A Timer exists already");
		strcpy(star,"exit");

		return count;
	}
	else
	{
		strcpy(name_cmm,current->comm);
		pid = current->pid;
		strcpy(tn.timer_name,name);
		setup_timer(&my_timer,my_timer_handler,0);
		time_jiffies = jiffies + time_sec*HZ;
		tn.total_time = time_jiffies;
        	mod_timer(&my_timer,time_jiffies);
		flag = 1;
		strcpy(star,"ok");
		*f_pos += count;
		timer_len = *f_pos;
		return count;
	}
}

static int timer_proc_read(char *page, char **start, off_t off,int count, int *eof, void *data)
{
	int elapsed,len;
	int loaded;
	if (off > 0)
	{
		*eof = 1;
		return 0;
	}
	loaded = (jiffies - init_load)/HZ;
    	memset(temp_name, 0, MAX_COOKIE_LENGTH );
	
	if(flag==1)
	{
		elapsed =((tn.total_time - jiffies)/HZ);
		sprintf(temp_name,"%s\n%s%d\n%s%d\n%s%s\n%s%d\n","Module Name: mytimer.ko","Time Since Module Loaded:",loaded,"Process ID:",pid,"Command Name:",name_cmm,"Time From Expiration:",elapsed);
		len = sprintf(page,"%s",temp_name);
		return len;
	}
	else
	return 0;	
}

static ssize_t timer_read(struct file *filp, char *buf,size_t count, loff_t *f_pos)
{
	int elapsed1;
	char update1[128];

if(flag == 1)
{
	if(strcmp(star,"update")==0)
	{
		strcpy(star,"ok");
		strcpy(update1,"update");
		if (copy_to_user(buf,update1,strlen(update1)))
        	{
               		return -EFAULT;
        	}
		return strlen(update1); 
	}
	else
	if(strcmp(star,"exit")==0)
	{
		strcpy(star,"ok");
                strcpy(update1,"exit");
                if (copy_to_user(buf,update1,strlen(update1)))
                {
                        return -EFAULT;
                }
                return strlen(update1);
	}
	else
	if(strcmp(star,"ok")==0)
	{
		elapsed1 =((tn.total_time - jiffies)/HZ);
		sprintf(update1,"%s %d\n",tn.timer_name,elapsed1);
		count =strlen(update1);
		if(copy_to_user(buf,update1,count))
		{
			return -EFAULT;
		}
		return count;
	}
return 0;
}
else
{
memset(update1,0,128);
return 0; 
}
}




static int fasync_example_fasync(int fd, struct file *filp, int mode) 
{
	return fasync_helper(fd, filp, mode, &async_queue);
}



void my_timer_handler(unsigned long data)
{
flag = 0;
del_timer(&my_timer);
if(async_queue)
	kill_fasync(&async_queue, SIGIO, POLL_IN);
}

