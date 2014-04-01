#include <string.h>
#include "def.h"
#include "ficheros_basico.h"
#include "ficheros.h"

int mi_read_ficheros (int n_inodo, int posicion, int tamano, char *buffer) {
	inodo in;
	int leido = 0;
	int n_bloque_p = posicion/TAM_BLOQUE;
	int n_bloque_f = (posicion+tamano-1)/TAM_BLOQUE;
	int n_byte_p = posicion%TAM_BLOQUE;
	int n_byte_f = (posicion+tamano-1)%TAM_BLOQUE;
	char buffer_tmp[TAM_BLOQUE];
	memset(buffer_tmp,0,TAM_BLOQUE);
	if (DEBUG == 1) {
		printf("(ficheros.c|mi_read_ficheros) DEBUG: entrando en la función.\n");
	}
	if (leer_inodo(n_inodo,&in) == -1) {
		printf("(ficheros.c|mi_read_ficheros) ERROR: imposible leer el inodo %d.\n",n_inodo);
		return -1;
	}
	if (n_bloque_p == n_bloque_f) {
		if (leer_bloque(in.punteros[n_bloque_p],buffer_tmp) == -1) {
			printf("(ficheros.c|mi_read_ficheros) ERROR: imposible leer el bloque %d.\n",in.punteros[n_bloque_p]);
			return -1;
		}
		if (DEBUG == 1) {
			printf("(ficheros.c|mi_read_ficheros) DEBUG: leido bloque %d.\n",in.punteros[n_bloque_p]);
		}
		memcpy(buffer,&buffer_tmp[n_byte_p],tamano);
		leido = tamano;
	} else {
		// el primero
		if (leer_bloque(in.punteros[n_bloque_p],buffer_tmp) == -1) {
			printf("(ficheros.c|mi_read_ficheros) ERROR: imposible leer el bloque %d.\n",in.punteros[n_bloque_p]);
			return -1;
		}
		if (DEBUG == 1) {
			printf("(ficheros.c|mi_read_ficheros) DEBUG: leido bloque %d.\n",in.punteros[n_bloque_p]);
		}
		memcpy(buffer,&buffer_tmp[n_byte_p],TAM_BLOQUE-n_byte_p);
		leido = TAM_BLOQUE-n_byte_p;
		// siguientes
		int n_bloque;
		for (n_bloque=n_bloque_p+1;n_bloque<n_bloque_f;n_bloque++) {
			if (leer_bloque(in.punteros[n_bloque],buffer_tmp) == -1) {
				printf("(ficheros.c|mi_read_ficheros) ERROR: imposible leer el bloque %d.\n",in.punteros[n_bloque]);
				return -1;
			}
			if (DEBUG == 1) {
				printf("(ficheros.c|mi_read_ficheros) DEBUG: leido bloque %d.\n",in.punteros[n_bloque]);
			}
			memcpy(buffer+leido,buffer_tmp,TAM_BLOQUE);
			leido = leido+TAM_BLOQUE;
		}
		// el ultimo
		if (leer_bloque(in.punteros[n_bloque_f],buffer_tmp) == -1) {
			printf("(ficheros.c|mi_read_ficheros) ERROR: imposible leer el bloque %d.\n",in.punteros[n_bloque_f]);
			return -1;
		}
		memcpy(buffer+leido,buffer_tmp,n_byte_f);
		leido = leido+n_byte_f;
	}
	in.f_ultimoacceso = time(NULL);
	if (escribir_inodo(n_inodo,&in) == -1) {
		printf("(ficheros.c|mi_read_ficheros) ERROR: imposible escribir inodo %d.\n",n_inodo);
		return -1;
	}
	if (DEBUG == 1) {
		printf("(ficheros.c|mi_read_ficheros) DEBUG: total leido %d.\n",leido);
		printf("(ficheros.c|mi_read_ficheros) DEBUG: saliendo de la función.\n");
	}
	return leido;
}
		

