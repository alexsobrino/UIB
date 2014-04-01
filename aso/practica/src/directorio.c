#include <stdlib.h>
#include <string.h>
#include "def.h"
#include "ficheros_basico.h"
#include "ficheros.h"
#include "directorio.h"
#include "semaforos.h"

int montar (char *nombre) 
{
	return montar_basico(nombre);
}

int desmontar (void)
{
	return desmontar_basico();
}

int buscar_fichero (char *nombre, inodo in, int *n_inodo)
{
	entrada e;
	int posicion = 0;
	char buffer[sizeof(entrada)];
	if (in.longitud == 0) {
		printf("(directorio.c|buscar_fichero) INFO: longitud del inodo %d es 0.\n",in.n_inodo);
		return -1;
	}
	int i;
	int leido = 0;
	while (posicion < in.longitud) {
		if ((leido = mi_read_ficheros(*n_inodo,posicion,sizeof(entrada),buffer)) < 0) {
			printf("(directorio.c|buscar_fichero) ERROR: lectura incompleta [n_inodo: %d  posicion: %d  tamaño: %d  leido: %d].\n",*n_inodo,posicion,sizeof(entrada),leido);
			//return -1;
		}
		memcpy(&e,buffer,sizeof(entrada));
		i = 0;
		while (nombre[i] == e.nombre[i]) {
			if ((nombre[i] == '\0') && (i>0)) {
				*n_inodo = e.n_inodo;
				return 0;
			}
			i++;
		}
		posicion = posicion + sizeof(entrada);
	}
	return -1;
}

int buscar_directorio (char *nombre, inodo in, int *n_inodo, int control, char *fichero)
{
	char nombre_tmp[MAX_NOMBRE];
	int i,j;
	for (i=0;i<MAX_NOMBRE-1;i++) {
		nombre_tmp[i] = '\0';
	}
	if (nombre[control] != '/') {
		printf("(directorio.c|buscar_directorio) ERROR: el caracter %d del nombre debe ser '/'.\n",control);
		return -1;
	}
	i = 0;
	j = control+1;
	while ((nombre[j] != '\0') && (nombre[j] != '/')) {
		nombre_tmp[i] = nombre[j];
		i++; j++;
	}
	nombre_tmp[i] = '\0';
	if (nombre[j] == '\0') {
		strcpy(fichero,nombre_tmp);
	}
	if (buscar_fichero(nombre_tmp,in,n_inodo) == 0) {
		if (nombre[j] == '/') {
			if (leer_inodo(*n_inodo,&in) == -1) {
				printf("(directorio.c|buscar_directorio) ERROR: imposible leer el inodo %d.\n",*n_inodo);
				return -1;
			}
			if (buscar_directorio(nombre,in,n_inodo,j,fichero) == 0) {
				return 0;
			} else {
				return -1;
			}
		}
	}
	return 0;
}

int mi_create_directorio (char *nombre)
{
	entrada e;
	int n_inodo = 0;
	char nombre_tmp[MAX_NOMBRE];
	char *cadena;
	inodo in;
	int escrito = 0;
	cadena = (char *)(malloc((strlen(nombre)*sizeof(char))+1));
	char buffer[sizeof(entrada)];
	strcpy(cadena,nombre);
	if (leer_inodo(0,&in) == -1) {
		printf("(directorio.c|mi_create_directorio) ERROR: imposible leer inodo 0.\n");
		return -1;
	}
	if (buscar_directorio(cadena,in,&n_inodo,0,nombre_tmp) == -1) {
		return -1;
	}
	if (in.n_inodo != n_inodo) { 
		printf("(directorio.c|mi_create_directorio) ERROR: el archivo ya existe.\n");
		return -1;
	}
	strcpy(e.nombre,nombre_tmp);
	e.n_inodo = reservar_inodo();
	memcpy(buffer,&e,sizeof(entrada));
	if ((escrito = mi_write_ficheros(n_inodo,in.longitud,sizeof(entrada),buffer)) < sizeof(entrada)) {
		printf("(directorio.c|mi_create_directorio) ERROR: escritura incompleta [n_inodo: %d  posicion: %d  tamaño: %d  escrito: %d].\n",n_inodo,in.longitud,sizeof(entrada),escrito);
	}
	free(cadena);
	return 0;
}

