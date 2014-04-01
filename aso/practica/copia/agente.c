#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/resource.h>
#include "def.h"
#include "mensajes.h"
#include "servidor.h"
#include "directorio.h"

extern int mutex;
int n_acabados;
int pid_servidores[N_SERVIDORES];

void enterrador ()
{
	int n_enterrados = 0;
	while (wait3(NULL,WNOHANG,NULL) > 0) {
		n_enterrados++;
		n_acabados++;
	}
	if (n_enterrados > 0) {
		printf("(agente.c) Acabados: %d.\n",n_acabados);
	}
}

int main (int argc, char **argv)
{
	setlinebuf(stdout);
	if (montar(argv[1]) == -1) {
		return -1;
	}
	int cola_r,cola_s;
	cola_s = crear_cola(50);
	cola_r = crear_cola(51);
	mutex = inicializar_mutex(1);
	n_acabados = 0;
	int i;
	int n_creados = 0;
	for (i=0;i<N_SERVIDORES;i++) {
		if ((pid_servidores[i]=fork()) == 0) {
			printf("(agente.c) Creado serdidor [pid: %d].\n",getpid());
			servidor(cola_s,cola_r);
			exit(0);
		} else if (pid_servidores[i] > 0) {
			n_creados++;
			usleep(100000);
		} else {
			printf("(agente.c) ERROR: Imposible crear el servidor.\n");
			break;
		}
	}
	printf("(agente.c) Servidores creados: %d.\n",n_creados);
	getchar();
	for (i=0;i<n_creados;i++) {
		printf("(agente.c) Matando servidor [pid: %d].\n",pid_servidores[i]);
		kill(pid_servidores[i],SIGTERM);
	}
	while (n_acabados < n_creados) {
		enterrador();
	}
	eliminar_sem(mutex);
	eliminar_cola(cola_r);
	eliminar_cola(cola_s);
	desmontar();
}

