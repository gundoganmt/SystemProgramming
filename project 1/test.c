#include <stdio.h>
#define NR_mycall 355

int main(void){
	int pid,flag;
	scanf("%d %d",&pid,&flag);
	y=syscall(NR_mycall,pid,flag);
	printf("%d\n",y);
	return 0;
}
