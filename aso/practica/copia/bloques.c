#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "def.h"

static int dsf;

// Montamos el sistema de ficheros
int montar_bloques (char *nombre) 
{
	//int dsf;
	if ((dsf = open(nombre,O_CREAT|O_RDWR,S_IRUSR|S_IWUSR)) == -1) {
		printf("(bloques.c|montar_bloques) ERROR: problema al montar el sistema de ficheros.\n");
		return -1;
	}
	printf("(bloques.c|montar_bloques) INFO: Sistema de ficheros montado.\n");
	return dsf;
}

int desmontar_bloques ()
{
	if (close(dsf) == -1) {
		printf("(bloques.c|desmontar_bloques) ERROR: Problema al desmontar el sistema de ficheros.\n");
		return -1;
	}
	printf("(bloques.c|desmontar_bloques) INFO: Sistema de ficheros desmontado.\n");
	return 0;
}

int leer_bloque (int n_bloque, char *buffer) 
{
	if (n_bloque < 0) {
		return -1;
	}
	int offset = n_bloque * TAM_BLOQUE;
	if (lseek(dsf,offset,SEEK_SET) == (offset-1)) {
		printf("(bloques.c|leer_bloque) ERROR: Problema con el lseek [offset: %d].\n",offset);
		return -1;
	}
	if (read(dsf,buffer,TAM_BLOQUE) == -1) {
		printf("(bloques.c|leer_bloque) ERROR: problema con la lectura del bloque [n_bloque: %d].\n",n_bloque);
		return -1;
	}
	if (DEBUG == 1) {
		printf("(bloques.c|leer_bloque) DEBUG: leido bloque %d offset %d.\n",n_bloque,offset);
	}
	return 0;
}

int escribir_bloque (int n_bloque, char *buffer) 
{
	if (n_bloque < 0) {
		return -1;
	}
	int offset = n_bloque * TAM_BLOQUE;
	if (lseek(dsf,offset,SEEK_SET) == (offset-1)) {
		printf("(bloques.c|escribir_bloque) ERROR: Problema con el lseek [offset: %d].\n",offset);
		return -1;
	}
	int tmp;
	if ((tmp = write(dsf,buffer,TAM_BLOQUE)) == -1) {
		printf("(bloques.c|escribir_bloque) ERROR: Problema con la lectura del bloque [n_bloque: %d].\n",n_bloque);
		return -1;
	}
	if (tmp != TAM_BLOQUE) {
		printf("(bloques.c|escribir_bloque) ERROR: se han escrito %d bytes en vez de %d.\n",tmp,TAM_BLOQUE);
	}
	if (DEBUG == 1) {
		printf("(bloques.c|escribir_bloque) DEBUG: escrito bloque %d offset %d.\n",n_bloque,offset);
	}
	return 0;
}
