#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/device.h>
#include <linux/wait.h>
#include <linux/semaphore.h>
#define BUFF_SIZE 150
#define MAX_STRING 100
MODULE_LICENSE("Dual BSD/GPL");

struct semaphore sem;
DECLARE_WAIT_QUEUE_HEAD(redUpisa);


dev_t my_dev_id;
static struct class *my_class;
static struct device *my_device;
static struct cdev *my_cdev;


char stred[MAX_STRING];
int pos = 0;
int endRead = 0;


int stred_open(struct inode *pinode, struct file *pfile);
int stred_close(struct inode *pinode, struct file *pfile);
ssize_t stred_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset);
ssize_t stred_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset);


struct file_operations my_fops =
{
	.owner = THIS_MODULE,
	.open = stred_open,
	.read = stred_read,
	.write = stred_write,
	.release = stred_close,
};


int stred_open(struct inode *pinode, struct file *pfile)
{
		printk(KERN_INFO "Succesfully opened file\n");
		return 0;
}

int stred_close(struct inode *pinode, struct file *pfile)
{
		printk(KERN_INFO "Succesfully closed file\n");
		return 0;
}

ssize_t stred_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
	int ret;
	char buff[MAX_STRING];
	long int len;
	if (endRead){
		endRead = 0; 
		printk(KERN_INFO "Succesfully read from file\n");
		return 0;
	}
	len = scnprintf(buff,MAX_STRING , "Citamo : %s ", stred);
	ret = copy_to_user(buffer, buff, len);
	if(ret)
		return -EFAULT;
	endRead = 1;
	return len;
}
	

ssize_t stred_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	char buff[BUFF_SIZE];
	int ret,i;
	int broj;
	ret = copy_from_user(buff, buffer, length);
	 if(ret)
                return -EFAULT;
        buff[length-1] = '\0';
	
	if(strstr(buff, "string="))
	{
		char *p = buff + 7;
	//brisemo sta god da je bilo prije
		for( i=0; i<MAX_STRING ; i++)
		{
				stred[i] = 0;
				pos = 0;  
		}
		size_t duzina = strlen(p);
		
		if(duzina <= MAX_STRING) 
		{
			strlcpy(stred, p, duzina);
		        pos = duzina;
			printk(KERN_INFO "Upisali smo string %s u bafer \n", stred);
			printk(KERN_INFO "Pozicija mi je na mjestu: %d", pos);
		}
		else
		{ 
			 printk(KERN_INFO "Previse karaktera unijeto!");
		}
	}

	else if(strstr(buff,  "truncate="))
	{
		char *p = buff+9;
		ret= sscanf(p,"%d",&broj);
		if(ret == 1)
		{
			for(i = 0; i < broj; i++)
			{
				if(pos != 0)
				{
					stred[pos-1] = 0;
					pos--;

				}
			}
		printk(KERN_INFO "Successfuly deleted %d characters\n", broj);
		printk(KERN_INFO "Sada je string:%s \n", stred);
		wake_up_interruptible(&redUpisa);
		}
		else
		{
			printk(KERN_WARNING "Pogresno unijeta komanda\n");
		}

	}
	else if( strstr (buff, "clear"))
	{
		for( i=0; i<MAX_STRING;  i++)
		{
				stred[i] = 0;  
		}
		pos = 0;
		printk(KERN_INFO "Succesfully cleared buffer \n"); 
		wake_up_interruptible(&redUpisa);
 	}
   	else if( strstr(buff, "shrink"))
	{
		int b=0;
		char *p = skip_spaces(stred);
		int razlika = strlen(stred) - strlen(p);
		if(razlika)
		{
			b = 1;
			pos = pos - razlika;
			strlcpy(stred, p ,strlen(p));
			printk(KERN_INFO "IMA %d spaceova ispred stringa \n", razlika);
		}
		else
		{
			 printk(KERN_INFO "nema praznih mjesta ispred stringa \n");
		}
		int br = 0;
		while(stred[strlen(stred) - 1] == ' ')
			{
				stred[strlen(stred) - 1] = 0;
				pos--;
				br++;
				b = 1;
			}
	printk(KERN_INFO "IMA %d spaceova iza stringa, I konacni string je:%s  \n", br, stred);
		if(b)
		{
			wake_up_interruptible(&redUpisa);
		}
	}


	else if(strstr(buff, "remove="))
	{
		char *p = buff + 7;
		size_t duzina = strlen(p);
		char pom[MAX_STRING];

		printk(KERN_INFO "rijec koju trazimo je: %s", p);

		char *ovdje;
		ovdje = strstr(stred, p);
		if(ovdje)
		{
			do
			{
			int  duzina_prvi_dio = strlen(stred) - strlen(ovdje);
			strlcpy(pom, stred, duzina_prvi_dio);
			printk(KERN_INFO "String prije rijeci je: %s \n", pom);
			ovdje += duzina;
			pos-= duzina;
			strcat(pom, ovdje);
			strlcpy(stred, pom, strlen(pom)+1);
		     	//ne znam zasto mi makne poslednji karakter uvijek, i u string= funkciji, zato sam dodavala +1, mada to nije resenje pa mi mozete objasniti kad vidite kod vjerovatno sam nesto zeznula sa duzinama pokazivaca 
		        printk(KERN_INFO "Obrisana je rijec %s i Sada je string: %s \n",p, stred);
			wake_up_interruptible(&redUpisa);
			ovdje = strstr(stred, p);
			}while(ovdje != 0);
		}
		else
		{
		printk(KERN_INFO "nema rijeci %s unutar stringa \n", p);
		}

	}
	else if(strstr(buff,"append="))
	{
		char *p = buff + 7;
		size_t duzina = strlen(p);
		int ukupna_duzina=duzina + pos;
		printk(KERN_INFO "Duzina rijeci koju dodajemo je:%d, a ukupna duzina: %d", duzina, ukupna_duzina); 

		if(down_interruptible(&sem))
			return -ERESTARTSYS;
		while(ukupna_duzina>100)
		{
			up(&sem);
			if(wait_event_interruptible(redUpisa, (ukupna_duzina<= MAX_STRING)))
				return ERESTARTSYS;
			if(down_interruptible(&sem))
				return -ERESTARTSYS;
		}
		if((ukupna_duzina) <= MAX_STRING)
		{
			strcat(stred, p);
			pos = pos + duzina;
		}
		else
		{ 
			 printk(KERN_INFO "Previse karaktera unijeto!");
		}
		up(&sem);
	}
	else{
		printk(KERN_WARNING "Pogresno unijeta komanda\n");
	}

	return length;
	}
		
