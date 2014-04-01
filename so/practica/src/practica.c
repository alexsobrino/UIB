/*	/-----------------------------------------------\
 *	|						|
 *	|	 Práctica de Sistemas Operatiros	|
 *	|	---------------------------------	|
 *	|	 Ricardo Galli | UIB 2002 - 2003	|
 *	|						|
 *	\-----------------------------------------------/
 *
 *	Alumnos:
 *		- Alejandro Sobrino Beltrán - 43132352
 *			(jander@mallorcaweb.net)
 *		- Enrique Pereiro Arceredillo - 431
 *			(shouad@ono.com)
 *		- Miguel Angel Quiles Evans - 43
 *			(wingfox02@hotmail.com)
 *
 *	Ingeniería en Informática de Sistemas
 *
 */
	
#define _USE_BSD
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "semaforos.h"
#include "lectores_escritores.h"

#define clientes 1000
#define servidores 2
#define POS_ARRAY 100

int CLIENTES = clientes;
int SERVIDORES = servidores;

struct datos_array {
	int valor,pid_mod,n_mod;
};

struct datos_mensaje {
	int operacion,indice,valor;
};

struct mimensaje {
	long tipo;
	struct datos_mensaje datos;
};

int cola1, cola2;
int shmd;
int mutex_clientes;
int acabados;


void enterrador()
{
	int enterrados = 0;
	while(wait3(NULL, WNOHANG, NULL) > 0) {
		enterrados++;
		acabados++;
	}
	if(enterrados > 0)
		printf("Acabados %d de %d\n", acabados,CLIENTES+SERVIDORES);
}


/*				( comprobar_error )
 * 
 * comprueba errores a la hora de crear colas y memoria compartida, y a la hora
 * de mandar y recibir mensajes de las colas
 */

void comprobar_error (char *texto, int valor, int memd, int pid, char *error)
{
	if (valor < 0) {
		printf("%s en %d con PID %d (%s)\n",texto,memd,pid,error);
	}
}


int crear_cola(int clave)
{
	int cola;
	cola = msgget(clave, IPC_CREAT|0666);
	comprobar_error("Cola: ERROR en la creación",cola,cola,getpid(),NULL);
	if (cola < 0) exit(0);
	printf ("Cola creada con ID:: %d\n", cola);
	return cola;
}


void remover_cola(int cola)
{
	msgctl(cola, IPC_RMID, 0);
}


/*				( crear_shm )
 * 
 * crea un área de memoria compartida dada una clave identificativa
 */

int crear_shm (int shmd)
{
	shmd = shmget(IPC_PRIVATE,sizeof(struct datos_array)*POS_ARRAY,0666);
	comprobar_error("Memoria compartida: ERROR en la creación",shmd,shmd,getpid(),NULL);
	if (shmd < 0) exit(0);
	printf("Memoria compartida creada con ID: %d\n",shmd);
	return shmd;
}


/* 				( remover_shm )
 *
 * elimina un área de memoria compartida dada su clave ID
 */
 
void remover_shm (int shmd)
{
	shmctl(shmd,IPC_RMID,0);
	printf("Eliminada memoria compartida con ID: %d\n",shmd);
}


/*				( inicializar_shm )
 *
 * inicializa la memoria compartida con clave ID shmd, con valores a 0
 */
 
void inicializar_shm(int shmd)
{
	int i;
	printf("Inicializando la memoria compartida con ID %d a 0.\n",shmd);
	struct datos_array *compartida;
	compartida = (struct datos_array *) shmat(shmd,NULL,0);
	for (i=0;i<POS_ARRAY;i++) {
		compartida[i].valor = compartida[i].n_mod = compartida[i].pid_mod = 0;
	}
}


/*				( imprimir_shm )
 *
 * imprime el contenido de la memoria compartida con clave ID shmd
 */

void imprimir_shm(int shmd)
{
	int i,valor_total,n_mod_total;
	printf("Estado de la memoria compartida con ID %d\n",shmd);
	valor_total = n_mod_total = 0;
	struct datos_array *compartida;
	compartida = (struct datos_array *) shmat(shmd,NULL,0);
	for (i=0;i<POS_ARRAY;i++) {
		printf("índice: %d - valor: %d - número modificaciones: %d - pid última modificación: %d\n",i,compartida[i].valor,compartida[i].n_mod,compartida[i].pid_mod);
		valor_total = valor_total+compartida[i].valor;
		n_mod_total = n_mod_total+compartida[i].n_mod;
	}
	printf("Total de valores: %d - Total de modificaciones: %d\n",valor_total,n_mod_total);
}


void acabar(int s)
{
	remover_cola(cola1);
	remover_cola(cola2);
	exit(0);
}

