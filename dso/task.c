#include <dos.h>
#include <stdio.h>
#include <stdlib.h>

#include "task.h"
#include "sched.h"
#include "boolean.h"
#include "fpu.h"

// Siguiente identificador de tarea a usar.
static unsigned next_tid = 1;

/* Definición de variables globales
 * - Copia del antiguo SS y SP
 * - Task Code Segment y Task Instruction Pointer
 * - Base de la pila
 * - Task Control Block
 * - Vector de punteros a TCBs
 * - Número de tareas */
static unsigned oldss, oldsp;
static unsigned tcs, tip;
static unsigned stack_base;
static struct t_tcb *tcb;
extern struct t_tcb **vtcb;
extern unsigned num_tasks;

static struct t_tcb tcbs[MAX_TASKS];
static unsigned int last_tcb = 0;


// Crea la pila de la tarea definida por la variable global 'tcb'
static void create_stack (void)
{
	tcb->tss = FP_SEG(tcb->stack);
	stack_base = FP_OFF(&tcb->stack[MAX_STACK_SIZE - 1]);
	tcb->tsp = stack_base;
}


// Inicialización de la pila
static void init_stack (void far *function)
{
	// Guardamos una copia del valor de SS y SP
	oldss = _SS;
	oldsp = _SP;
	tcs = FP_SEG(function);
	tip = FP_OFF(function);
  // Sobreescribimos el SS y SP del sistema con los de la tarea. 
  // Así tendremos acceso a parámetros/variables locales...
	_SS = tcb->tss;
	_SP = tcb->tsp;
	// Guardamos en la pila de la tarea
	asm push tcs
	asm push tip
	asm push stack_base
	tcb->tsp = _SP;
	// Restauramos la pila del sistema
	_SS = oldss;
	_SP = oldsp;
}


// Verificación de que la prioridad asignada es única
static int check_prior (unsigned int p)
{
	int i;
	for (i=0;i<num_tasks;i++) {
		if (p == (vtcb[i]->prior))
			return FALSE;
	}
	return TRUE;
}


// Definición de una tarea con prioridad 'p'
void def_task (void far *function, unsigned int p)
{
	tcb = &tcbs[last_tcb++];
	create_stack();
	tcb->tid = next_tid++;
	if (check_prior(p)) {
		tcb->prior = p;
	} else {
		printf("(def_task) ERROR: prioridad no valida.\n");
		exit(1);
	}
  tcb->a_prior_din = 0;
  tcb->prior_din[tcb->a_prior_din] = p;
	sched_addtcb(tcb);
	fpu_init(tcb->fpu_buf);
	init_stack(function);
}


// Elimina la memoria de la tarea apuntada por el TCB 't'
void kill_task (struct t_tcb *t)
{
	free(t->stack);
	free(t);
}


// Tarea de menor prioridad. Será la que se ejecutará cuando no haya nada más que ejecutar...
static void idle_task(void)
{
  while (1) {
    NULL;
	}
}


// Creación de la tarea 'idle_task'
void create_idle_task (int p)
{
	tcb = &tcbs[last_tcb++];
	create_stack();
	tcb->tid = 0;
	if (check_prior(p)) {
		tcb->prior = p;
	} else {
		printf("(create_idle_task) ERROR: prioridad no valida.\n");
		exit(1);
	}
	tcb->prior = p;
	sched_addtcb(tcb);
	init_stack(idle_task);
}
