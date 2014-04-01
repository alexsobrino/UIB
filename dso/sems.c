#include <stdio.h>
#include <stdlib.h>

#include "boolean.h"
#include "sems.h"
#include "task.h"
#include "sched.h"

// Variables globales externas
extern int noctxswitch;
extern struct t_tcb *texec;
extern unsigned int wake_sem_bin;
extern unsigned int wake_sem_ipcp;
extern semaphore *sem_bin;
extern semaphore_ipcp *sem_ipcp;



// SEMÁFOROS BINARIOS


// Crear el semáforo binario
semaphore *sem_create (short v)
{
	semaphore *s;
	if ((s = (semaphore *)malloc(sizeof(semaphore))) == NULL) {
		printf("(sem_create) ERROR: malloc failed.\n");
		exit(1);
	}
	s->valor = v;
	sched_addsem(s);
	return s;
}


// Eliminar el semáforo binario 's'
void sem_destroy (semaphore *s)
{
	q_rm(s->q);
	free(s);
}


// Implementación de 'wait' sobre semáforos binarios
void sem_wait (semaphore *s)
{
	noctxswitch = TRUE;
	if (s->valor) {
		s->valor = 0;
		noctxswitch = FALSE;
	} else {
		texec->state = SUSP_SEM_BIN;
		sem_bin = s;
		scheduler();
	}
}


// Implementación de 'signal' sobre semáforos binarios
void sem_signal (semaphore *s)
{
	noctxswitch = TRUE;
	if (q_empty(s->q)) {
		s->valor = 1;
		noctxswitch = FALSE;
	} else {
		wake_sem_bin = TRUE;
		sem_bin = s;
		scheduler();
	}
}



// SEMÁFOROS IPCP


// Crear el semáforo IPCP
semaphore_ipcp *semipcp_create (short v, unsigned int t)
{
  semaphore_ipcp *s;
  if ((s = (semaphore_ipcp *)malloc(sizeof(semaphore_ipcp))) == NULL) {
    printf("(semipcp_create) ERROR: malloc failed.\n");
    exit(1);
  }
  s->valor = v;
  s->techo = t;
  sched_addsemipcp(s);
  return s;
}


// Eliminar el semáforo IPCP 's'
void semipcp_destroy (semaphore_ipcp *s)
{
  q_rm(s->q);
  free(s);
}


// Implementación de 'wait' sobre semáforos IPCP
void semipcp_wait (semaphore_ipcp *s)
{
  noctxswitch = TRUE;
  if (texec->a_prior_din <= (MAX_PRIOR_DIN-1)) {
    texec->prior_din[texec->a_prior_din] = texec->prior;
    texec->a_prior_din = texec->a_prior_din+1;
    texec->prior = s->techo;
    if (s->valor) {
      s->valor = 0;
      noctxswitch = FALSE;
    } else {
      texec->state = SUSP_SEM_IPCP;
      sem_ipcp = s;
      scheduler();
    }
  } else {
    printf("(semipcp_wait) ERROR: MAX_PRIOR_DIN failed.\n");
    exit(1);
  }
}


// Implementación de 'signal' sobre semáforos IPCP
void semipcp_signal (semaphore_ipcp *s)
{
  noctxswitch = TRUE;
  if (q_empty(s->q)) {
    s->valor = 1;
    noctxswitch = FALSE;
    texec->a_prior_din = texec->a_prior_din-1;
    texec->prior = texec->prior_din[texec->a_prior_din];
  } else {
    wake_sem_ipcp = TRUE;
    sem_ipcp = s;
    scheduler();
  }
}
