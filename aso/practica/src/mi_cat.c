#include "def.h"
#include "ficheros.h"
#include "directorio.h"

int main (int argc, char **argv) 
{
	if (argc != 3) {
		printf("(mi_cat.c) ERROR: usar %s nombre_sistema_fichero nombre_fichero.\n",argv[0]);
		return -1;
	}
	estat st;
	if (montar(argv[1]) == -1) {
		return -1;
	}
	char buffer[sizeof(estat)];
	mi_stat_directorio(argv[2],buffer);
	memcpy(&st,buffer,sizeof(estat));
	int i,j;
	char buffer_tmp[TAM_BLOQUE];
	for (i=0;i<=(st.longitud/TAM_BLOQUE);i++) {
		mi_cat_directorio(argv[2],i,buffer_tmp);
		j = 0;
		while ((j<TAM_BLOQUE) && (buffer_tmp[j] != '#')) {
			printf("%c",buffer_tmp[j]);
			j++;
		}
	}
	if (desmontar() == -1) {
		return -1;
	}
	return 0;
}
