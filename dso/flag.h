#ifndef _FLAG_H_
#define _FLAG_H_

#define FLAG0 0
#define FLAG1 1
#define FLAG2 2
#define FLAG3 4
#define FLAG4 8
#define FLAG5 16
#define FLAG6 32
#define FLAG7 64
#define FLAG8 128

/* Condiciones de flags
 * - Todos los flags a 0
 * - Todos los flags a 1
 * - Cualquier flag a 0
 * - Cualquier flag a 1 */
#define COND_ALL_CLEAR 0
#define COND_ALL_SET 1
#define COND_ANY_CLEAR 2
#define COND_ANY_SET 3

#define OPT_CONSUME 0
#define OPT_DONT_CONSUME 1

// 8 flags, 1 bit por flag
typedef unsigned char gflags;


//Estructura que define máscara, condición, consumo de la misma,
// y el grupo de flags para especificar  un conjunto de eventos o flag
//(el grupo de flags especifico por el cual se suspendió la tarea es
// necesario ya que sino podria especificarse un evento para otro
// grupo y que la cond. coincidiera con la misma por la que la tarea
// estaba suspendida, activandose erroneamente.
typedef struct flag_struct {
  gflags        mask;  //Estado de los 8 flags para el evento
  gflags       *flag;  //Es la direcc. del conjunto de flags por el cual se ha suspendido
  unsigned int  cond;  //Condicion sobre la mascara (COND_xxx)
  unsigned int  cons;  //Valor que indica el consumo de flags o no
} gflags_event;


// Set a '1' de los flags indicados en 'mask' del grupo de flags apuntado por 'grupflags'
void setFlag (gflags *, gflags);

// Set a '0' de los flags indicados en 'mask' del grupo de flags apuntado por 'groupflags'
void clrFlag (gflags *, gflags);

// Crea e inicializa a '0' un grupo de flags
gflags *initFlags(void);

/*
  Funcion que suspende la tarea que lo invoca hasta que los flags del
  'grupflag' satisfacen la condicion 'cond' (que seran del tipo COND_xxx
  especificadas en flag.h), los cuales hacen referencia a los flags especificados
  por la mascara 'mask' , igualmente con la opcion 'option' (que será del tipo
  OPT_xxx especificados en flag.h) definiremos si una vez cumplida la condición
  se han de cumplir los flags.
*/

void waitFlag (gflags *, gflags, int, int);

#endif // _FLAG_H_
