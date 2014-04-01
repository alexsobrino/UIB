#include "directorio.h"

int main (int argc, char **argv)
{
	if (argc != 3) {
		printf("(mi_rm.c) ERROR: usar %s nombre_sistema_ficheros nombre_fichero.\n",argv[0]);
		return -1;
	}
	if (montar(argv[1]) == -1) {
		return -1;
	}
	mi_unlink_directorio(argv[2]);
	if (desmontar() == -1) {
		return -1;
	}
}
