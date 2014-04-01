#include "kb.h"
#include "flag.h"
#include "sems.h"
#include "sched.h"
#include "timer.h"
#include "screen.h"
#include "boolean.h"

// DEFINICIÓN DE CONSTANTES

// Tamaño de alto 'height'
#define TAM_H 25
// Tamaño en ancho 'width'
#define TAM_W 80
// Límite superior (palas)
#define LSUP 3
// Límite inferior (palas)
#define LINF 25
// Límite derecho (width-1)
#define LDCH 79
// Límite izquierdo
#define LIZD 0
// Tamaño del alto de las palas 'height'
#define TAM_PALA_H 5
// Tamaño del ancho de las palas 'width'
#define TAM_PALA_W 1
// Incremento de la velocidad de la bola (20 cars/seg)
#define INC_VELBOLA 20
// Velocidad máxima de la bola
#define VEL_MAX_BOLA 200
// Velocidad mínima de la bola
#define VEL_MIN_BOLA 20
// Período de refresco de la pantalla (0.005segs == 5ms)
#define REFRESCO_PANT 0.005
// Incremento temporal de 1 seg
#define INCREMENTO_TEMP 1

// COLORES
#define COLOR_TANTEO 15
#define COLOR_TIEMPO 15
#define COLOR_TIEMPOLIM 78
#define COLOR_BG 0
#define COLOR_PALADR 64
#define COLOR_PALAIZ 64
#define COLOR_BOLA 12
#define COLOR_SEPARADOR 4
#define COLOR_JUG 14

// DEFINICION DE VARIABLES

// Coordenadas de posición
typedef struct posicion {
   unsigned int x;
   unsigned int y;
};
// Posiciones de:
//  * Bola
//  * Pala izqda
//  * Pala drcha
struct posicion POS_BOLA;
struct posicion POS_PIZD;
struct posicion POS_PDCH;

// Velocidad de la bola (cars/seg)
unsigned int VEL_BOLA;

// Puntuaciones
typedef struct puntuacion{
   unsigned int punt_1jug, punt_2jug;
};
struct puntuacion TANTEO;

// Tiempo transcurrido (segs)
unsigned int TIEMPO;
// Tiempo máximo (segs)
unsigned int TIEMPO_MAXIMO;
// Tiempo de alerta (segs)
unsigned int TIEMPO_ALERTA;

static semaphore_ipcp *sem_tanteo;
static semaphore_ipcp *sem_final;
static semaphore_ipcp *sem_pospizd;
static semaphore_ipcp *sem_pospdch;
static semaphore_ipcp *sem_velbola;
static semaphore_ipcp *sem_posbola;
static semaphore_ipcp *sem_tiempo;

// Grupo de flags para el teclado:
//  * Flag #1: tecla 'A'
//  * Flag #2: tecla 'Z'
//  * Flag #3: tecla 'K'
//  * Flag #4: tecla 'M'
//  * Flag #5: tecla '+'
//  * Flag #6: tecla '-'
//  * Flag #7: tecla 'q'
//  * Flag #8: SIN USAR
gflags *flags_teclas;

// Declaramos un grupo de flags para controlar el
// evento que los jugadores 'marquen' un tanto y la
// finalización del juego
// FLAG 1 = marca jugador derecha
// FLAG 2 = marca jugador izquierda
// FLAG 3 = finaliza el juego
gflags *flags_juego;

extern unsigned char key;


// SUBE_PI: task encargada de subir la pala izquierda
void SUBE_PI (void)
{
  while (1) {
    // Se tiene que pulsar la tecla 'A'
    waitFlag(flags_teclas,FLAG1,COND_ALL_SET,OPT_CONSUME);
    semipcp_wait(sem_pospizd);
    if (POS_PIZD.y > LSUP)
      POS_PIZD.y = POS_PIZD.y - 1;
    semipcp_signal(sem_pospizd);
	}
}


// BAJA_PI: task encargada de bajar la pala izquierda
void BAJA_PI (void)
{
	while (1) {
    // Se tiene que pulsar la tecla 'Z'
    waitFlag(flags_teclas,FLAG2,COND_ALL_SET,OPT_CONSUME);
    semipcp_wait(sem_pospizd);
    if ((POS_PIZD.y + TAM_PALA_H) < LINF)
      POS_PIZD.y = POS_PIZD.y + 1;
    semipcp_signal(sem_pospizd);
  }
}


