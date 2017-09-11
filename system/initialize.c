/* initialize.c - nulluser, sysinit */

/* Handle system initialization and become the null process */

#include <xinu.h>
#include <string.h>

extern	void	start(void);	/* Start of Xinu code			*/
extern	void	*_end;		/* End of Xinu code			*/

/* Function prototypes */

extern	void main(void);	/* Main is the first process created	*/
static	void sysinit(); 	/* Internal system initialization	*/
extern	void meminit(void);	/* Initializes the free memory list	*/
local	process startup(void);	/* Process to finish startup tasks	*/

/* Declarations of major kernel variables */

struct	procent	proctab[NPROC];	/* Process table			*/
struct	sentry	semtab[NSEM];	/* Semaphore table			*/
struct	memblk	memlist;	/* List of free memory blocks		*/

/* Active system status */

int	prcount;		/* Total number of live processes	*/
pid32	currpid;		/* ID of currently executing process	*/

/*declare group table */
struct groupent grouptab[2];
//int32 unixtable[60][3];
/* initializing values of groups */
/*grouptab[0].group = PROPORTIONALSHARE;
grouptab[0].initialpriority = 10;
grouptab[0].newprio = grouptab[0].initialpriority;
grouptab[1].group = TSSCHED;
grouptab[1].initialpriority = 10;
grouptab[1].newprio = grouptab[1].initialpriority;
*/

/*initialize unix table for ts scheduling*/
/*int32 unixtable [60][3] = {{200,0,50},
{200,0,50},
{200,0,50},
{200,0,50},
{200,0,50},
{200,0,50},
{200,0,50},
{200,0,50},
{200,0,50},
{200,0,50},
{160,0,51},
{160,1,51},
{160,2,51},
{160,3,51},
{160,4,51},
{160,5,51},
{160,6,51},
{160,7,51},
{160,8,51},
{160,9,51},
{120,10,52},
{120,11,52},
{120,12,52},
{120,13,52},
{120,14,52},
{120,15,52},
{120,16,52},
{120,17,52},
{120,18,52},
{120,19,52},
{80,20,53},
{80,21,53},
{80,22,53},
{80,23,53},
{80,24,53},
{80,25,54},
{80,26,54},
{80,27,54},
{80,28,54},
{80,29,54},
{40,30,55},
{40,31,55},
{40,32,55},
{40,33,55},
{40,34,55},
{40,35,56},
{40,36,57},
{40,37,58},
{40,38,58},
{40,39,58},
{40,40,58},
{40,41,58},
{40,42,58},
{40,43,58},
{40,44,58},
{40,45,58},
{40,46,58},
{40,47,58},
{40,48,58},
{20,49,59}
};*/

/* Control sequence to reset the console colors and cusor positiion	*/

#define	CONSOLE_RESET	" \033[0m\033[2J\033[;H"

/*------------------------------------------------------------------------
 * nulluser - initialize the system and become the null process
 *
 * Note: execution begins here after the C run-time environment has been
 * established.  Interrupts are initially DISABLED, and must eventually
 * be enabled explicitly.  The code turns itself into the null process
 * after initialization.  Because it must always remain ready to execute,
 * the null process cannot execute code that might cause it to be
 * suspended, wait for a semaphore, put to sleep, or exit.  In
 * particular, the code must not perform I/O except for polled versions
 * such as kprintf.
 *------------------------------------------------------------------------
 */

void	nulluser()
{	
	struct	memblk	*memptr;	/* Ptr to memory block		*/
	uint32	free_mem;		/* Total amount of free memory	*/
	
	/* Initialize the system */

	sysinit();

	/* Output Xinu memory layout */
	free_mem = 0;
	for (memptr = memlist.mnext; memptr != NULL;
						memptr = memptr->mnext) {
		free_mem += memptr->mlength;
	}
	kprintf("%10d bytes of free memory.  Free list:\n", free_mem);
	for (memptr=memlist.mnext; memptr!=NULL;memptr = memptr->mnext) {
	    kprintf("           [0x%08X to 0x%08X]\n",
		(uint32)memptr, ((uint32)memptr) + memptr->mlength - 1);
	}

	kprintf("%10d bytes of Xinu code.\n",
		(uint32)&etext - (uint32)&text);
	kprintf("           [0x%08X to 0x%08X]\n",
		(uint32)&text, (uint32)&etext - 1);
	kprintf("%10d bytes of data.\n",
		(uint32)&ebss - (uint32)&data);
	kprintf("           [0x%08X to 0x%08X]\n\n",
		(uint32)&data, (uint32)&ebss - 1);

	/* Enable interrupts */

	enable();

        /* Limit network relevant calls for CS503 courses   */
	///* Initialize the network stack and start processes */

	//net_init();

	/* Create a process to finish startup and start main */

	resume(create((void *)startup, INITSTK, PROPORTIONALSHARE, INITPRIO,
					"Startup process", 0, NULL));

	/* Become the Null process (i.e., guarantee that the CPU has	*/
	/*  something to run when no other process is ready to execute)	*/

	while (TRUE) {
		;		/* Do nothing */
	}

}


/*------------------------------------------------------------------------
 *
 * startup  -  Finish startup takss that cannot be run from the Null
 *		  process and then create and resumethe main process
 *
 *------------------------------------------------------------------------
 */
