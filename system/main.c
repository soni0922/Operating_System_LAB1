#include<xinu.h>

void cpubound(char ch){

        int loop;
/*      if(ch == 'A' || ch == 'B'){
                sleep(10);
                loop = 1000000;
        }else{
                loop = 1000000;
        }*/
        int i,r=2,s=3,j;
        struct procent *ptr;
        ptr=&proctab[currpid];
        for(i=0;i<10;i++){
                for(j=0;j<1000000;j++){
                        r=r*s;
                        s=r*r;
//                        kprintf("%c",ch);
                }
                kprintf("\nprocess name: %s, priority: %d, quantum: %d\n",ptr->prname, ptr->prprio, ptr->prquantum);
        }
        kprintf("\nprocess %c ends!\n",ch);
}



void iobound(char ch){

	int loop, sleepvalue;
/*	if(ch == 'A' || ch == 'B'){
		sleep(10);
		loop = 1000000;
	}else{
		loop = 1000000;
	}*/
	/*if(ch == 'A'){
		sleepvalue = 10;
	}else{
		sleepvalue = 50;
	}*/
	int i,r=2,s=3,j;
	struct procent *ptr;
	ptr=&proctab[currpid];
	for(i=0;i<10;i++){
		for(j=0;j<30;j++){
			//r=r*s;
			//s=r*r;
			sleepms(32);
			kprintf("%c",ch);
		}
		kprintf("process name: %s, priority: %d, quantum: %d",ptr->prname, ptr->prprio, ptr->prquantum);
	}
	kprintf("process %c ends!",ch);
}

void main(void)
{
	//chgprio(TSSCHED,30);

	resume(create(iobound,4096,TSSCHED,50,"processA",1,'A'));
	resume(create(iobound,4096,TSSCHED,50,"processB",1,'B'));
	//sleepms(3);
	resume(create(cpubound,4096,TSSCHED,50,"processC",1,'C'));
	resume(create(cpubound,4096,TSSCHED,50,"processD",1,'D'));


}