servidor()
{
	struct mimensaje buff,buff2;
	int res,temp,temp_valor;
	struct datos_array *compartida;
	compartida = (struct datos_array *) shmat(shmd,NULL,0);

	signal (SIGTERM, acabar);
	printf ("Servidor con PID: %d\n", getpid());
	while (1) {
		res = msgrcv(cola1, &buff, sizeof(struct datos_mensaje), 0, 0);
		comprobar_error("Servidor: ERROR de recepción",res,cola1,getpid(),strerror(errno));
		if (res > 0) {
			if (buff.datos.operacion == 0) { 	// escritura
				printf("Servidor (PID: %d): modificando índice %d con valor %d de cliente con PID %d\n",getpid(),buff.datos.indice,buff.datos.valor,buff.tipo);
				entrada_escritores();
				compartida[buff.datos.indice].valor = buff.datos.valor;
				compartida[buff.datos.indice].n_mod = compartida[buff.datos.indice].n_mod+1;
				compartida[buff.datos.indice].pid_mod = buff.tipo;
				buff2 = buff;
				buff2.datos.valor = compartida[buff.datos.indice].valor;
				salida_escritores();
				res = msgsnd(cola2,&buff2,sizeof(struct datos_mensaje),0);
				comprobar_error("Servidor: ERROR de envío",res,cola2,getpid(),strerror(errno));
			} else {				// lectura
				printf("Servidor (PID: %d): leyendo índice %d de cliente con PID %d\n",getpid(),buff.datos.indice,buff.tipo);
				entrada_lectores();
				buff2 = buff;
				buff2.datos.valor = compartida[buff.datos.indice].valor;
				salida_lectores();
				res = msgsnd(cola2, &buff2, sizeof(struct datos_mensaje), 0);
				comprobar_error("Servidor: ERROR de envío",res,cola2,getpid(),strerror(errno));
			}
		}
	}
}

cliente()
{
	int i, res;
	struct mimensaje buff, buff2;
	srand(getpid());

	for (i=0; i < 100; i++) {
		buff.tipo = getpid();
		// generación pseudo-aleatoria de la operación y el índice donde se realizará la operación
		buff.datos.operacion = rand()%4;
		buff.datos.indice = rand()%100;
		if (buff.datos.operacion == 0) {	// escritura
			printf("Cliente (PID: %d): Modificación\n",getpid());
			buff.datos.valor = 0;
			// leemos el valor para posteriormente modificarlo
			buff.datos.operacion = 1;
			esperar_sem(mutex_clientes);
			res = msgsnd(cola1, &buff, sizeof(struct datos_mensaje), 0);
			printf("Cliente (PID: %d): realizando petición para modificar el contenido del índice %d\n",getpid(),buff.datos.indice);
			comprobar_error("Cliente: ERROR de envío",res,cola1,getpid(),strerror(errno));
			res = msgrcv(cola2, &buff2, sizeof(struct datos_mensaje), getpid(), 0);
			comprobar_error("Cliente: ERROR de recepción",res,cola2,getpid(),strerror(errno));
			buff.datos.valor = buff2.datos.valor+1;
			// procedemos a modificarlo
			buff.datos.operacion = 0;
			res = msgsnd(cola1, &buff, sizeof(struct datos_mensaje), 0);
			printf("Cliente (%d): realizada modificación en índice %d con valor %d\n",getpid(),buff.datos.indice,buff.datos.valor);
			comprobar_error("Cliente: ERROR de envío",res,cola1,getpid(),strerror(errno));
			res = msgrcv(cola2, &buff2, sizeof(struct datos_mensaje), getpid(), 0);
			comprobar_error("Cliente: ERROR de recepción",res,cola2,getpid(),strerror(errno));
			senalizar_sem(mutex_clientes);
		} else {				// lectura
			printf("Cliente (PID: %d) - Consulta\n",getpid());
			buff.datos.valor = 0;
			res = msgsnd(cola1, &buff, sizeof(struct datos_mensaje), 0);
			comprobar_error("Cliente: ERROR de envío",res,cola1,getpid(),strerror(errno));
			res = msgrcv(cola2, &buff2, sizeof(struct datos_mensaje), getpid(), 0);
			comprobar_error("Cliente: ERROR de recepción",res,cola2,getpid(),strerror(errno));
			printf("Cliente (PID: %d): valor = %d\n", getpid(), buff2.datos.valor);
		}
		usleep(10000);
	}
}



main()
{
	int i;
	int pid_servidores[SERVIDORES];
	int pid_clientes;

	acabados=0;

	// inicializaciones previas
	setlinebuf(stdout);
	cola1 = crear_cola(50);
	cola2 = crear_cola(51);
	mutex_clientes = nuevo_sem(17,1);
	inicializar_sem(mutex_clientes,1);
	inicializar_le();
	shmd = crear_shm(shmd);
	inicializar_shm(shmd);
	
	for (i=0; i < SERVIDORES; i++) {
		if ((pid_servidores[i] = fork()) == 0) {
			if (pid_servidores[i] < 0) {
				SERVIDORES = SERVIDORES - 1;
				break;
			}	
			servidor();
			exit(0);
		}
	}
	for(i=0; i < CLIENTES; i++) {
		if((pid_clientes = fork()) == 0 ) {
			if (pid_clientes < 0) {
				CLIENTES = CLIENTES - 1;
				break;
			}	
			cliente();
			exit(0);
		}
		usleep(10000);
		enterrador();
	}
	while (acabados < CLIENTES) {
		enterrador();
		usleep(10000);
	}
	for (i=0; i < SERVIDORES; i++) {
		printf ("Matando servidor con PID %d\n", pid_servidores[i]);
		kill(pid_servidores[i], SIGTERM);
	}
	while (acabados < (CLIENTES + SERVIDORES))  {
		enterrador();
		usleep(10000);
	}
	
	// final
	imprimir_shm(shmd);
	remover_shm(shmd);
	eliminar_sem(mutex_clientes);
}
