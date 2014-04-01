/*	/-----------------------------------------------\
	|						|
	|	 Práctica de Sistemas Operatiros	|
	|	---------------------------------	|
 	|	 Ricardo Galli | UIB 2002 - 2003	|
	|						|
	\-----------------------------------------------/

	Alumnos:
		- Alejandro Sobrino Beltrán - 43132352
			(jander@mallorcaweb.net)
		- Enrique Pereiro Arceredillo - 431
			(shouad@ono.com)
		- Miguel Angel Quiles Evans - 43
			(wingfox02@hotmail.com)

	Ingeniería en Informática de Sistemas

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

#include "semaforos.h"
#include "lectores_escritores.h"

#define CLIENTES 50
#define SERVIDORES 2
#define POS_ARRAY 100

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
struct datos_array *compartida;
int mutex_clientes;
int acabados;
int pid_servidores[SERVIDORES];
int contador = 0;

void enterrador()
{
	int enterrados = 0;
	while(wait3(NULL, WNOHANG, NULL) > 0) {
		enterrados++;
		acabados++;
	}
	if(enterrados > 0) 
		printf("Acabados: %d\n", acabados);
}

int crear_cola(int clave)
{
	int cola;
	cola = msgget(clave, IPC_CREAT|0666);
	if(cola < 0 ) {
		printf("Error en cola %d\n", cola);
		exit (0);
	}
	printf ("COLA: %d\n", cola);
	return cola;
}

void remover_cola(int cola)
{
	msgctl(cola, IPC_RMID, 0);
}

int crear_shm (int shmd)
{
	shmd = shmget(IPC_PRIVATE,sizeof(struct datos_array)*POS_ARRAY,0666);
	if (shmd < 0) {
		printf("Error: problemas con la memoria compartida.\n");
		exit(0); // MIRAR si 0 o 1...
	}
	printf("Memoria compartida: %d\n",shmd);
	return shmd;
}

void remover_shm (int shmd)
{
	shmctl(shmd,IPC_RMID,0);
	printf("Eliminada memoria compartida: %d\n",shmd);
}

void acabar(int s)
{
	remover_cola(cola1);
	remover_cola(cola2);
	printf ("Saliendo %d, recibí %d mensajes\n", getpid(), contador);
	exit(0);
}

servidor()
{
	struct mimensaje buff,buff2;
	int res,temp,temp_valor;
	//struct datos_array *compartida;

	signal (SIGTERM, acabar);
	printf ("Servidor: %d\n", getpid());
	while (1) {
		res = msgrcv(cola1, &buff, sizeof(struct datos_mensaje), 0, 0);
		if (res < 0) {
			printf ("Error recepción %d %s\n", getpid(), strerror(errno));
		} else {
			if (buff.datos.operacion == 0) { /*escritura*/
				//compartida = (struct datos_array *) shmat(shmd,NULL,0); // segun relay no va aqui, sino en el main()
				// entrada escritores
				printf("Servidor (%d): modificando valor %d de cliente %d\n",getpid(),buff.datos.valor,buff.tipo);
				entrada_escritores();
				//contador += buff.datos.valor;
				temp_valor = buff.datos.valor;
				temp_valor++;
				compartida[buff.datos.indice].valor = temp_valor;
				temp = compartida[buff.datos.indice].n_mod+1;
				compartida[buff.datos.indice].n_mod = temp;
				compartida[buff.datos.indice].pid_mod = buff.tipo;
				// salida escritores
				//salida_escritores();
				buff2 = buff;
				buff2.datos.valor = temp_valor;
				res = msgsnd(cola2,&buff2,sizeof(struct datos_mensaje),0);
				salida_escritores();
				if (res < 0) {
					printf ("Servidor, error envio %d,%d,  %s\n", getpid(), cola2, strerror(errno));
				}
			} else {	/*lectura*/
				//compartida = (struct datos_array *) shmat(shmd,NULL,0); // segun relay no va aqui, sino en el main()
				// entrada lectores
				printf("Servidor (%d): leyendo valor %d de cliente %d\n",getpid(),buff.datos.indice,buff.tipo);
				entrada_lectores();
				buff2 = buff;
				buff2.datos.valor = compartida[buff.datos.indice].valor;
				// salida lectores
				//salida_lectores();
				res = msgsnd(cola2, &buff2, sizeof(struct datos_mensaje), 0);
				salida_lectores();
				if (res < 0) {
					printf ("Servidor, error envio %d,%d,  %s\n", getpid(), cola2, strerror(errno));
				}
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
		buff.datos.operacion = rand()%4;
		buff.datos.indice = rand()%100;
		if (buff.datos.operacion == 0) {
			printf("Cliente (%d) - Modificación\n",getpid());
			/* modificación */
			buff.datos.valor = 0;
			buff.datos.operacion = 1;
			// esperar_semaforo o algo asin (wait)
			esperar_sem(mutex_clientes);
			res = msgsnd(cola1, &buff, sizeof(struct datos_mensaje), 0);
			printf("Cliente (%d): realizando petición de valor para modificación %d\n",getpid(),buff.datos.indice);
			if (res < 0) {
				printf ("Error envio %d,%d,  %s\n", getpid(), cola1, strerror(errno));
			}
			res = msgrcv(cola2, &buff2, sizeof(struct datos_mensaje), getpid(), 0);
			if (res < 0) {
				printf ("Error recepción %d,%d,  %s\n", getpid(), cola2, strerror(errno));
			}
			buff.datos.valor = buff2.datos.valor+1;
			buff.datos.operacion = 0;
			res = msgsnd(cola1, &buff, sizeof(struct datos_mensaje), 0);
			printf("Cliente (%d): realizada modificación en %d con valor %d\n",getpid(),buff.datos.indice,buff.datos.valor);
			if (res < 0) {
				printf ("Error envio %d,%d,  %s\n", getpid(), cola1, strerror(errno));
			}
			res = msgrcv(cola2, &buff2, sizeof(struct datos_mensaje), getpid(), 0);
			if (res < 0) {
				printf ("Error recepción %d,%d,  %s\n", getpid(), cola2, strerror(errno));
			}
			// signal
			senalizar_sem(mutex_clientes);
		} else { /*consulta*/
			printf("Cliente (%d) - Consulta\n",getpid());
			buff.datos.valor = 0;
			//esperar_sem(mutex_clientes);
			res = msgsnd(cola1, &buff, sizeof(struct datos_mensaje), 0);
			if (res < 0) {
				printf ("Error envio %d,%d,  %s\n", getpid(), cola1, strerror(errno));
			}
			res = msgrcv(cola2, &buff2, sizeof(struct datos_mensaje), getpid(), 0);
			//senalizar_sem(mutex_clientes);
			if (res < 0) {
				printf ("Error recepción %d,%d,  %s\n", getpid(), cola2, strerror(errno));
			}
			printf("Cliente (%d): valor = %d\n", getpid(), buff2.datos.valor);
		}
		usleep(10000);
	}
}