int mi_read_directorio (char *nombre, int posicion, int tamano, char *buffer) 
{
	inodo in;
	int n_inodo = 0;
	char nombre_tmp[MAX_NOMBRE];
	int leido = 0;
	if (leer_inodo(0,&in) == -1) {
		printf("(directorio.c|mi_read_directorio) ERROR: imposible leer inodo 0.\n");
		return -1;
	}
	if (buscar_directorio(nombre,in,&n_inodo,0,nombre_tmp) == -1) {
		printf("(directorio.c|mi_read_directorio) ERROR: imposible encontrar directorio.\n");
		return -1;
	}
	if ((leido = mi_read_ficheros(n_inodo,posicion,tamano,buffer)) < tamano) {
		printf("(directorio.c|mi_read_directorio) ERROR: lectura incompleta [n_inodo: %d  posicion: %d  tamaño: %d  leido: %d].\n",n_inodo,posicion,tamano,leido);
		return leido;
	}
	return leido;
}

int mi_unlink_directorio (char *nombre) 
{
	entrada e;
	int n_inodo = 0;
	inodo in1,in2;
	char nombre_tmp[MAX_NOMBRE];
	char buffer[sizeof(entrada)];
	int leido,escrito;
	leido = escrito = 0;
	if (leer_inodo(n_inodo,&in1) == -1) {
		printf("(directorio.c|mi_unlink_directorio) ERROR: imposible leer inodo %d.\n",n_inodo);
		return -1;
	}
	if (buscar_directorio(nombre,in1,&n_inodo,0,nombre_tmp) < 0) {
		printf("(directorio.c|mi_unlink_directorio) ERROR: imposible encontrar directorio.\n");
		return -1;
	}
	if (in1.n_inodo == n_inodo) {
		printf("(directorio.c|mi_unlink_directorio) ERROR: el archivo no existe.\n");
		return -1;
	}
	if (leer_inodo(n_inodo,&in2) == -1) {
		printf("(directorio.c|mi_unlink_directorio) ERROR: imposible leer inodo %d.\n",n_inodo);
		return -1;
	}
	if ((in1.longitud > 0) && (in2.tipo == 2)) {
		printf("(directorio.c|mi_unlink_directorio) ERROR: el directorio contiene ficheros.\n");
		return -1;
	}
	int i;
	int posicion = 0;
	while (posicion < in1.longitud) {
		if ((leido = mi_read_ficheros(in1.n_inodo,(in1.longitud-sizeof(entrada)),sizeof(entrada),buffer)) < sizeof(entrada)) {
			printf("(directorio.c|mi_unlink_directorio) ERROR: lectura incompleta [n_inodo: %d  posicion: %d  tamaño: %d  leido: %d].\n",in1.n_inodo,(in1.longitud-sizeof(entrada)),sizeof(entrada),leido);
		}
		memcpy(&e,buffer,sizeof(entrada));
		i = 0;
		while (nombre_tmp[i] == e.nombre[i]) {
			if ((nombre_tmp[i] == '\0') && (i>0)) {
				if ((leido = mi_read_ficheros(in1.n_inodo,(in1.longitud-sizeof(entrada)),sizeof(entrada),buffer)) < sizeof(entrada)) {
					printf("(directorio.c|mi_unlink_directorio) ERROR: lectura incompleta [n_inodo: %d  posicion: %d  tamaño: %d  leido: %d].\n",in1.n_inodo,(in1.longitud-sizeof(entrada)),sizeof(entrada),leido);
				}
				if ((escrito = mi_write_ficheros(in1.n_inodo,posicion,sizeof(entrada),buffer)) < sizeof(entrada)) {
					printf("(directorio.c|mi_unlink_directorio) ERROR: lectura incompleta [n_inodo: %d  posicion: %d tamaño: %d  escrito: %d].\n",in1.n_inodo,posicion,sizeof(entrada),escrito);
				}
				if (mi_truncar_ficheros(in1.n_inodo,(in1.longitud-sizeof(entrada))) == -1) {
					printf("(directorio.c|mi_unlink_directorio) ERROR: imposible truncar archivo [n_inodo: %d  longitud: %d].\n",in1.n_inodo,in1.longitud-sizeof(entrada));
					return -1;
				}
				if (liberar_inodo(n_inodo) == -1) {
					printf("(directorio.c|mi_unlink_directorio) ERROR: imposible liberar inodo %d.\n",n_inodo);
					return -1;
				}
				return 0;
			}
			i++;
		}
		posicion = posicion + sizeof(entrada);
	}
	return -1;
}