int mi_write_ficheros (int n_inodo, int posicion, int tamano, char *buffer)
{
	inodo in;
	int escrito = 0;
	int n_bloque_p = posicion/TAM_BLOQUE;
	int n_bloque_f = (posicion+tamano-1)/TAM_BLOQUE;
	int n_byte_p = posicion%TAM_BLOQUE;
	int n_byte_f = (posicion+tamano-1)%TAM_BLOQUE;
	char buffer_tmp[TAM_BLOQUE];
	memset(buffer_tmp,0,TAM_BLOQUE);
	if (leer_inodo(n_inodo,&in) == -1) {
		printf("(ficheros.c|mi_write_ficheros) ERROR: imposible leer inodo %d.\n",n_inodo);
		return -1;
	}
	if (n_bloque_p == n_bloque_f) {
		if (in.punteros[n_bloque_p] == -1) {
			in.punteros[n_bloque_p] = reservar_bloque();
		}
		if (leer_bloque(in.punteros[n_bloque_p],buffer_tmp) == -1) {
			printf("(ficheros.c|mi_write_ficheros) ERROR: imposible leer el bloque %d.\n",in.punteros[n_bloque_p]);
			return -1;
		}
		memcpy(buffer_tmp+n_byte_p,buffer,tamano);
		if (escribir_bloque(in.punteros[n_bloque_p],buffer_tmp) == -1) {
			printf("(ficheros.c|mi_write_ficheros) ERROR: imposible escribir el bloque %d.\n",in.punteros[n_bloque_p]);
			return -1;
		}
		if (DEBUG == 1) {
			printf("(ficheros.c|mi_write_ficheros) DEBUG: escrito bloque %d.\n",in.punteros[n_bloque_p]);
		}
		escrito = tamano;
	} else {
		// el primero
		if (in.punteros[n_bloque_p] == -1) {
			in.punteros[n_bloque_p] = reservar_bloque();
		}
		if (leer_bloque(in.punteros[n_bloque_p],buffer_tmp) == -1) {
			printf("(ficheros.c|mi_write_ficheros) ERROR: imposible leer el bloque %d.\n",in.punteros[n_bloque_p]);
			return -1;
		}
		memcpy(buffer_tmp+n_byte_p,buffer,TAM_BLOQUE-n_byte_p);
		if (escribir_bloque(in.punteros[n_bloque_p],buffer_tmp) == -1) {
			printf("(ficheros.c|mi_write_ficheros) ERROR: imposible escribir el bloque %d.\n",in.punteros[n_bloque_p]);
			return -1;
		}
		if (DEBUG == 1) {
			printf("(ficheros.c|mi_write_ficheros) DEBUG: escrito bloque %d.\n",in.punteros[n_bloque_p]);
		}
		escrito = TAM_BLOQUE-n_byte_p;
		// los siguientes
		int n_bloque;
		for (n_bloque=n_bloque_p+1;n_bloque<n_bloque_f;n_bloque++) {
			memcpy(buffer_tmp,buffer+escrito,TAM_BLOQUE);
			if (in.punteros[n_bloque] == -1) {
				in.punteros[n_bloque] = reservar_bloque();
			}
			if (escribir_bloque(in.punteros[n_bloque],buffer_tmp) == -1) {
				printf("(ficheros.c|mi_write_ficheros) ERROR: imposible escribir el bloque %d.\n",in.punteros[n_bloque]);
				return -1;
			}
			if (DEBUG == 1) {
				printf("(ficheros.c|mi_write_ficheros) DEBUG: escrito bloque %d.\n",in.punteros[n_bloque]);
			}
			escrito = escrito+TAM_BLOQUE;
		}	
		// el ultimo
		if (in.punteros[n_bloque_f] == -1) {
			in.punteros[n_bloque_f] = reservar_bloque();
		}
		if (leer_bloque(in.punteros[n_bloque_f],buffer_tmp) == -1) {
			printf("(ficheros.c|mi_write_ficheros) ERROR: imposible leer el bloque %d.\n",in.punteros[n_bloque_f]);
			return -1;
		}
		memcpy(buffer_tmp,buffer+escrito,n_byte_f);
		if (escribir_bloque(in.punteros[n_bloque_f],buffer_tmp) == -1) {
			printf("(ficheros.c|mi_write_ficheros) ERROR: imposible escribir el bloque %d.\n",in.punteros[n_bloque_f]);
			return -1;
		}
		if (DEBUG == 1) {
			printf("(ficheros.c|mi_write_ficheros) DEBUG: escrito bloque %d.\n",in.punteros[n_bloque_f]);
		}
		escrito = escrito+n_byte_f;
	}
	if (in.longitud < posicion+tamano) {
		in.longitud = posicion+tamano;
	}
	in.f_modificacion = in.f_ultimoacceso = time(NULL);
	if (escribir_inodo(n_inodo,&in) == -1) {
		printf("(ficheros.c|mi_write_ficheros) ERROR: imposible escribir inodo %d.\n",n_inodo);
		return -1;
	}
	if (DEBUG == 1) {
		printf("(ficheros.c|mi_write_ficheros) DEBUG: total escrito %d.\n",escrito);
	}
	return escrito;
}

int mi_truncar_ficheros (int n_inodo, int tamano)
{
	inodo in;
	if (tamano == 0) {
		if (liberar_inodo(n_inodo) == -1) {
			return -1;
		}
	} else {
		if (leer_inodo(n_inodo,&in) == -1) {
			return -1;
		}
		in.longitud = tamano;
		in.f_modificacion = time(NULL);
		in.f_ultimoacceso = time(NULL);
		int i;
		for (i=(tamano/TAM_BLOQUE)+1;i<(in.longitud/TAM_BLOQUE)+1;i++) {
			if (escribir_mb(in.punteros[i],0) == -1) {
				return -1;
			}
			in.punteros[i] = -1;
		}
		if (escribir_inodo(n_inodo,&in) == -1) {
			return -1;
		}
	}
	return 0;
}

int mi_stat_ficheros (int n_inodo, estat *estado)
{
	inodo in;
	if (leer_inodo(n_inodo,&in) == -1) {
		return -1;
	}
	estado->n_inodo = n_inodo;
	estado->f_creacion = in.f_creacion;
	estado->f_modificacion = in.f_modificacion;
	estado->f_ultimoacceso = in.f_ultimoacceso;
	estado->longitud = in.longitud;
	return 0;
}
