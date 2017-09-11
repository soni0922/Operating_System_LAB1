/* chgprio.c - chgprio */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  chgprio  -  Change the priority of a group
 *------------------------------------------------------------------------
 */
pri16   chgprio(
          int         group,            /* type of group      */
          pri16         newprio         /* New priority                 */
        )
{
        intmask mask;                   /* Saved interrupt mask         */
        /*struct  procent *prptr; */        /* Ptr to process's table entry */
        pri16   oldprio;                /* Priority to return           */

        mask = disable();
	if(group == PROPORTIONALSHARE)		/* PS group */
	{
		oldprio = grouptab[0].initialpriority;
		grouptab[0].initialpriority = newprio;
	}
	else if(group == TSSCHED)	/* TS group */
	{
		oldprio = grouptab[1].initialpriority;
		grouptab[1].initialpriority = newprio;
	}
	else
	{
		restore(mask);
        	return (pri16) SYSERR;
	}

       /* if (isbadpid(pid)) {
                restore(mask);
                return (pri16) SYSERR;
        }
        prptr = &proctab[pid];
        oldprio = prptr->prprio;
        prptr->prprio = newprio;*/
        restore(mask);
        return oldprio;
}