// SUBE_PD: task encargada de subir la pala derecha
void SUBE_PD (void)
{
	while (1) {
    // Se tiene que pulsar la tecla 'K'
    waitFlag(flags_teclas,FLAG3,COND_ALL_SET,OPT_CONSUME);
    semipcp_wait(sem_pospdch);
    if (POS_PDCH.y > LSUP)
      POS_PDCH.y = POS_PDCH.y - 1;
    semipcp_signal(sem_pospdch);
	}
}


// BAJA_PD: task encargada de bajar la pala derecha
void BAJA_PD (void)
{
	while (1) {
    // Se tiene que pulsar la tecla 'M'
    waitFlag(flags_teclas,FLAG4,COND_ALL_SET,OPT_CONSUME);
    semipcp_wait(sem_pospdch);
    if ((POS_PDCH.y + TAM_PALA_H) < LINF)
      POS_PDCH.y = POS_PDCH.y + 1;
    semipcp_signal(sem_pospdch);
	}
}


// INCR_VB: task encargada de incrementar la velocidad
//          de la bola (20 caracteres/segundo)
void INCR_VB (void)
{
	while (1) {
    // Se tiene que pulsar la tecla '+'
    waitFlag(flags_teclas,FLAG5,COND_ALL_SET,OPT_CONSUME);
    semipcp_wait(sem_velbola);
    if (VEL_BOLA < VEL_MAX_BOLA)
      VEL_BOLA = VEL_BOLA + INC_VELBOLA;
    semipcp_signal(sem_velbola);
	}
}


// DECR_VB: task encargada de decrementar la velocidad
//          de la bola (20 caracteres/segundo)
void DECR_VB(void){

	while (1) {
    // Se tiene que pulsar la tecla '-'
    waitFlag(flags_teclas,FLAG6,COND_ALL_SET,OPT_CONSUME);
    semipcp_wait(sem_velbola);
    if (VEL_BOLA > VEL_MIN_BOLA)
      VEL_BOLA = VEL_BOLA - INC_VELBOLA;
    semipcp_signal(sem_velbola);
	}
}


// INCRT: task encargada de incrementar el tiempo
//        con un periodo de 1 segundo
void INCRT (void)
{
	while (1) {
    delay_until(get_elaptime() + (INCREMENTO_TEMP*1000000));
    semipcp_wait(sem_tiempo);
    TIEMPO = TIEMPO + 1;
    if (TIEMPO == TIEMPO_MAXIMO){
      // Se señaliza el evento de finalización
      semipcp_wait(sem_final);
      setFlag(flags_juego,FLAG3);
      semipcp_signal(sem_final);
    }
    semipcp_signal(sem_tiempo);
	}
}


// BOLA: task encargada de gestionar el movimiento
//       de la bola (rebotes, desplazamientos, ...)
void BOLA (void)
{
  unsigned long int tiempo_bola;
  int dir_x = 1;
  int dir_y = 1;
	while (1) {
	  // Se calcula el tiempo de activación de la bola que viene
	  // definido por (1 / vel_bola). La velocidad de movimiento
	  // se calcula en caracteres / milisegundo
    semipcp_wait(sem_velbola);
    tiempo_bola = 1000000/VEL_BOLA;
    semipcp_signal(sem_velbola);
    delay_until(get_elaptime() + tiempo_bola);
    // La nueva posición de la bola, depende directamente de la
    // anterior posición de la bola. Por ello, debemos establecer
    // una región crítica y no hacer el 'signal' hasta que se
    // haya actualizado por completo la nueva posición de la bola
    semipcp_wait(sem_posbola);
		// Condiciones de cambio de dirección horizontal (1 por linea)
		//  * rebote en el lado izquierdo
		//  * rebote en el lado derecho
		//  * choque contra la palanca izquierda
		//  * choque contra la palanza derecha
		if (((POS_BOLA.x <= LIZD) && (dir_x < 0)) ||
		    ((POS_BOLA.x >= LDCH) && (dir_x > 0)) ||
		    ((dir_x < 0) && (POS_BOLA.x == LIZD) && (POS_BOLA.y >= POS_PIZD.y) && (POS_BOLA.y <= POS_PIZD.y + TAM_PALA_H)) ||
		    ((dir_x > 0) && (POS_BOLA.x == (LDCH-1)) && (POS_BOLA.y >= POS_PDCH.y) && (POS_BOLA.y <= POS_PDCH.y + TAM_PALA_H))) {
          dir_x = - dir_x;
    }
		if (POS_BOLA.x == LIZD)
		  // Incrementamos el marcador del jugador drcho
      setFlag(flags_juego,FLAG1);
    if (POS_BOLA.x == LDCH)
      setFlag(flags_juego,FLAG2);
    // Condiciones de cambio de dirección vertical (1 por linea)
    if (((POS_BOLA.y >= LINF) && (dir_y > 0)) ||
        ((POS_BOLA.y <= LSUP) && (dir_y < 0))) {
          dir_y = - dir_y;
    }
    // Actualizamos la dirección de la bola
		POS_BOLA.x += dir_x;
		POS_BOLA.y += dir_y;
	  // Hacemos el signal, ya que la bola tiene su nueva POS
    semipcp_signal(sem_posbola);
	}
}


