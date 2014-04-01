#ifndef _TQUEUE_H_
#define _TQUEUE_H_

#include "task.h"

struct t_queue {
  unsigned int size;
  struct t_tcb **q;
};

struct t_queue *q_create (unsigned int);
void q_add (struct t_queue *, struct t_tcb *);
struct t_tcb *q_get_next (struct t_queue *, struct t_tcb *);
struct t_tcb *q_get_hp (struct t_queue *);
int q_empty (struct t_queue *);
void q_del (struct t_queue *, struct t_tcb *);
void q_move (struct t_queue *, struct t_queue *, struct t_tcb *);
void q_rm (struct t_queue *);

#endif // _TQUEUE_H_
