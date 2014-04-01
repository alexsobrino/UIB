#ifndef _SCHED_H_
#define _SCHED_H_

#include "sems.h"
#include "task.h"

void start_multitasking (void);
void end_multitasking (void);
void far scheduler (void);
void sched_addtcb (struct t_tcb *);
void sched_addsem (semaphore *);
void sched_addsemipcp (semaphore_ipcp *);

#endif
