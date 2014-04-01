#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "cliente.h"
#include "directorio.h"
#include "cliente_lib.h"

void cliente (int cola_s, int cola_r) 
{
	char nombre[MAX_NOMBRE];
	char buffer[MAX_LINEA];
	memset(buffer,'\0',MAX_NOMBRE);
	memset(buffer,'\0',MAX_LINEA);
	sprintf(nombre,"/cliente-%d.dat",getpid());
	mi_create_cliente(nombre,cola_s,cola_r);
	sprintf(buffer,"Inicio del log [cliente: %d]\n",getpid());
	printf("        BAF·FER:  %s.\n",buffer);
	mi_write_cliente(nombre,0,strlen(buffer),buffer,cola_s,cola_r);
	int posicion = strlen(buffer);
	int i,tmp;
	time_t t;
	struct tm *p_t;
	for (i=0;i<MAX_LOG+1;i++) {
		t = time(NULL);
		p_t = localtime(&t);
		memset(buffer,'\0',MAX_LINEA);
		tmp = strftime(buffer,MAX_LINEA,"%R::%S ",p_t);
		sprintf(buffer+tmp,"Linea número %d\n",i);
		mi_write_cliente(nombre,posicion,strlen(buffer),buffer,cola_s,cola_r);
		posicion = posicion + strlen(buffer);
		usleep(100000);
	}
	memset(buffer,'\0',MAX_LINEA);
	sprintf(buffer,"Fin del log [cliente: %d]\n",getpid());
	mi_write_cliente(nombre,posicion,strlen(buffer),buffer,cola_s,cola_r);
}

