/* resched.c - resched, resched_cntl */
/* */
#include <xinu.h>

struct	defer	Defer;

//int32 unixtable[60][3];

/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */
void	resched(void)		/* Assumes interrupts are disabled	*/
{
	int32 MAXINT = 2147483647;

/*initialize unix table for ts scheduling*/
int32 unixtable[60][3] = {{200,0,50},{200,0,50},{200,0,50},{200,0,50},{200,0,50},{200,0,50},{200,0,50},{200,0,50},{200,0,50},{200,0,50},{160,0,51},{160,1,51},{160,2,51},{160,3,51},{160,4,51},{160,5,51},{160,6,51},{160,7,51},{160,8,51},{160,9,51},{120,10,52},{120,11,52},{120,12,52},{120,13,52},{120,14,52},{120,15,52},{120,16,52},{120,17,52},{120,18,52},{120,19,52},{80,20,53},{80,21,53},{80,22,53},{80,23,53},{80,24,53},{80,25,54},{80,26,54},{80,27,54},{80,28,54},{80,29,54},{40,30,55},{40,31,55},{40,32,55},{40,33,55},{40,34,55},{40,35,56},{40,36,57},{40,37,58},{40,38,58},{40,39,58},{40,40,58},{40,41,58},{40,42,58},{40,43,58},{40,44,58},{40,45,58},{40,46,58},{40,47,58},{40,48,58},{20,49,59}};



	struct procent *ptold;	/* Ptr to table entry for old process	*/
	struct procent *ptnew;	/* Ptr to table entry for new process	*/

	/* If rescheduling is deferred, record attempt and return */

	if (Defer.ndefers > 0) {
		Defer.attempt = TRUE;
		return;
	}

	/* Point to process table entry for the current (old) process */
	
	ptold = &proctab[currpid];
	
	/*kprintf("\n------------------**--------------------------\n");
	kprintf("\ncurrent pid : %d\n",currpid);
	kprintf("\ncurrent pid state: %d\n",ptold->prstate);
	kprintf("\ncurrent pid prio: %d\n",ptold->prprio);
	kprintf("\ncurrent pid group: %d\n",ptold->prgroup);
	kprintf("\ncurrent pid name: %s\n",ptold->prname);*/
	kprintf("\ninitial resched, proc name: %s , proc prio: %d, proc quantum: %d\n",ptold->prname,ptold->prprio,ptold->prquantum);

	/* count no. of ready processes from each group except null process*/
	pid32 procid; struct procent *p;int countps = 0;int countts = 0;int pickgroup;
	procid = firstid(readylist);
	
	while(procid != queuetail(readylist)){
	//kprintf("\nProcess id inside search of ready processes : %d\n",procid);

		if(procid != NULLPROC){
			p = &proctab[procid];
			//kprintf("\nProcess name : %s and process group : %d\n",p->prname,p->prgroup);
			if(p->prgroup == PROPORTIONALSHARE){
				countps++;
			}
			else if(p->prgroup == TSSCHED){
				countts++;
			}
		}
		procid = queuetab[procid].qnext;
	}
	kprintf("\ncount of ts process : %d\n",countts);
	kprintf("\ncount of ps process : %d\n",countps);


	if(ptold->prgroup == PROPORTIONALSHARE ){                       /* if curr process belongs to group A, priority is rest to initial one*/
                        //kprintf("\nVALUE of grouptab[0].initpr : %d\n",grouptab[0].initialpriority);
                        grouptab[0].newprio = grouptab[0].initialpriority;
			//kprintf("\nnew prio of PS group: %d\n",grouptab[0].newprio);
	}else if (ptold->prgroup == TSSCHED){
                        grouptab[1].newprio = grouptab[1].initialpriority;
                }

         grouptab[0].newprio = grouptab[0].newprio + countps;
         grouptab[1].newprio = grouptab[1].newprio + countts;


	if(countps ==0 && countts == 0){					/*pick group of null process */
		p = &proctab[NULLPROC];
		pickgroup = p->prgroup;
	}
	else if(countps == 0){
		pickgroup = TSSCHED;
	}
	else if (countts == 0){
		pickgroup = PROPORTIONALSHARE;
	}
	else{
		/* pick group whose prio is higher */
		if(grouptab[0].newprio >= grouptab[1].newprio){			/*if prio equal then ps group */
			pickgroup = PROPORTIONALSHARE;
		}else {
			pickgroup = TSSCHED;
		}
	}


	if(ptold->prgroup == PROPORTIONALSHARE){					/*PS scheduling policy to pick a process*/

		/* end value of ti for each process in ps scheduling*/
		ptold->prtiend = cputicks;

		//kprintf("\nend clock ticks for this process : %d\n",cputicks);

		/* checking if process should be in blocked state for ps scheduling*/
		if(ptold->prstate == PR_CURR || ptold->prstate == PR_READY || preempt <= 0){
			//kprintf("\nProcess was not blocked earlier\n");
			ptold->prblocked = FALSE;
	        }else if(preempt > 0){
	                //kprintf("\nProcess was blocked earlier\n");
	                ptold->prblocked = TRUE;
	        }

			uint32 t = ptold->prtiend - ptold->prtistart;
			ptold->prpi = ptold->prpi + (t*100/ptold->prri);
			ptold->prprio = MAXINT - ptold->prpi;				/*updating xinu priority for ps scheduling*/
			//kprintf("\nprocess new prio inside PS policy : %d\n",ptold->prprio);
	}
	else if(ptold->prgroup == TSSCHED){						/*TS scheduling policy to pick a process*/
	
		/* determine whether cpu or io bound process*/
		
		if(preempt > 0)					/* io bound process*/
		{
			/*update priority as ts_slpret and quantum accordingly*/
			//kprintf("\nTS group : old prio : %d\n",ptold->prprio);
			ptold->prprio = unixtable[ptold->prprio][2];
			 //kprintf("\nTS group : new prio : %d\n",ptold->prprio);
			ptold->prquantum = unixtable[ptold->prprio][0];
		}
		else if(preempt <= 0){							/* cpu bound process*/
		
			/*update priority as ts_tqexp and quantum accordingly*/
			ptold->prprio = unixtable[ptold->prprio][1];
			ptold->prquantum = unixtable[ptold->prprio][0];	
		}
		//kprintf("\nprocess new prio inside TS policy : %d\n",ptold->prprio);
	}

if(pickgroup == PROPORTIONALSHARE){

	//kprintf("\nPS group is picked to run\n");
	if ((ptold->prstate == PR_CURR) && (ptold->prgroup == pickgroup)) {  /* Process remains eligible */
	
	
	/*	pid32 newprocid; struct procent *ptr;
		newprocid = firstid(readylist);
		ptr = &proctab[newprocid];
		while(ptr->prgroup != pickgroup){
			newprocid = queuetab[newprocid].qnext;
			ptr = &proctab[newprocid];
		}
		if (ptold->prprio > queuetab[newprocid].qkey) {
			return;
		}

		 Old process will no longer remain current */

		ptold->prstate = PR_READY;
		insert(currpid, readylist, ptold->prprio);
	}

	/* Force context switch to highest priority ready process */

	currpid = dequeuegroup(readylist,pickgroup);
	ptnew = &proctab[currpid];
	ptnew->prstate = PR_CURR;
	preempt = QUANTUM;		/* Reset time slice for process	*/

	
	/* checking if process is scheduleed for the first time in ps scheduling*/
	if(ptnew->prpi == 0){
		//kprintf("\nprocess is scheduled for first time\n");
		ptnew->prpi = cputicks;
		ptnew->prprio = MAXINT - ptnew->prpi;			/* updating xinu priority for ps scheduling*/
	}

	/*else check if process is scheduled after blocking in ps scheduling*/
	if(ptnew->prblocked == TRUE){
		if(cputicks > ptnew->prpi){             /* max(T,pi) */
			ptnew->prpi = cputicks;
		}
		ptnew->prprio = MAXINT - ptnew->prpi;                   /* updating xinu priority for ps scheduling*/		
	}
	/*start value of ti for new process for ps scheduling*/
	ptnew->prtistart = cputicks;
	//kprintf("\n start cpu ticks in PS policy : %d\n",ptnew->prtistart);

	/*unblocking scheduled process for ps scheduling*/
	ptnew->prblocked = FALSE;
}

else if(pickgroup == TSSCHED){
	//kprintf("\n TS group is picked to run\n");	 
	 if ((ptold->prstate == PR_CURR) && (ptold->prgroup == pickgroup)) {  /* Process remains eligible */

	/*	pid32 newprocid; struct procent *ptr;
	 	newprocid = firstid(readylist);
		ptr = &proctab[newprocid];
		while(ptr->prgroup != pickgroup){
			newprocid = queuetab[newprocid].qnext;
			ptr = &proctab[newprocid];
		}
		//kprintf("\nprio of readylist same group proc : %d\n",queuetab[newprocid].qkey);
		if (ptold->prprio > queuetab[newprocid].qkey) {
	        	kprintf("\nSame prc pckd in TS with pid : %d\n",currpid);
			kprintf("\nSame prc pckd in TS with prio : %d\n",ptold->prprio);
			kprintf("\nSame prc pckd in TS with quantum : %d\n",ptold->prquantum);
			preempt = ptold->prquantum;
			return;
		}

                  Old process will no longer remain current */
                 ptold->prstate = PR_READY;
                 insert(currpid, readylist, ptold->prprio);
         }

	/* Force context switch to highest priority ready process */

	
         currpid = dequeuegroup(readylist,pickgroup);
	 ptnew = &proctab[currpid];
         ptnew->prstate = PR_CURR;
         preempt = ptnew->prquantum;              /* Reset time slice for process */

}
	
	/*kprintf("\nnew pid : %d\n",currpid);
	kprintf("new pid state: %d\n",ptnew->prstate);
	kprintf("new pid prio: %d\n",ptnew->prprio);
	kprintf("new pid group: %d\n",ptnew->prgroup);*/
	kprintf("new pid name: %s\n",ptnew->prname);

	
	
	ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
	

	//kprintf("\n--------------resched finished -----------------\n");

	/* Old process returns here when resumed */
	
	return;
}

/*------------------------------------------------------------------------
 *  resched_cntl  -  Control whether rescheduling is deferred or allowed
 *------------------------------------------------------------------------
 */
status	resched_cntl(		/* Assumes interrupts are disabled	*/
	  int32	defer		/* Either DEFER_START or DEFER_STOP	*/
	)
{
	switch (defer) {

	    case DEFER_START:	/* Handle a deferral request */

		if (Defer.ndefers++ == 0) {
			Defer.attempt = FALSE;
		}
		return OK;

	    case DEFER_STOP:	/* Handle end of deferral */
		if (Defer.ndefers <= 0) {
			return SYSERR;
		}
		if ( (--Defer.ndefers == 0) && Defer.attempt ) {
			resched();
		}
		return OK;

	    default:
		return SYSERR;
	}
}



