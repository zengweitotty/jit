/*
    file Name:      jit.c
    Author:         zengweitotty
    version:        V1.0
    Data:           2013/03/11
    Email:          zengweitotty@gmail.com
    Description     using proc filesystem to test kernel timer function 
*/
#include <linux/kernel.h>	//using for printk
#include <linux/module.h>	
#include <linux/init.h>	//using for function module_init module_exit
#include <linux/moduleparam.h>	//using for kernel param
#include <linux/jiffies.h>	//using for jiffies
#include <linux/sched.h>	//using for schedule
#include <linux/proc_fs.h>	//using for create_proc_read_entry

//#define LOOP_TIMERS 10

//static int loop_timer = LOOP_TIMERS;	//loop timers
//module_param(loop_timer,int,S_IRUGO);
static int Tdelay = HZ;	//one second

static int jit_busy_fn(char *buf,char **start,off_t offset,int count,int *eof,void *data){
	unsigned long j0,j1;
	//while(loop_timer--){
		j0 = jiffies;
		j1 = jiffies + Tdelay;
		while(time_before(jiffies,j1)){
			cpu_relax();		
		}
		j1 = jiffies;
		count = sprintf(buf,"%9li,%9li\n",j0,j1);		
	//}		
	*start = buf;
	return count;
}
static int jit_sched_fn(char *buf,char **start,off_t offset,int count,int *eof,void *data){
	unsigned long j0,j1;
	//while(loop_timer--){
		j0 = jiffies;
		j1 = jiffies + Tdelay;
		while(time_before(jiffies,j1)){
			schedule();		
		}
		j1 = jiffies;
		count = sprintf(buf,"%9li,%9li\n",j0,j1);		
	//}		
	*start = buf;
	return count;
		
}
static int jit_queue_fn(char *buf,char **start,off_t offset,int count,int *eof,void *data){
	unsigned long j0,j1;
	wait_queue_head_t wait;
	init_waitqueue_head(&wait);
	//while(loop_timer--){
		j0 = jiffies;
		wait_event_interruptible_timeout(wait,0,Tdelay) ;	
		j1 = jiffies;
		count = sprintf(buf,"%9li,%9li\n",j0,j1);		
	//}		
	*start = buf;
	return count;
		
}
static int jit_schedto_fn(char *buf,char **start,off_t offset,int count,int *eof,void *data){
	unsigned long temp = Tdelay;
	unsigned long j0,j1;
	//while(loop_timer--){
		j0 = jiffies;
		//set_current_state(TASK_INTERRUPTIBLE);
		do{
			temp = schedule_timeout(temp); //may schedule not after timeout,so judment by return value
			j1 = jiffies;
		}while(temp);
		count = sprintf(buf,"%9li,%9li\n",j0,j1);		
	//}		
	*start = buf;
	return count;
		
}

static int __init jit_init(void){
	create_proc_read_entry("jit_busy",0,NULL,jit_busy_fn,NULL);
	create_proc_read_entry("jit_sched",0,NULL,jit_sched_fn,NULL);
	create_proc_read_entry("jit_queue",0,NULL,jit_queue_fn,NULL);
	create_proc_read_entry("jit_schedto",0,NULL,jit_schedto_fn,NULL);
	return 0;
}

static void __exit jit_exit(void){
	remove_proc_entry("jit_busy",NULL);
	remove_proc_entry("jit_sched",NULL);
	remove_proc_entry("jit_queue",NULL);
	remove_proc_entry("jit_schedto",NULL);
}

module_init(jit_init);
module_exit(jit_exit);

MODULE_AUTHOR("zengweitotty");
MODULE_DESCRIPTION("for test");
MODULE_LICENSE("GPL");