main()
{
	int i,pit0;

	acabados=0;

	setlinebuf(stdout);
	cola1 = crear_cola(50);
	cola2 = crear_cola(51);
	mutex_clientes = nuevo_sem(17,1);
	inicializar_sem(mutex_clientes,1);
	inicializar_le();
	shmd = crear_shm(shmd);
	compartida = (struct datos_array *) shmat(shmd,NULL,0);
	//srand(getpid()); /* inicializar la semilla del random <- si no va matar a relay */
	for (i=0; i < SERVIDORES; i++) {
		if ((pid_servidores[i] = fork()) == 0) {
			if (pid_servidores[i] < 0) {
				break;
			}
			servidor();
			exit(0);
		}
	}
	for(i=0; i < CLIENTES; i++) {
		if((pit0 = fork()) == 0 ) {
			if (pit0 < 0) {
				break;
			}
			cliente();
			exit(0);
		}
		usleep(10000);
		enterrador();
	}
	while (acabados < CLIENTES) { /* Espera a clientes */
		enterrador();
		usleep(10000);
	}
	for (i=0; i < SERVIDORES; i++) {
		printf ("Matando servidor %d\n", pid_servidores[i]);
		kill(pid_servidores[i], SIGTERM);
	}
	while (acabados < (CLIENTES + SERVIDORES))  {
		enterrador();
		usleep(10000);
	}
	remover_shm(shmd);
	eliminar_sem(mutex_clientes);
}
