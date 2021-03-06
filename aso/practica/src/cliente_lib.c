#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include "def.h"
#include "mensajes.h"
#include "ficheros.h"
#include "cliente_lib.h"

int mi_write_cliente (char *path, int posicion, int tamano, char *buffer, int cola_s, int cola_r)
{
	mensaje msg1,msg2;
	strcpy(msg1.contenido.nombre,path);
	msg1.contenido.tipo = 1;
	msg1.pid = getpid();
	int i;
	int tmp = tamano/TAM_BLOQUE;
	int resto = 0;
	if (tamano % TAM_BLOQUE != 0) {
		tmp++;
		resto = tamano % TAM_BLOQUE;
	} else {
		resto = TAM_BLOQUE;
	}
	for (i=0;i<tmp;i++) {
		if (i==(tmp-1)) {
			memcpy(&msg1.contenido.info,&buffer[i*TAM_BLOQUE],resto);
			msg1.contenido.tamano = resto;
			msg1.contenido.posicion = posicion + (i*TAM_BLOQUE);
		} else {
			memcpy(&msg1.contenido.info,&buffer[i*TAM_BLOQUE],TAM_BLOQUE);
			msg1.contenido.tamano = TAM_BLOQUE;
			msg1.contenido.posicion = posicion + (i*TAM_BLOQUE);
		}
		if (msgsnd(cola_s,&msg1,sizeof(c_mensaje),0) < 0) {
			printf("(cliente_lib.c) ERROR: imposible enviar el mensaje [cola: %d].\n",cola_s);
		}
		if (msgrcv(cola_r,&msg2,sizeof(c_mensaje),getpid(),0) < 0) {
			printf("(cliente_lib.c) ERROR: imposible recibir el mensaje [cola: %d].\n",cola_r);
		}
	}
	return 0;
}

int mi_read_cliente (char *path, int posicion, int tamano, char *buffer, int cola_s, int cola_r)
{
	mensaje msg1,msg2;
	strcpy(msg1.contenido.nombre,path);
	msg1.contenido.tipo = 5;
	msg1.pid = getpid();
	int tmp = tamano/TAM_BLOQUE;
	int resto = 0;
	if (tamano % TAM_BLOQUE != 0) {
		tmp++;
		resto = tamano%TAM_BLOQUE;
	} else {
		resto = TAM_BLOQUE;
	}
	int i;
	for (i=0;i<tmp;i++) {
		if (i==(tmp-1)) {
			memcpy(&msg1.contenido.info,&buffer[i*TAM_BLOQUE],resto);
			msg1.contenido.tamano = resto;
			msg1.contenido.posicion = posicion + (i*TAM_BLOQUE);
		} else {
			memcpy(&msg1.contenido.info,&buffer[i*TAM_BLOQUE],TAM_BLOQUE);
			msg1.contenido.tamano = TAM_BLOQUE;
			msg1.contenido.posicion = posicion + (i*TAM_BLOQUE);
		}
		if (msgsnd(cola_s,&msg1,sizeof(c_mensaje),0) < 0) {
			printf("(cliente_lib.c) ERROR: imposible enviar el mensaje [cola: %d].\n",cola_s);
		}
		if (msgrcv(cola_r,&msg2,sizeof(c_mensaje),getpid(),0) < 0) {
			printf("(cliente_lib.c) ERROR: imposible recibir el mensaje [cola: %d].\n",cola_r);
		}
	}
	return 0;
}

int mi_create_cliente (char *path, int cola_s, int cola_r) 
{
	mensaje msg1,msg2;
	strcpy(msg1.contenido.nombre,path);
	msg1.contenido.tipo = 0;
	msg1.contenido.tamano = 0;
	msg1.contenido.posicion = 0;
	msg1.pid = getpid();
	if (msgsnd(cola_s,&msg1,sizeof(c_mensaje),0) < 0) {
		printf("(cliente_lib.c) ERROR: imposible enviar el mensaje [cola: %d].\n",cola_s);
	}
	if (msgrcv(cola_r,&msg2,sizeof(c_mensaje),getpid(),0) < 0) {
		printf("(cliente_lib.c) ERROR: imposible recibir el mensaje [cola: %d].\n",cola_r);
	}
	return 0;
}

int mi_dir_cliente (char *path, char *buffer, int cola_s, int cola_r)
{
	mensaje msg1,msg2;
	strcpy(msg1.contenido.nombre,path);
	strcpy(msg1.contenido.info,buffer);
	msg1.contenido.tipo = 2;
	msg1.contenido.tamano = 0;
	msg1.contenido.posicion = 0;
	msg1.pid = getpid();
	if (msgsnd(cola_s,&msg1,sizeof(c_mensaje),0) < 0) {
		printf("(cliente_lib.c) ERROR: imposible enviar el mensaje [cola: %d].\n",cola_s);
	}
	if (msgrcv(cola_r,&msg2,sizeof(c_mensaje),getpid(),0) < 0) {
		printf("(cliente_lib.c) ERROR: imposible recibir el mensaje [cola: %d].\n",cola_r);
	}
	int i = 0;
	printf("(cliente_lib.c) INFO: listado del directorio %s con %d ficheros.\n",path,msg2.contenido.tamano);
	while (msg2.contenido.info[i] != '\0') {
		if (msg2.contenido.info[i] == ':') {
			printf("\n");
		} else {
			printf("%c",msg2.contenido.info[i]);
		}
		i++;
	}
	printf("\n");
	return 0;
}

int mi_rm_cliente (char *path, int cola_s, int cola_r)
{
	mensaje msg1,msg2;
	strcpy(msg1.contenido.nombre,path);
	msg1.contenido.tipo = 3;
	msg1.contenido.tamano = 0;
	msg1.contenido.posicion = 0;
	msg1.pid = getpid();
	if (msgsnd(cola_s,&msg1,sizeof(c_mensaje),0) < 0) {
		printf("(cliente_lib.c) ERROR: imposible enviar el mensaje [cola: %d].\n",cola_s);
	}
	if (msgrcv(cola_r,&msg2,sizeof(c_mensaje),getpid(),0) < 0) {
		printf("(cliente_lib.c) ERROR: imposible recibir el mensaje [cola: %d].\n",cola_r);
	}
	return 0;
}

int mi_stat_cliente (char *path, int cola_s, int cola_r)
{
	mensaje msg1,msg2;
	estat st;
	strcpy(msg1.contenido.nombre,path);
	msg1.contenido.tipo = 4;
	msg1.contenido.tamano = 0;
	msg1.contenido.posicion = 0;
	msg1.pid = getpid();
	if (msgsnd(cola_s,&msg1,sizeof(c_mensaje),0) < 0) {
		printf("(cliente_lib.c) ERROR: imposible enviar el mensaje [cola: %d].\n",cola_s);
	}
	if (msgrcv(cola_r,&msg2,sizeof(c_mensaje),getpid(),0) < 0) {
		printf("(cliente_lib.c) ERROR: imposible recibir el mensaje [cola: %d].\n",cola_r);
	}
	memcpy(&st,msg2.contenido.info,sizeof(estat));
	printf("(cliente_lib.c) INFO: archivo %s.\n",path);
	printf("(cliente_lib.c) INFO: inodo %d con longitud %d.\n",st.n_inodo,st.longitud);
	printf("(cliente_lib.c) INFO: fecha creaci�n %s - fecha modificaci�n %s - fecha de �ltimo acceso %s.\n",ctime(&st.f_creacion),ctime(&st.f_modificacion),ctime(&st.f_ultimoacceso));
	return 0;
}
