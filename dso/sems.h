#ifndef _SEMS_H_
#define _SEMS_H_

#include "tqueue.h"

// Semáforo binario
typedef struct t_sem {
	struct t_queue *q;
	short valor;
} semaphore;

// Semáforo IPCP
typedef struct t_sem_ipcp {
  struct t_queue *q;
  short valor;
  unsigned int techo;
} semaphore_ipcp;

// Funciones de los semáforos binarios
semaphore *sem_create (short);
void sem_destroy (semaphore *);
void sem_wait (semaphore *);
void sem_signal (semaphore *);

// Funciones de los semáforos IPCP
semaphore_ipcp *semipcp_create (short, unsigned int);
void semipcp_destroy (semaphore_ipcp *);
void semipcp_wait (semaphore_ipcp *);
void semipcp_signal (semaphore_ipcp *);


#endif // _SEMS_H_