int mi_ls_directorio (char *nombre, char *buffer)
{
	entrada e;
	int n_inodo = 0;
	int posicion = 0;
	char nombre_tmp[MAX_NOMBRE];
	char buffer_tmp[sizeof(entrada)];
	inodo in;
	int leido = 0;
	if (leer_inodo(n_inodo,&in) == -1) {
		printf("(directorio.c|mi_ls_directorio) ERROR: imposible leer inodo %d.\n",n_inodo);
		return -1;
	}
	if (buscar_directorio(nombre,in,&n_inodo,0,nombre_tmp) == -1) {
		printf("(directorio.c|mi_ls_directorio) ERROR: imposible encontrar directorio.\n");
		return -1;
	}
	if ((in.n_inodo == n_inodo) && (strlen(nombre_tmp)>1)) {
		printf("(directorio.c|mi_ls_directorio) ERROR: el directorio no existe.\n");
		return -1;
	}
	if (leer_inodo(n_inodo,&in) == -1) {
		printf("(directorio.c|mi_ls_directorio) ERROR: imposible leer inodo %d.\n",n_inodo);
		return -1;
	}
	while (posicion < in.longitud) {
		if ((leido = mi_read_ficheros(in.n_inodo,posicion,sizeof(entrada),buffer_tmp)) < sizeof(entrada)) {
			printf("(directorio.c|mi_ls_directorio) ERROR: lectura incompleta [n_inodo: %d posicion: %d  tamaño: %d  leido: %d].\n",in.n_inodo,posicion,sizeof(entrada));
			return -1;
		}
		memcpy(&e,buffer_tmp,sizeof(entrada));
		strcat(buffer,e.nombre);
		strcat(buffer,":");
		posicion = posicion + sizeof(entrada);
	}
	return in.longitud/sizeof(entrada);
}
		
int mi_mkdir_directorio (char *nombre) 
{
	entrada e;
	int n_inodo = 0;
	inodo in1,in2;
	char nombre_tmp[MAX_NOMBRE];
	char buffer[sizeof(entrada)];
	superbloque sb;
	int escrito = 0;
	if (leer_inodo(n_inodo,&in1) == -1) {
		printf("(directorio.c|mi_mkdir_directorio) ERROR: imposible leer inodo %d.\n",n_inodo);
		return -1;
	}
	if (buscar_directorio(nombre,in1,&n_inodo,0,nombre_tmp) == -1) {
		printf("(directorio.c|mi_mkdir_directorio) ERROR: imposible encontrar directorio.\n");
		return -1;
	}
	if (in1.n_inodo != n_inodo) {
		printf("(directorio.c|mi_mkdir_directorio) ERROR: el directorio ya existe.\n");
		return -1;
	}
	strcpy(e.nombre,nombre_tmp);
	e.n_inodo = reservar_inodo();
	if (leer_inodo(e.n_inodo,&in2) == -1) {
		printf("(directorio.c|mi_mkdir_directorio) ERROR: imposible leer inodo %d.\n",e.n_inodo);
		return -1;
	}
	in2.tipo = 2;
	if (escribir_inodo(e.n_inodo,&in2) == -1) {
		printf("(directorio.c|mi_mkdir_directorio) ERROR: imposible escribir inodo %d.\n",e.n_inodo);
		return -1;
	}
	memcpy(buffer,&e,sizeof(entrada));
	if ((escrito = mi_write_ficheros(in1.n_inodo,in1.longitud,sizeof(entrada),buffer)) < sizeof(entrada)) {
		printf("(directorio.c|mi_mkdir_directorio) ERROR: escritura incompleta [n_inodo: %d posicion: %d  tamaño: %d  escrito: %d].\n",in1.n_inodo,in1.longitud,sizeof(entrada),escrito);
	}
	return 0;
}

