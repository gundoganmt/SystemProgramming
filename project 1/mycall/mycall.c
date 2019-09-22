#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/sched.h>

asmlinkage long set_myFlag(pid_t pid,int flag){
	struct task_struct* process;
	if((current->cred->uid).val == 0){
		process=find_task_by_vpid(pid);
		process->myFlag=flag;
	}
	else return EPERM;
	return 0;
}