local process	startup(void)
{
        /* Limit network relevant calls for CS503 courses   */
	//uint32	ipaddr;			/* Computer's IP address	*/
	//char	str[128];		/* String used to format output	*/


	///* Use DHCP to obtain an IP address and format it */

	//ipaddr = getlocalip();
	//if ((int32)ipaddr == SYSERR) {
	//	kprintf("Cannot obtain an IP address\n");
	//} else {
	//	/* Print the IP in dotted decimal and hex */
	//	ipaddr = NetData.ipucast;
	//	sprintf(str, "%d.%d.%d.%d",
	//		(ipaddr>>24)&0xff, (ipaddr>>16)&0xff,
	//		(ipaddr>>8)&0xff,        ipaddr&0xff);
	//
	//	kprintf("Obtained IP address  %s   (0x%08x)\n", str,
	//							ipaddr);
	//}

	/* Create a process to execute function main() */

	resume(create((void *)main, INITSTK, PROPORTIONALSHARE, INITPRIO,
					"Main process", 0, NULL));

	/* Startup process exits at this point */

	/* initializing values of groups */
	grouptab[0].group = PROPORTIONALSHARE;
	grouptab[0].initialpriority = 10;
	grouptab[0].newprio = grouptab[0].initialpriority;
	grouptab[1].group = TSSCHED;
	grouptab[1].initialpriority = 10;
	grouptab[1].newprio = grouptab[1].initialpriority;

	
/*initialize unix table for ts scheduling*/
/*unixtable[60][3] = {{200,0,50},{200,0,50},{200,0,50},{200,0,50},{200,0,50},{200,0,50},{200,0,50},{200,0,50},{200,0,50},{200,0,50},{160,0,51},{160,1,51},{160,2,51},{160,3,51},{160,4,51},{160,5,51},{160,6,51},{160,7,51},{160,8,51},{160,9,51},{120,10,52},{120,11,52},{120,12,52},{120,13,52},{120,14,52},{120,15,52},{120,16,52},{120,17,52},{120,18,52},{120,19,52},{80,20,53},{80,21,53},{80,22,53},{80,23,53},{80,24,53},{80,25,54},{80,26,54},{80,27,54},{80,28,54},{80,29,54},{40,30,55},{40,31,55},{40,32,55},{40,33,55},{40,34,55},{40,35,56},{40,36,57},{40,37,58},{40,38,58},{40,39,58},{40,40,58},{40,41,58},{40,42,58},{40,43,58},{40,44,58},{40,45,58},{40,46,58},{40,47,58},{40,48,58},{20,49,59}};	
	
	kprintf("\nInside initialize, unixtable entry for 20 : %d\n",unixtable[20][1]);*/
	return OK;
}


/*------------------------------------------------------------------------
 *
 * sysinit  -  Initialize all Xinu data structures and devices
 *
 *------------------------------------------------------------------------
 */
static	void	sysinit()
{
	int32	i;
	struct	procent	*prptr;		/* Ptr to process table entry	*/
	struct	sentry	*semptr;	/* Ptr to semaphore table entry	*/

	/* Platform Specific Initialization */

	platinit();

	/* Reset the console */

	kprintf(CONSOLE_RESET);
	kprintf("\n%s\n\n", VERSION);

	/* Initialize the interrupt vectors */

	initevec();
	
	/* Initialize free memory list */
	
	meminit();

	/* Initialize system variables */

	/* Count the Null process as the first process in the system */

	prcount = 1;

	/* Scheduling is not currently blocked */

	Defer.ndefers = 0;

	/* Initialize process table entries free */

	for (i = 0; i < NPROC; i++) {
		prptr = &proctab[i];
		prptr->prstate = PR_FREE;
		prptr->prname[0] = NULLCH;
		prptr->prstkbase = NULL;
		prptr->prprio = 0;
	}

	/* Initialize the Null process entry */	

	prptr = &proctab[NULLPROC];
	prptr->prstate = PR_CURR;
	prptr->prprio = 2147483647;
	strncpy(prptr->prname, "prnull", 7);
	prptr->prstkbase = getstk(NULLSTK);
	prptr->prstklen = NULLSTK;
	prptr->prstkptr = 0;
	/*initialize group for null process*/
	prptr->prgroup = PROPORTIONALSHARE;
	prptr->prpi = 0;
	prptr->prri = 1;
	prptr->prquantum = 0;
	prptr->prtistart = cputicks;
	currpid = NULLPROC;
	
	/* Initialize semaphores */

	for (i = 0; i < NSEM; i++) {
		semptr = &semtab[i];
		semptr->sstate = S_FREE;
		semptr->scount = 0;
		semptr->squeue = newqueue();
	}

	/* Initialize buffer pools */

	bufinit();

	/* Create a ready list for processes */

	readylist = newqueue();

	/* Initialize the real time clock */

	clkinit();

	for (i = 0; i < NDEVS; i++) {
		init(i);
	}

	return;
}

int32	stop(char *s)
{
	kprintf("%s\n", s);
	kprintf("looping... press reset\n");
	while(1)
		/* Empty */;
}

int32	delay(int n)
{
	DELAY(n);
	return OK;
}
