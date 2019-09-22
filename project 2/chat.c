#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include "ioctl_project2.h"

#define CLASS_NAME "message"
#define DEVICE_NAME "messagebox"
#define MESSAGE_LENGTH 32
#define MESSAGE_ARRAY_LENGTH 20
#define BUFFER_LENGTH 1024

MODULE_AUTHOR("Alper Karayaman, Mahmut Gundogan");
MODULE_LICENSE("GPL");
MODULE_VERSION("1");
MODULE_DESCRIPTION("System Programming Project 2");
static int include_read=0;
static int unread_message_limit=MESSAGE_ARRAY_LENGTH;
static int major_number;
static struct class* chat_class=NULL;
static struct device* chat_device=NULL;
static char messages[MESSAGE_ARRAY_LENGTH][MESSAGE_LENGTH];
static char message[BUFFER_LENGTH];


static int chat_open(struct inode* inode,struct file* file_pointer){
	printk(KERN_INFO "messagebox: Device opened\n");
	return 0;
}

static int reader_of(char* message,char* username){
	int i=1,j=0;
	while(message[i] != '@') i++;
	i++;
	while(message[i] != '@') i++;
	i++;
	while(username[j] != '\0'){
		if(message[i] != username[j]){ 
			return 0;
		}
		i++;
		j++;
	}
	return 1;
}

static int same_reader(char* first,char* second){
	int fpos=2,spos=2;
	while(first[fpos] != '@') fpos++;
	fpos++;
	while(second[spos] != '@') spos++;
	spos++;
	while(1){
		if(first[fpos] == ' ' && second[spos] == ' ') return 1;
		if(first[fpos] != second[spos]) return 0;
		if(first[fpos] == '\0' && second[spos] == '\0') return 1;
		fpos++;
		spos++;
	}
	return 1;
}



static void add_message(char* from,int* pos){
	int i=2;
	while(from[i] != '@'){
		message[(*pos)++]=from[i];
		i++;
	}
	message[(*pos)++]=':';
	i++;
	while(from[i] != ' ') i++;
	for(;from[i] != '\0';i++){
		message[(*pos)++]=from[i];
	}
	message[(*pos)++]='\n';
}

static ssize_t chat_read(struct file* file_pointer,char* buffer,size_t buffer_count,loff_t* offset){
	int i=0,j=0;
	printk(KERN_INFO "messagebox: Reading from device\n");
	for(i=0;i<MESSAGE_ARRAY_LENGTH;i++){
		if(messages[i][0] == '\0') continue;
		if((messages[i][0] == 'u' || include_read) && reader_of(messages[i],buffer)){
			add_message(messages[i],&j);
			messages[i][0]='r';
		}
	}
	message[j]='\0';
	copy_to_user(buffer,message,j);
	return 0;
}

static ssize_t chat_write(struct file* file_pointer,const char* buffer,size_t buffer_count,loff_t* offset){
	int i,count,pos_to_write;
	count=0;
	for(i=0;i<MESSAGE_ARRAY_LENGTH;i++){
		if(messages[i][0] == '\0') continue;
		if(messages[i][0] == 'u' && same_reader(messages[i],buffer)){
			count++;
		}
	}
	if(count == unread_message_limit){
		printk(KERN_ALERT "messagebox: Reached unread message limit\n");
		return 1;
	}
	pos_to_write=0;
	while(pos_to_write < MESSAGE_ARRAY_LENGTH && messages[pos_to_write][0] != '\0'){
		pos_to_write++;
	} 
	if(pos_to_write == MESSAGE_ARRAY_LENGTH){
		printk(KERN_ALERT "messagebox: is full\n");
		return 1;
	}
	printk(KERN_INFO "messagebox: Writing to device\n");
	sprintf(messages[pos_to_write],"%s",buffer);
	messages[pos_to_write][MESSAGE_LENGTH-1]='\0';
	return 0;
}

static int chat_release(struct inode* inode,struct file* file_pointer){
	printk(KERN_INFO "messagebox: Device closed\n");
	return 0;
}

static void delete_messages(char* user_name){
	int i;
	for(i=0;i<MESSAGE_ARRAY_LENGTH;i++){
		if(messages[i][0] == '\0') continue;
		if(reader_of(messages[i],user_name)){
			messages[i][0]='\0';
		}
	}
}

static long chat_ioctl(struct file *file_pointer, unsigned int cmd, unsigned long limit)
{
	char* delete_user;
    switch (cmd)
    {
        case INCLUDE_READ:
			include_read=limit;
            break;
        case UNREAD_LIMIT:
			unread_message_limit=limit;
            break;
        case DELETE_MESSAGES:
			copy_from_user(delete_user,(char*)limit,sizeof(char)*10);
			delete_messages(delete_user);
			break;
        default:
            return -EINVAL;
    }
 
    return 0;
}
 

static struct file_operations fops = {
	.read = chat_read,
	.write = chat_write,
	.open = chat_open,
	.release = chat_release,
	.unlocked_ioctl = chat_ioctl
};

static int chat_init(void){
	int i;
	for(i=0;i<MESSAGE_ARRAY_LENGTH;i++){
		messages[i][0]= '\0';
	}
	printk(KERN_INFO "messagebox: Started\n");
	major_number=register_chrdev(0,DEVICE_NAME,&fops);
	if(major_number < 0){
		printk(KERN_ALERT "messagebox: Error when allocating major number\n");
		return major_number;
	}
	printk(KERN_INFO "messagebox: Major number is %d\n",major_number);
	chat_class=class_create(THIS_MODULE,CLASS_NAME);
	if(IS_ERR(chat_class)){
		unregister_chrdev(major_number,DEVICE_NAME);
		printk(KERN_ALERT "messagebox: Unable to add to kernel\n");
		return PTR_ERR(chat_class);
	}
	chat_device=device_create(chat_class,NULL,MKDEV(major_number,0),NULL,DEVICE_NAME);
	if(IS_ERR(chat_device)){
		class_destroy(chat_class);
		unregister_chrdev(major_number,DEVICE_NAME);
		printk(KERN_ALERT "messagebox: Unable to create device\n");
		return PTR_ERR(chat_device);
	}
	
	return 0;
}

static void chat_exit(void){
	device_destroy(chat_class,MKDEV(major_number,0));
	class_unregister(chat_class);
	class_destroy(chat_class);
	unregister_chrdev(major_number,DEVICE_NAME);
	printk(KERN_INFO "messagebox: Ended\n");
}

module_init(chat_init);
module_exit(chat_exit);
