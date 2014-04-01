#include <stdlib.h>

#include "isr.h"
#include "task.h"
#include "sched.h"
#include "timer.h"
#include "tqueue.h"
#include "boolean.h"

#define CLOCKuHZ 1.193180
#define PERIODO 1000

// Booleana que controla el permiso para
// hacer intercambios de contexto
int noctxswitch;

// Tiempo entre que se hace el time_init()
// y se produce la última interrupción
unsigned long int last_tick;

// Tiempo entre que se produce la última 
// interrupción y la siguiente
static unsigned int next_tick;

static void far *old_tick;

extern struct t_tcb *texec;
extern struct t_queue *susp_time;


static unsigned int get_cuenta (void)
{
  asm {
    mov al,00h
    out 43h,al
    in al,40h
    mov ah,al
    in al,40h
    xchg ah,al
  }
  return _AX;
}


#define remaining_time() (get_cuenta() / CLOCKuHZ)
#define time_since_last_tick() (next_tick - remaining_time())


// Al cabo de 't' usegundos se generará una interrupción
static void set_interrupt (unsigned t)
{
  unsigned c = (unsigned)CLOCKuHZ*t;
  asm {
    mov al,34h
    out 43h,al
    mov ax,c
    out 40h,al
    mov al,ah
    out 40h,al
  }
}


// Inicialización del temporizador
void time_init (void)
{
  old_tick = install_isr(timer_isr,0x08);
  last_tick = 0;
  next_tick = PERIODO;
  set_interrupt(next_tick);
}


// Restauramos el temporizador
void time_kill (void)
{
  install_isr(old_tick,0x08);
}


// Establece el temporizador
static void set_timer (void)
{
  struct t_tcb *t;
  unsigned int min;
  unsigned long int tmp;

  min = PERIODO;
  t = q_get_hp(susp_time);
  while (t != NULL) {
    if (t->wakeup_time > last_tick) {
      tmp = t->wakeup_time - last_tick;
      if (tmp < min)
        min = tmp;
    }
    t = q_get_next(susp_time,t);
  }
  next_tick = min;
  set_interrupt(next_tick);
}


void interrupt timer_isr (void)
{
  int nctx;
  last_tick = last_tick+next_tick;
  set_timer();

  asm {
    sti
    mov al,20h
    out 20h,al
    push ax
    mov ax,TRUE;
    xchg ax,noctxswitch
    mov nctx,ax
    pop ax
  }

  if (nctx == FALSE)
    scheduler();
}


unsigned long int get_elaptime (void)
{
  return (last_tick + time_since_last_tick());
}


// Implementación del delay_until()
void delay_until (unsigned long int t)
{
  noctxswitch = TRUE; // No aceptamos intercambios de contexto
  texec->wakeup_time = t;
  texec->state = SUSP_TIME;
  if ((last_tick < t) && (t < (last_tick+next_tick))) {
    next_tick = t - get_elaptime();
    set_interrupt(next_tick);
  }
  scheduler();
}

