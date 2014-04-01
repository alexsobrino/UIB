#include "def.h"
#include "ficheros.h"
#include "directorio.h"

int main (int argc, char **argv)
{
	if (argc != 3) {
		printf("(mi_ls.c) ERROR: usar %s nombre_sistema_fichero nombre_directorio.\n",argv[0]);
		return -1;
	}
	if (montar(argv[1]) == -1) {
		return -1;
	}
	char buffer[sizeof(estat)];
	if (mi_stat_directorio(argv[2],buffer) == -1) {
		return -1;
	}
	estat st;
	memcpy(&st,buffer,sizeof(estat));
	char buffer_tmp[MAX_NOMBRE*(st.longitud/sizeof(entrada))];
	memset(buffer_tmp,'\0',MAX_NOMBRE*(st.longitud/sizeof(entrada)));
	int n_f = mi_ls_directorio(argv[2],buffer_tmp);
	printf("(mi_ls.c) Número de ficheros: %d.\n\n",n_f);
	int i = 0;
	while (buffer_tmp[i] != '\0') {
		if (buffer_tmp[i] == ':') {
			printf("\n");
		} else {
			printf("%c",buffer_tmp[i]);
		}
		i++;
	}
	printf("\n");
	if (desmontar() == -1) {
		return -1;
	}
	return 0;
}
