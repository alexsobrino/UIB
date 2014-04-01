#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include "def.h"
#include "mensajes.h"
#include "directorio.h"

void servidor (int cola_s, int cola_r)
{
	mensaje msg1,msg2;
	int control;
	while (1) {
		if (msgrcv(cola_s,&msg1,sizeof(c_mensaje),0,0) < 0) {
			printf("(servidor.c) ERROR: imposible recibir mensaje [%d].\n",getpid());
		}
		if (msg1.contenido.tipo == 0) {
			if (mi_create_directorio(msg1.contenido.nombre) == -1) {
				 printf ("(servidor.c) ERROR: imposible crear fichero.\n");
			}
			if (DEBUG == 1) {
				printf("(servidor.c) DEBUG: hecho mi_create_directorio(%s).\n",msg1.contenido.nombre);
			}
		} else if (msg1.contenido.tipo == 1) {
			if ((msg2.contenido.tamano = mi_write_directorio(msg1.contenido.nombre,msg1.contenido.posicion,msg1.contenido.tamano,msg1.contenido.info)) == -1) {
				 printf ("(servidor.c) ERROR: imposible escribir.\n");
			}
			if (DEBUG == 1) {
				printf("(servidor.c) DEBUG: hecho mi_write_directorio(%s,%d,%d,%s).\n",msg1.contenido.nombre,msg1.contenido.posicion,msg1.contenido.tamano,msg1.contenido.info);
			}
		} else if (msg1.contenido.tipo == 2) {
			msg2.contenido.tamano = mi_ls_directorio(msg1.contenido.nombre,msg1.contenido.info);
			strcpy(msg2.contenido.info,msg1.contenido.info);
			if (DEBUG == 1) {
				printf("(servidor.c) DEBUG: hecho mi_ls_directorio(%s,%s).\n",msg1.contenido.nombre,msg1.contenido.info);
			}
		} else if (msg1.contenido.tipo == 3) {
			mi_unlink_directorio(msg1.contenido.nombre);
			if (DEBUG == 1) {
				printf("(servidor.c) DEBUG: hecho mi_unlink_directorio(%s).\n",msg1.contenido.nombre);
			}
		} else if (msg1.contenido.tipo == 4) {
			mi_stat_directorio(msg1.contenido.nombre,msg1.contenido.info);
			strcpy(msg2.contenido.info,msg1.contenido.info);
			if (DEBUG == 1) {
				printf("(servidor.c) DEBUG: hecho mi_stat_directorio(%s,%s).\n",msg1.contenido.nombre,msg1.contenido.info);
			}
		} else if (msg1.contenido.tipo == 5) {
			if ((msg2.contenido.tamano = mi_read_directorio(msg1.contenido.nombre,msg1.contenido.posicion,msg1.contenido.tamano,msg1.contenido.info)) == -1) {
				printf("(servidor.c) ERROR: imposible leer.\n");
			}
			if (DEBUG == 1) {
				printf("(servidor.c) DEBUG: hecho mi_read_directorio(%s,%d,%d,%s).\n",msg1.contenido.nombre,msg1.contenido.posicion,msg1.contenido.tamano,msg1.contenido.info);
			}
		} else {
			printf("(servidor.c) ERROR: tipo de operación no reconocida.\n");
		}
		msg2.pid = msg1.pid;
		if (msgsnd(cola_r,&msg2,sizeof(c_mensaje),0) < 0) {
			printf("(servidor.c) ERROR: imposible enviar mensaje [%d].\n",getpid());
		}
	}
}