static int __init stred_init(void)
{
   int ret = 0;
	int i=0;

	//Initialize array
	for (i=0; i<MAX_STRING; i++)
		stred[i] = 0;
//semafor inicijalizacija
	sema_init(&sem, 1);
   ret = alloc_chrdev_region(&my_dev_id, 0, 1, "stred");
   if (ret){
      printk(KERN_ERR "failed to register char device\n");
      return ret;
   }
   printk(KERN_INFO "char device region allocated\n");

   my_class = class_create(THIS_MODULE, "stred_class");
   if (my_class == NULL){
      printk(KERN_ERR "failed to create class\n");
      goto fail_0;
   }
   printk(KERN_INFO "class created\n");

   my_device = device_create(my_class, NULL, my_dev_id, NULL, "stred");
   if (my_device == NULL){
      printk(KERN_ERR "failed to create device\n");
      goto fail_1;
   }
   printk(KERN_INFO "device created\n");

	my_cdev = cdev_alloc();
	my_cdev->ops = &my_fops;
	my_cdev->owner = THIS_MODULE;
	ret = cdev_add(my_cdev, my_dev_id, 1);
	if (ret)
	{
      printk(KERN_ERR "failed to add cdev\n");
		goto fail_2;
	}
   printk(KERN_INFO "cdev added\n");
   printk(KERN_INFO "Hello world\n");

   return 0;

   fail_2:
      device_destroy(my_class, my_dev_id);
   fail_1:
      class_destroy(my_class);
   fail_0:
      unregister_chrdev_region(my_dev_id, 1);
   return -1;
}

static void __exit stred_exit(void)
{
   cdev_del(my_cdev);
   device_destroy(my_class, my_dev_id);
   class_destroy(my_class);
   unregister_chrdev_region(my_dev_id,1);
   printk(KERN_INFO "Goodbye, cruel world\n");
}


module_init(stred_init);
module_exit(stred_exit);





