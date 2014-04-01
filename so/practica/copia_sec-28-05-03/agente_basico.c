#define _USE_BSD
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define CLIENTES 100
#define SERVIDORES 2

typedef struct {
	int op, valor;
} Datos;

typedef struct {
	long tipo;
	Datos datos;
} Mess;

int cola1, cola2;
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


void acabar(int s)
{
	remover_cola(cola1);
	remover_cola(cola2);
	printf ("Saliendo %d, recibí %d mensajes\n", getpid(), contador);
	exit(0);
}

servidor()
{
	Mess buff;
	int res;

	signal (SIGTERM, acabar);
	printf ("Servidor: %d\n", getpid());
	while(1) {
		res = msgrcv(cola1, &buff, sizeof(Datos), 0, 0);
		if (res < 0) {
			printf ("Error recepción %d %s\n", getpid(), strerror(errno));
		} else {
			printf ("Servidor (%d), recibido valor %d de cliente %d\n", getpid(), buff.datos.valor, buff.tipo);
			contador += buff.datos.valor;
			buff.datos.valor = contador;
			res = msgsnd(cola2, &buff, sizeof(Datos), 0);
			if (res < 0) {
				printf ("Servidor, error envio %d,%d,  %s\n", getpid(), cola2, strerror(errno));
			}
		}
	}
}

cliente()
{
	int i, res;
	Mess buff, buff2;

	for (i=0; i < 100; i++) {
		buff.tipo = getpid();
		buff.datos.valor = 1;
		res = msgsnd(cola1, &buff, sizeof(Datos), 0);
		if (res < 0) {
			printf ("Error envio %d,%d,  %s\n", getpid(), cola1, strerror(errno));
		}
		res = msgrcv(cola2, &buff2, sizeof(Datos), getpid(), 0);
		if (res < 0) {
			printf ("Error recepción %d,%d,  %s\n", getpid(), cola2, strerror(errno));
		}
		printf("Cliente (%d): valor = %d\n", getpid(), buff2.datos.valor);
		usleep(10000);
	}
}



main()
{
	int i;

	acabados=0;

	setlinebuf(stdout);
	cola1 = crear_cola(50);
	cola2 = crear_cola(51);
	for (i=0; i < SERVIDORES; i++) {
		if ((pid_servidores[i] = fork()) == 0) {
			servidor();
			exit(0);
		}
	}
	for(i=0; i < CLIENTES; i++) {
		if(fork() == 0 ) {
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
}
