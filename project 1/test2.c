#include <stdio.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define NR_mycall 355
int main(){
	int id,val, length=3,i;
	pid_t *arr;
	id=shmget(IPC_PRIVATE,sizeof(pid_t)*length,IPC_CREAT | 0700);
	arr=shmat(id,NULL,0);
	pid_t pid;
	if(pid=fork()){
		arr[0]=pid;
		syscall(NR_mycall,arr[0],1);
		sleep(5);
		printf("Woke up\n");
		for(i=0;i<length;i++){
			val=kill(arr[i],0);
			if(!val){
				break;
			}
		}
		if(!val){
			printf("Perfect\n");
		}
		else printf("Failed\n");
	}
	else{
		if(pid=fork()){
			arr[1]=pid;
			if(pid=fork()){
				arr[2]=pid;
				return 0;
			}
			else{
				while(1){
					sleep(1);
				}
			}
		}
		else{
			while(1){
				sleep(1);
			}
		}
	}
}
