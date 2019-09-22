#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<ctype.h>
#include "ioctl_project2.h"
#define MAX_MESSAGE_LENGTH 100
#define BUFFER_LENGTH 1024               
static char buffer[BUFFER_LENGTH];     

int is_message(char* message){
	int i=0;
	while(message[i] != '\0' && isspace(message[i])) i++;
	return message[i] == '@';
}

int is_read(char* message){
	int i=0;
	while(message[i] != '\0' && isspace(message[i])) i++;
	return message[i] == 'r' || message[i] == 'R';
}

int main(int n,char* arguments[]){
	char username[10];
	char delete_username[10];
	char send[BUFFER_LENGTH];
	char message[MAX_MESSAGE_LENGTH];
   int exclude_include,delete_choice;
   int ret, fd;
   int max_unread_messages=atoi(arguments[2]);
   printf("Enter 1 to include read messages 0 otherwise:");
   scanf("%d",&exclude_include);
   printf("Enter unread message limit:");
   scanf("%d",&max_unread_messages);
   printf("Enter 1 to delete messages of a user 0 otherwise:");
   scanf("%d",&delete_choice);
   if(delete_choice){
	  printf("Enter username to delete messages:");
	  scanf("%s",delete_username);
	  ioctl(fd,DELETE_MESSAGES,delete_username);
	}
	getchar();
   getlogin_r(username,10);
   fd = open("/dev/messagebox", O_RDWR);             
   if (fd < 0){
      perror("Failed to open the device...");
      return errno;
   }
	if(exclude_include){
		if(ioctl(fd, INCLUDE_READ,1) == -1)
			perror("Failed to include read");
	}
	else{
		if(ioctl(fd, INCLUDE_READ,0) == -1)
			perror("Failed to exclude read");
	}
	
	if(ioctl(fd,UNREAD_LIMIT,max_unread_messages) == -1){
		perror("Failed to set unread messages limit");
	}
	
   while(1){
	   printf("Type a message to send message or r for reading your messages, otherwise program will end\n");
	   fgets(message,MAX_MESSAGE_LENGTH,stdin);
	   if(is_message(message)){
		  printf("Writing message to the device [%s].\n", message);
		  sprintf(send,"u@%s%s",username,message);
		  ret = write(fd, send, strlen(send)); 
		  if (ret < 0){
			perror("Failed to write the message to the device.");
			return errno;
	      }
	   }
	   else if(message[0] == 'r'){
		      sprintf(buffer,"%s",username);
		      ret = read(fd, buffer, BUFFER_LENGTH);        
		      if (ret < 0){
				perror("Failed to read the message from the device.");
				return errno;
			  }
			  printf("Your message box:\n%s", buffer);
	   }
	   else{
		  break; 
	   }
   }
   printf("End of the program\n");
   return 0;
}
