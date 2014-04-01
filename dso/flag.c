#include <stdio.h>
#include <stdlib.h>

#include "boolean.h"
#include "flag.h"
#include "sched.h"

// Variables globales
extern int noctxswitch;
extern struct t_tcb *texec;
extern unsigned int wake_event;
extern gflags *gflag;


void setFlag (gflags *grupflag, gflags mask)
{
  // No se realizan intercambios de contexto
  noctxswitch = TRUE;
  // Establecemos los flags con la máscara (OR bit a bit)
  *grupflag = *grupflag | mask;
  // Indicamos que se produce un evento
  wake_event = TRUE;
  gflag = grupflag;
  scheduler();
}


void clrFlag (gflags *grupflag, gflags mask)
{
  // No se realizan intercambios de contexto
  noctxswitch = TRUE;
  // Establecemos los flags con la máscara (AND bit a bit)
  *grupflag = *grupflag & (~mask);
  wake_event = TRUE;  //Indicamos que se ha producido un evento
  gflag = grupflag;//Indicamos a grupflag como el último en ser modificado
  scheduler();
}


gflags *initFlags (void)
{
  gflags *flag;
  if ((flag = (gflags *)malloc(sizeof(gflags))) == NULL) {
    printf("(initFlags): ERROR, malloc failed.\n");
    exit(1);
  }
  *flag = FLAG0;
  return flag;
}


void waitFlag (gflags *grupflag, gflags mask, int cond, int option)
{
  gflags c_flag = *grupflag;
  unsigned int result = FALSE;
  c_flag = c_flag & mask;

  if (cond == COND_ALL_CLEAR) {
    if (c_flag == 0)
      result = TRUE;
  } else if (cond == COND_ALL_SET) {
    if (c_flag == mask)
      result = TRUE;
  } else if (cond == COND_ANY_CLEAR) {
    if (c_flag != mask)
      result = TRUE;
  } else { // cond == COND_ANY_SET
    if (c_flag != 0)
      result = TRUE;
  }
  // No se realizan intercambios de contexto
  noctxswitch = TRUE;
  if (result) { // Se cumple la condición
    noctxswitch = FALSE;
  } else {      // Se suspende
    texec->event.mask = mask;
    texec->event.cond = cond;
    texec->event.cons = option;
    texec->event.flag = grupflag;
    texec->state = SUSP_APER;
    scheduler();
  }
}
