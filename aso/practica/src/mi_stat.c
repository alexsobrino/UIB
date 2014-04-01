#include "ficheros.h"
#include "directorio.h"

int main (int argc, char **argv)
{
	if (argc != 3) {
		printf("(mi_stat.c) ERROR: usar %s nombre_sistema_ficheros nombre_fichero.\n",argv[0]);
		return -1;
	}
	estat st;
	char buffer[sizeof(estat)];
	if (montar(argv[1]) == -1) {
		return -1;
	}
	mi_stat_directorio(argv[2],buffer);
	memcpy(&st,buffer,sizeof(estat));
	printf("Inodo: %d\nFecha de creaci�n: %sFecha de modificaci�n: %sFecha de �ltimo acceso: %sTama�o en bytes: %d.\n",st.n_inodo,ctime(&st.f_creacion),ctime(&st.f_modificacion),ctime(&st.f_ultimoacceso),st.longitud);
	if (desmontar() == -1) {
		return -1;
	}
	return 0;
}
