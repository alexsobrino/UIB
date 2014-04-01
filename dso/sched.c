#include <stdio.h>
#include <stdlib.h>

#include "kb.h"
#include "fpu.h"
#include "sems.h"
#include "task.h"
#include "timer.h"
#include "tqueue.h"
#include "boolean.h"

extern int noctxswitch;
extern unsigned long int last_tick;

// Menor prioridad asignada
static unsigned int bottom_prior = 0;
// Booleana que controla el entorno multitarea
int halt = FALSE;

unsigned num_tasks = 0;
struct t_tcb **vtcb = NULL;
unsigned llamadas = 0;
struct t_tcb *texec;

// Copias de los valores de SS y SP del sistema
static unsigned sys_ss, sys_sp;
// Booleanas que indican al scheduler si debe despertar
// alguna tarea que espera por un semáforo o un evento
unsigned int wake_event;
unsigned int wake_sem_bin;
unsigned int wake_sem_ipcp;

// Semáforos binarios
semaphore *sem_bin;
static semaphore **vsem = NULL;
static unsigned int num_sem_bin = 0;
// Semáforos IPCP
semaphore_ipcp *sem_ipcp;
static semaphore_ipcp **vsemi = NULL;
static unsigned int num_sem_ipcp = 0;

// Flags
gflags *gflag;

// Declaración de las colas
static struct t_queue *activas;
struct t_queue *susp_time;
struct t_queue *susp_evap;


// Inicialización de las colas
static void queues_init (int size)
{
  int i;
  // Creamos las colas de tareas activas y suspendidas
  activas = q_create(size);
  susp_time = q_create(size);
  susp_evap = q_create(size);
  // Creamos las colas de los semáforos
  for (i=0;i<num_sem_bin;i++)
    vsem[i]->q = q_create(size);
  for (i=0;i<num_sem_ipcp;i++)
    vsemi[i]->q = q_create(size);
  // Todas las tareas estarán 'activas'
  for (i=0;i<num_tasks;i++) {
    vtcb[i]->state = ACTIVE;
    q_add(activas,vtcb[i]);
  }
}


// Inicialización de variables, colas, etc para el planificador
void start_multitasking (void)
{
  int i;
  // Guardamos el SS y SP del sistema
  sys_sp = _SP;
  sys_ss = _SS;
  // Inicializamos variables del scheduler
  wake_event = FALSE;
  wake_sem_bin = FALSE;
  wake_sem_ipcp = FALSE;
  halt = FALSE;
  // Creamos la tarea 'idle' con la menor prioridad posible
  create_idle_task(++bottom_prior);
  // Inicializaciones varias
  kb_init();
  queues_init(bottom_prior+1);
  time_init();
  // Tarea activa con máxima prioridad
  texec = q_get_hp(activas);
  // Llamada a la tarea
  _SS = texec->tss;
  _SP = texec->tsp;
  asm pop bp
  noctxswitch = FALSE;
  asm retf
}


// Paramos el planificador, así que habrá que eliminar tareas, memoria, semáforos, ...
static void kill_multitasking (void)
{
  int i;
  time_kill();
  kb_end();
  for (i=0;i<num_tasks;i++)
    kill_task(vtcb[i]);
  q_rm(activas);
  q_rm(susp_time);
  q_rm(susp_evap);
  for (i=0;i<num_sem_bin;i++)
    sem_destroy(vsem[i]);
  for (i=0;i<num_sem_ipcp;i++)
    semipcp_destroy(vsemi[i]);
  free(vsem);
  free(vsemi);
  free(vtcb);

  // Restauramos el SS y SP del sistema
  _SS = sys_ss;
  _SP = sys_sp;
  asm pop bp
  asm retf
}


// Variable booleana
void end_multitasking (void)
{
  halt = TRUE;
}


// Activación de una tarea suspendida por tiempo
static void wake_delay_time (void)
{
  int i;
  struct t_tcb *t = q_get_hp(susp_time);
  while (t != NULL) {
    if (t->wakeup_time <= last_tick) {
      t->state = ACTIVE;
      q_move(susp_time,activas,t);
    }
    t = q_get_next(susp_time,t);
  }
}


// Activación de una tarea suspendida por evento esporádico
static void wake_delay_event (void)
{
  gflags fg;
  struct t_tcb *t = q_get_hp(susp_evap);
  while (t != NULL) {
    if (t->event.flag == gflag) {
      fg = *gflag & t->event.mask;
      switch (t->event.cond) {
        case COND_ALL_CLEAR:
          if (fg == 0) {
            t->state = ACTIVE;
            q_move(susp_evap,activas,t);
            if (t->event.cons == OPT_CONSUME)
              *gflag = *gflag | t->event.mask;
          }
          break;
        case COND_ALL_SET:
          if (fg == t->event.mask) {
            t->state = ACTIVE;
            q_move(susp_evap,activas,t);
            if (t->event.cons == OPT_CONSUME) {
              *gflag = *gflag & (~t->event.mask);
              fg = *gflag;
            }
          }
          break;
        case COND_ANY_CLEAR:
          if (fg != t->event.mask) {
            t->state = ACTIVE;
            q_move(susp_evap,activas,t);
            if (t->event.cons == OPT_CONSUME)
              *gflag = *gflag | t->event.mask;
          }
          break;
        case COND_ANY_SET:
          if (fg != 0) {
            t->state = ACTIVE;
            q_move(susp_evap,activas,t);
            if (t->event.cons == OPT_CONSUME)
              *gflag = *gflag & (~t->event.mask);
          }
          break;
      }
    }
    t = q_get_next(susp_evap,t);
  }
}


