#include <stdlib.h>
#include <stdio.h>

#include "task.h"
#include "tqueue.h"
#include "boolean.h"


// Creación de una cola de tamaño 'size'
struct t_queue *q_create (unsigned int size)
{
  int i;
  struct t_queue *q;

  if ((q = (struct t_queue *)malloc(sizeof(struct t_queue))) == NULL) {
    printf("(q_create) ERROR en malloc.\n");
    exit(1);
  }
  q->size = size;
  if ((q->q = (struct t_tcb **)malloc(q->size * sizeof(struct t_tcb *))) == NULL) {
    printf("(q_create) ERROR en malloc.\n");
    exit(1);
  }
  for (i=0;i<size;i++)
    q->q[i] = NULL;
  return q;
}


// Añadir el tcb apuntado por 't' a la cola 'q'
void q_add (struct t_queue *q, struct t_tcb *t)
{
  q->q[t->prior] = t;
}


// Siguiente TCB a 't' en la cola 'q'
struct t_tcb *q_get_next (struct t_queue *q, struct t_tcb *t)
{
  int i = t->prior + 1;
  while ((q->q[i] == NULL) && (i < q->size)) 
    i++;
  if (i == q->size)
    // No existe el siguiente elemento
    return NULL;
  else
    return q->q[i];
}


// Indica si la cola 'q' está vacía o no a través de un booleano
int tq_empty(struct t_queue *q)
{
	int i = 0;
	while (q->q[i] == NULL && i < q->size) {
		i++;
	}
	if (i == q->size) 
    return TRUE;
	else
    return FALSE;
}


// Devuelve el TCB de la tarea de mayor prioridad en la cola 'q'
struct t_tcb *q_get_hp (struct t_queue *q)
{
  int i = 0;
  struct t_tcb *t;
  
  while ((q->q[i] == NULL) && (i < q->size))
    i++;
  if (i == q->size)
    return NULL;
  else
    return q->q[i];
}


// Indica si la cola 'q' está vacía a través de un booleano
int q_empty (struct t_queue *q)
{
  int i = 0;

  while ((q->q[i] == NULL) && (i < q->size))
    i++;
  if (i == q->size)
    return TRUE;
  else
    return FALSE;
}


// Elimina el TCB apuntado por 't' de la cola 'q'
void q_del (struct t_queue *q, struct t_tcb *t)
{
  q->q[t->prior] = NULL;
}


// Mueve el TCB 't' de la cola 'src' a la cola 'dst'
void q_move (struct t_queue *src, struct t_queue *dst, struct t_tcb *t)
{
  q_add(dst,t);
  q_del(src,t);
}


// Elimina la cola 'q', liberando la memoria
void q_rm (struct t_queue *q)
{
  free(q->q);
  free(q);
}
