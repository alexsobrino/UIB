#include "ficheros_basico.h"

int main (void) {
	if (montar_basico("disco.imagen") == -1) {
		return -1;
	}
	inodo in;
	if (leer_inodo(13,&in) == -1) {
		return -1;
	}
	printf("(pruebas2.c) inodo %d siguiente %d tipo %c\n",in.n_inodo,in.punteros[0],in.tipo);
	if (leer_inodo(97,&in) == -1) {
		return -1;
	}
	printf("(pruebas2.c) inodo %d siguiente %d tipo %c\n",in.n_inodo,in.punteros[0],in.tipo);
	superbloque sb_a;
	leer_sb(&sb_a);
	printf("primer inodo libre: %d\n",sb_a.l_inodos);
	printf("numero de bloques: %d\n",sb_a.n_bloques);
	int n_inodo1 = reservar_inodo();
	int n_inodo2 = reservar_inodo();
	printf("inodo reservado: %d\n",n_inodo1);
	printf("inodo reservado: %d\n",n_inodo2);
	if (liberar_inodo(n_inodo1) == -1) {
		return -1;
	}
	if (liberar_inodo(n_inodo2) == -1) {
		return -1;
	}
	if (leer_inodo(n_inodo1,&in) == -1) {
		return -1;
	}
	printf("(pruebas2.c) inodo %d siguiente %d tipo %c\n",in.n_inodo,in.punteros[0],in.tipo);
	if (leer_inodo(n_inodo2,&in) == -1) {
		return -1;
	}
	printf("(pruebas2.c) inodo %d siguiente %d tipo %c\n",in.n_inodo,in.punteros[0],in.tipo);
	leer_sb(&sb_a);
	printf("primer inodo libre: %d\n",sb_a.l_inodos);
	int i;
	for (i=0;i<sb_a.n_bloques;i++) {
		printf("	(pruebas2.c) reservo el bloque %d.\n",reservar_bloque());
	}
	/*printf("bloque 72: %d.\n",leer_mb(72));
	if (escribir_mb(72,1) == -1) {
		return -1;
	}
	printf("bloque 72: %d.\n",leer_mb(72));*/
	if (desmontar_basico() == -1) {
		return -1;
	}
}