void far scheduler (void)
{
  int i;
  struct t_tcb *t;
  llamadas++;
  texec->tsp = _BP;
  fpu_save(texec->fpu_buf);

  // Movemos la tarea que se ejecutaba a donde toca
  switch (texec->state) {
    case ACTIVE:
      // nada que hacer
      break;
    case SUSP_SEM_BIN:
      q_move(activas,sem_bin->q,texec);
      break;
    case SUSP_SEM_IPCP:
      q_move(activas,sem_ipcp->q,texec);
      break;
    case SUSP_TIME:
      q_move(activas,susp_time,texec);
      break;
    case SUSP_APER:
      q_move(activas,susp_evap,texec);
  }

  // Comprobamos si ha habido un 'signal' en los semáforos
  // binarios (sem_bin)
  if (wake_sem_bin) {
    t = q_get_hp(sem_bin->q);
    t->state = ACTIVE;
    q_move(sem_bin->q,activas,t);
    wake_sem_bin = FALSE;
  }

  // Comprobamos si ha habido un 'signal' en los semáforos
  // ipcp (sem_ipcp)
  if (wake_sem_ipcp) {
    t = q_get_hp(sem_ipcp->q);
    t->state = ACTIVE;
    t->a_prior_din = t->a_prior_din-1;
    t->prior = t->prior_din[t->a_prior_din];
    q_move(sem_ipcp->q,activas,t);
    wake_sem_ipcp = FALSE;
  }

  // Comprobamos si se ha producido un evento
  if (wake_event) {
    wake_delay_event();
    wake_event = FALSE;
  }

  // Comprobamos las tareas con 'delay' de tiempo
  wake_delay_time();

  // ¿hay que parar el scheduler?
  if (halt)
    kill_multitasking();

  // Seleccionamos la tarea activa de mayor prioridad
  texec = q_get_hp(activas);
  // La ejecutamos
  fpu_restore(texec->fpu_buf);
  _SS = texec->tss;
  _SP = texec->tsp;
  asm pop bp
  noctxswitch = FALSE;
  asm retf
}


// Nueva tarea implica un nuevo TCB
void sched_addtcb (struct t_tcb *t)
{
  static unsigned n = 1;
  struct t_tcb **tmp;
  num_tasks++;
  // Vector de TCBs vacío -> reservamos memoria
  if (vtcb == NULL) {
    if ((vtcb = (struct t_tcb **)malloc(sizeof(struct t_tcb *)*n)) == NULL) {
      printf("(sched_addtcb): ERROR, malloc failed.\n");
      exit(1);
    }
  // Agrandar el vector de TCBs
  } else if (num_tasks > n) {
    n++;
    if ((tmp = (struct t_tcb **)realloc(vtcb,sizeof(struct t_tcb *)*n)) == NULL) {
      printf("(sched_addtcb): ERROR, realloc failed.\n");
      exit(1);
    }
    vtcb = tmp;
  }
  vtcb[num_tasks-1] = t;
  if (bottom_prior < t->prior)
    bottom_prior = t->prior;
}


// Nuevo semáforo binario
void sched_addsem (semaphore *s)
{
  semaphore **tmp;
  static unsigned n = 1;
  num_sem_bin++;
  if (vsem == NULL) {
    if ((vsem = (semaphore **)malloc(sizeof(semaphore *)*n)) == NULL) {
      printf("(sched_addsem): ERROR, malloc failed.\n");
      exit(1);
    }
  } else if (num_sem_bin > n) {
    n += 1;
    if ((tmp = (semaphore **)realloc(vsem,sizeof(semaphore *)*n)) == NULL) {
      printf("(sched_addsem): ERROR, realloc failed.\n");
      exit(1);
    }
    vsem = tmp;
  }
  vsem[num_sem_bin-1] = s;
}


// Nuevo semáforo IPCP
void sched_addsemipcp (semaphore_ipcp *s)
{
  semaphore_ipcp **tmp;
  static unsigned n = 1;
  num_sem_ipcp++;

  if (vsemi == NULL) {
    if ((vsemi = (semaphore_ipcp **)malloc(sizeof(semaphore_ipcp *)*n)) == NULL) {
      printf("(sched_addsemipcp): ERROR, malloc failed.\n");
      exit(1);
    }
  } else if (num_sem_ipcp > n) {
    n += 1;
    if ((tmp = (semaphore_ipcp **)realloc(vsemi,sizeof(semaphore_ipcp *)*n)) == NULL) {
      printf("(sched_addsemipcp): ERROR, realloc failed.\n");
      exit(1);
    }
    vsemi = tmp;
  }
  vsemi[num_sem_ipcp-1] = s;
}
