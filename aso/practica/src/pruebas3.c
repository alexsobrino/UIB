#include "ficheros_basico.h"
#include "ficheros.h"

int main (void) {
	if (montar_basico("disco.imagen") == -1) {
		return -1;
	}
	/*int n_inodo = reservar_inodo();
	char buffer[20];
	strcpy(buffer,"0123456789ABCABCABC\0");
	printf("n_inodo: %d tamaño: %d buffer: %s\n",n_inodo,sizeof(buffer),buffer);
	if (mi_write_ficheros(n_inodo,0,sizeof(buffer),buffer) < sizeof(buffer)) {
		printf("problema...\n");
	}
	char buffer_tmp[20];
	memset(buffer_tmp,0,20);
	if (mi_read_ficheros(n_inodo,0,20,buffer_tmp) < 20) {
		printf("oballa...\n");
	}
	printf("buffer_tmp: %s\n",buffer_tmp);*/
	int n_inodo = reservar_inodo();
	if (mi_write_ficheros(n_inodo,0,18,"hola me llamo alex") < 18) {
		printf("a cascarla\n");
	}
	char buffer[19];
	memset(buffer,0,19);
	if (mi_read_ficheros(n_inodo,0,18,buffer) < 18) {
		printf("OBALLA...\n");
	}
	buffer[18] = '\0';
	inodo in;
	if (leer_inodo(n_inodo,&in) == -1) {
		return -1;
	}
	printf("n_inodo %d inodo n_inodo %d siguiente %d\n",n_inodo,in.n_inodo,in.punteros[0]);
	printf("BUFFER: %s\n",buffer);
	if (desmontar_basico() == -1) {
		return -1;
	}
	printf("glosdag\n");
	return 0;
}