// VISUALIZ: task encargada de la visualización de las palas,
//           la bola, el marcador y el tiempo. Todo ello con un
//           periodo de 5ms
void VISUALIZ (void)
{
  unsigned int i,j;
  struct posicion POS_BOLA_ANT;
  POS_BOLA_ANT = POS_BOLA;
  while (1) {
    delay_until(get_elaptime() + (REFRESCO_PANT * 1000000));
    // Marcador
    semipcp_wait(sem_tanteo);
    writeuint(COLOR_TANTEO,5,1,TANTEO.punt_1jug);
    writeuint(COLOR_TANTEO,65,1,TANTEO.punt_2jug);
    semipcp_signal(sem_tanteo);
    // Tiempo
		writeuint(COLOR_TIEMPO,45,0,TIEMPO_MAXIMO);
		semipcp_wait(sem_tiempo);
    // Si el tiempo se acerca a su límite...
    if (TIEMPO <= TIEMPO_ALERTA)
      writeuint(COLOR_TIEMPO,45,1,TIEMPO);
    else
      writeuint(COLOR_TIEMPOLIM,45,1,TIEMPO);
		semipcp_signal(sem_tiempo);
    // Borramos las palas
		for (i=LSUP;i<LINF;i++){
		  writetxt(COLOR_BG, LIZD, i, " ");
		  writetxt(COLOR_BG, LDCH, i, " ");
    }
    // Borramos la bola
    semipcp_wait(sem_posbola);
    writetxt(COLOR_BG,POS_BOLA_ANT.x,POS_BOLA_ANT.y," ");
		semipcp_signal(sem_posbola);
    // Dibujamos las palas
    for(i=0;i<TAM_PALA_H;i++){
      for (j=0;j<TAM_PALA_W;j++){
        semipcp_wait(sem_pospizd);
        writetxt(COLOR_PALAIZ,LIZD,POS_PIZD.y+i," ");
        semipcp_signal(sem_pospizd);
        semipcp_wait(sem_pospdch);
        writetxt(COLOR_PALADR,LDCH,POS_PDCH.y+i," ");
        semipcp_signal(sem_pospdch);
		  }
    }
    // Dibujamos la bola
		semipcp_wait(sem_posbola);
		POS_BOLA_ANT = POS_BOLA;
		writetxt(COLOR_BOLA,POS_BOLA.x,POS_BOLA.y,"o");
		semipcp_signal(sem_posbola);
	}
}


// MARC_IZD: task encargada de incrementar el marcador izqdo
void MARC_IZD (void)
{
	while (1) {
    waitFlag(flags_juego,FLAG2,COND_ALL_SET,OPT_CONSUME);
    semipcp_wait(sem_tanteo);
    TANTEO.punt_1jug += 1;
    semipcp_signal(sem_tanteo);
	}
}


// MARC_DCH: task encargada de incrementar el marcador drcho
void MARC_DCH (void)
{
	while (1) {
    waitFlag(flags_juego, FLAG1, COND_ALL_SET, OPT_CONSUME);
    semipcp_wait(sem_tanteo);
    TANTEO.punt_2jug += 1;
    semipcp_signal(sem_tanteo);
	}
}


