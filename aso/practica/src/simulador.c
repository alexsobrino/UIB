#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/resource.h>
#include "def.h"
#include "cliente.h"
#include "mensajes.h"

int n_acabados;

void enterrador (void)
{
	int n_enterrados = 0;
	while (wait3(NULL,WNOHANG,NULL) > 0) {
		n_enterrados++;
		n_acabados++;
	}
	if (n_enterrados > 0) {
		printf("(simulador.c) Acabados: %d.\n",n_acabados);
	}
}

int main (void)
{
	n_acabados = 0;
	int n_clientes = 0;
	int cola_s,cola_r;
	cola_s = crear_cola(50);
	cola_r = crear_cola(51);
	int i,pid_cliente;
	for (i=0;i<N_CLIENTES;i++) {
		if ((pid_cliente = fork()) == 0) {
			cliente(cola_s,cola_r);
			exit(0);
		} else if (pid_cliente > 0) {
			n_clientes++;
			usleep(100000);
		}
	}
	printf("(simulador.c) Clientes creados: %d.\n",n_clientes);
	while (n_acabados < n_clientes) {
		enterrador();
	}
}