int mi_write_directorio (char *nombre, int posicion, int tamano, char *buffer)
{
	inodo in;
	int n_inodo = 0;
	char nombre_tmp[MAX_NOMBRE];
	int escrito = 0;
	if (leer_inodo(0,&in) == -1) {
		printf("(directorio.c|mi_write_directorio) ERROR: imposible leer inodo 0.\n");
		return -1;
	}
	if (buscar_directorio(nombre,in,&n_inodo,0,nombre_tmp) == -1) {
		printf("(directorio.c|mi_write_directorio) ERROR: imposible encontrar directorio.\n");
		return -1;
	}
	if ((escrito = mi_write_ficheros(n_inodo,posicion,tamano,buffer)) < 0) {
		printf("(directorio.c|mi_write_directorio) ERROR: escritura incompleta [n_inodo: %d posicion: %d  tamaño: %d  escrito: %d].\n",n_inodo,posicion,tamano,escrito);
		return escrito;
	}
	return escrito;
}

int mi_stat_directorio (char *nombre, char *buffer) 
{
	estat st;
	inodo in;
	int n_inodo = 0;
	char nombre_tmp[MAX_NOMBRE];
	if (leer_inodo(n_inodo,&in) == -1) {
		printf("(directorio.c|mi_stat_directorio) ERROR: imposible leer inodo 0.\n");
		return -1;
	}
	if (buscar_directorio(nombre,in,&n_inodo,0,nombre_tmp) == -1) {
		printf("(directorio.c|mi_stat_directorio) ERROR: imposible encontrar directorio.\n");
		return -1;
	}
	if ((in.n_inodo == n_inodo) && (n_inodo>0)) {
		printf("(directorio.c|mi_stat_directorio) ERROR: el archivo no existe.\n");
		return -1;
	}
	mi_stat_ficheros(n_inodo,&st);
	memcpy(buffer,&st,sizeof(estat));
	return 0;
}

int mi_cat_directorio (char *nombre, int parte, char *buffer)
{
	inodo in;
	int n_inodo = 0;
	char nombre_tmp[MAX_NOMBRE];
	if (leer_inodo(n_inodo,&in) == -1) {
		printf("(directorio.c|mi_cat_directorio) ERROR: imposible leer inodo 0.\n");
		return -1;
	}
	if (buscar_directorio(nombre,in,&n_inodo,0,nombre_tmp) == -1) {
		printf("(directorio.c|mi_cat_directorio) ERROR: imposible encontrar directorio.\n");
		return -1;
	}
	if (in.n_inodo == n_inodo) {
		printf("(directorio.c|mi_cat_directorio) ERROR: el archivo no existe.\n");
		return -1;
	}
	if (leer_inodo(n_inodo,&in) == -1) {
		printf("(directorio.c|mi_cat_directorio) ERROR: imposible leer inodo %d.\n",n_inodo);
		return -1;
	}
	int tmp;
	if ((tmp = mi_read_ficheros(n_inodo,parte*TAM_BLOQUE,TAM_BLOQUE,buffer)) < TAM_BLOQUE) {
		buffer[tmp] = '#';
	}
}