// FINALIZACION: task encargada de finalizar la aplicacion cuyo
//               evento vendrá determinado por una tecla o bien
//               por el tiempo máximo
void FINALIZACION (void)
{
  while (1) {
    waitFlag(flags_juego, FLAG3, COND_ALL_SET, OPT_CONSUME);
    end_multitasking();
  }
}


// Inicialización
void init_data (void)
{
  // Creamos el grupo de flags para teclado
  flags_teclas = initFlags();
  //Creamos el grupo de flags para el juego
  flags_juego = initFlags();
  // Variables
  POS_BOLA.x = 40;
  POS_BOLA.y = 10;
  POS_PIZD.x = 0;
  POS_PIZD.y = 10;
  POS_PDCH.x = 79;
  POS_PDCH.y = 10;
  VEL_BOLA = VEL_MIN_BOLA;
  TANTEO.punt_1jug = 0;
  TANTEO.punt_2jug = 0;
  TIEMPO = 0;
  TIEMPO_MAXIMO = 60;
  TIEMPO_ALERTA = TIEMPO_MAXIMO - (TIEMPO_MAXIMO / 4);
}


// Fondo
void paint_bg (void)
{
  int i,j;
  // Fondo de pantalla
  clearscreen(COLOR_BG);
  for (i=0; i < LSUP; i++){
    for(j=0; j < TAM_W; j++){
      //Fondo de la cabecera
      writetxt(COLOR_BG, j,i," ");
      if (i == LSUP-1) {
        writetxt(COLOR_SEPARADOR, j,i,"~");
      }
    }
  }
  // Cabecera
  writetxt(COLOR_JUG,1,0,"JUGADOR #1:");
  writetxt(COLOR_JUG,60,0,"JUGADOR #2: ");
  writetxt(COLOR_TIEMPO,25,0,"LIMITE: ");
  writetxt(COLOR_TIEMPO,25,1,"TIEMPO: ");
}


// Creación de los semáforos necesarios
void create_sems (void)
{
  // Creamos el semáforo tanteo (techo 9)
  sem_tanteo = semipcp_create(1,9);
  // Creamos el semáforo final (techo 6)
  sem_final = semipcp_create(1,6);
  // Creamos el semáforo pospizd (techo 2)
  sem_pospizd = semipcp_create(1,2);
  // Creamos el semáforo pospdch (techo 3)
  sem_pospdch = semipcp_create(1,3);
  // Creamos el semáforo velbola (techo 1)
  sem_velbola = semipcp_create(1,1);
  // Creamos el semáforo posbola (techo 1)
  sem_posbola = semipcp_create(1,1);
  // Creamos el semáforo tiempo (techo 6)
  sem_tiempo = semipcp_create(1,6);
}


// Definición de las tareas
//Función que define y crea las tareas especificadas
void create_tasks (void)
{
  def_task(FINALIZACION,0);
  def_task(BOLA,1);
  def_task(SUBE_PI,2);
  def_task(SUBE_PD,3);
  def_task(BAJA_PI,4);
  def_task(BAJA_PD,5);
  def_task(INCRT,6);
  def_task(INCR_VB,7);
  def_task(DECR_VB,8);
  def_task(MARC_IZD,9);
  def_task(MARC_DCH,10);
  def_task(VISUALIZ,11);
}


// Función que trata las consecuencias de un evento por teclado
void hook (void)
{
  switch (key) {
    // Presionamos 'q'
    case 16:
      end_multitasking();
		  break;
    // Presionamos 'a'
    case 30:
      setFlag(flags_teclas, FLAG1);
		  break;
    // Presionamos 'z'
    case 44:
      setFlag(flags_teclas, FLAG2);
      break;
    //tecla 'k'
    case 37:
      setFlag(flags_teclas, FLAG3);
		  break;
    //tecla 'm'
    case 50:
      setFlag(flags_teclas, FLAG4);
		  break;
    //tecla '+'
    case 27:
      setFlag(flags_teclas, FLAG5);
		  break;
    //tecla '-'
    case 53:
      setFlag(flags_teclas, FLAG6);
      break;
  }
  key = 0;
}


void main(void)
{
  // Asignamos la función de hook's
  set_kb_hook(hook);
  init_data();
  create_tasks();
  create_sems();
  paint_bg();
  // big-bang
	start_multitasking();
}
