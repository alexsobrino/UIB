#include <string.h>
#include "def.h"
#include "bloques.h"
#include "ficheros_basico.h"

int main (int argc, char **argv) 
{
	if (argc != 3) {
		printf ("ERROR: usar %s nombre_fichero numero_bloques.\n",argv[0]);
		return -1;
	}
	printf("(mi_mkfs.c) nº de argumentos: %d  argumento 1: %s  argumento 2: %d\n",argc,argv[1],argv[2]);
	if (montar_bloques(argv[1]) == -1) {
		return -1;
	}
	int i;
	char buffer[TAM_BLOQUE];
	memset(buffer,0,TAM_BLOQUE);
	for (i=0;i<atoi(argv[2]);i++) {
		if (escribir_bloque(i,buffer) == -1) {
			return -1;
		}
		printf("(mi_mkfs.c) inicializado a 0 el bloque: %d\n",i);
	}
	superbloque sb;
	sb.tam_bloque = TAM_BLOQUE;
	sb.n_bloques = atoi(argv[2]);
	sb.n_bloque_sb = 0;
	sb.n_bloque_mb = 1;
	sb.n_bloques_mb = div_c(sb.n_bloques,TAM_BLOQUE*8);
	sb.n_bloque_inodos = sb.n_bloques_mb + sb.n_bloque_mb;
	sb.n_bloques_inodos = div_c(sb.n_bloques,div_c(TAM_BLOQUE,sizeof(inodo)));
	sb.n_bloque_datos = sb.n_bloque_inodos + sb.n_bloques_inodos;
	sb.l_inodos = 1;
	memset(buffer,0,TAM_BLOQUE);
	memcpy(buffer,&sb,sizeof(superbloque));
	if (escribir_bloque(0,buffer) == -1) {
		return -1;
	}
	printf("(mi_mkfs.c) tamaño de bloques: %d\n",TAM_BLOQUE);
	printf("(mi_mkfs.c) número de bloques: %d\n",sb.n_bloques);
	printf("(mi_mkfs.c) bloque del SB: %d\n",sb.n_bloque_sb);
	printf("(mi_mkfs.c) bloque inicial del MB: %d\n",sb.n_bloque_mb);
	printf("(mi_mkfs.c) bloques para el MB: %d\n",sb.n_bloques_mb);
	printf("(mi_mkfs.c) bloque inicial de inodos: %d\n",sb.n_bloque_inodos);
	printf("(mi_mkfs.c) bloques para inodos: %d\n",sb.n_bloques_inodos);
	printf("(mi_mkfs.c) bloque inicial de datos: %d\n",sb.n_bloque_datos);
	printf("(mi_mkfs.c) primer inodo libre: %d\n",sb.l_inodos);
	inodo in;
	in.tipo = '0';
	in.longitud = 0;
	for (i=0;i<MAX_PTROS;i++) {
		in.punteros[i] = -1;
	}
	int j;
	int n_inodo_tmp = 0;
	for (i=sb.n_bloque_inodos;i<sb.n_bloque_datos;i++) {
		memset(buffer,0,TAM_BLOQUE);
		for (j=0;j<(TAM_BLOQUE/sizeof(inodo));j++) {
			in.f_creacion = in.f_modificacion = in.f_ultimoacceso = time(NULL);
			in.n_inodo = n_inodo_tmp;
			if (in.n_inodo != sb.n_bloques-1) {
				in.punteros[0] = in.n_inodo+1;
			} else if (in.n_inodo = 0) {
				in.punteros[0] = -1;
				in.tipo = '2';
			} else {
				in.punteros[0] = -1;
			}
			memcpy(&buffer[j*sizeof(inodo)],&in,sizeof(inodo));
			/*if (in.punteros[0] == -1) {
				break;
			}*/
			n_inodo_tmp++;
		}
		if (escribir_bloque(i,buffer) == -1) {
			return -1;
		}
	}
	/*for (i=0;i<sb.n_bloques;i++) {
		memset(buffer,0,TAM_BLOQUE);
		in.f_creacion = in.f_modificacion = in.f_ultimoacceso = time(NULL);
		in.n_inodo = i;
		if (in.n_inodo != sb.n_bloques-1) {
			in.punteros[0] = i+1;
		} else {
			in.punteros[0] = -1;
		}
		memcpy(buffer+((i*sizeof(inodo))%TAM_BLOQUE),&in,sizeof(inodo));
		printf("(mi_mkfs.c) escrito inodo %d siguiente %d en la posicion %d del buffer.\n",in.n_inodo,in.punteros[0],(i*sizeof(inodo))%TAM_BLOQUE);
		if (((i*sizeof(inodo))%TAM_BLOQUE == TAM_BLOQUE-sizeof(inodo)) || (in.n_inodo == sb.n_bloques-1)) {
			if (escribir_bloque(sb.n_bloque_inodos+((i*sizeof(inodo))/TAM_BLOQUE),buffer) == -1) {
				return -1;
			}
			printf("(mi_mkfs.c) escritos inodos en bloque %d.\n\n",sb.n_bloque_inodos+((i*sizeof(inodo))/TAM_BLOQUE));
			memset(buffer,0,TAM_BLOQUE);
		}
	}*/
	if (desmontar_bloques() == -1) {
		return -1;
	}
	return 0;
}

/*int main (int argc, char **argv)
{
	// Inicio
	if (argc != 3) {
		printf("ERROR: usar %s nombre_fichero numero_bloques.\n",argv[0]);
	}
	int dsf = montar_basico(argv[1]);
	
	// Inicializamos todo a ceros
	int i;
	char buffer[TAM_BLOQUE];
	memset(buffer,0,TAM_BLOQUE);
	for (i=0;i<atoi(argv[2]);i++) {
		escribir_bloque(dsf,i,buffer);
	}
	
	// Inicialización del superbloque
	//superbloque sb;
	sb.tam_bloque = TAM_BLOQUE;
	sb.n_bloques = atoi(argv[2]);
	sb.n_bloque_sb = 0;
	sb.n_bloque_mb = 1;
	sb.n_bloques_mb = div_c(sb.n_bloques,TAM_BLOQUE*8);
	sb.n_bloque_inodo = sb.n_bloques_mb + 1;
	sb.n_bloques_inodos = div_c(sb.n_bloques,div_c(TAM_BLOQUE,sizeof(inodo)));
	sb.n_bloque_datos = sb.n_bloque_inodo + sb.n_bloques_inodos + 1;
	printf("[SB] Tamaño de bloque: %d\n",sb.tam_bloque);
	printf("[SB] Número de bloques: %d\n",sb.n_bloques);
	printf("[SB] Número de bloques MB: %d\n",sb.n_bloques_mb);
	printf("[SB] Número de bloques Inodos: %d\n",sb.n_bloques_inodos);
	memset(buffer,0,TAM_BLOQUE);
	memcpy(buffer,&sb,sizeof(superbloque));
	escribir_bloque(dsf,0,buffer);
	
	// Inicialización del mapa de bits
	memset(buffer,0,TAM_BLOQUE);
	for (i=0;i<sb.n_bloques_mb;i++) {
		escribir_bloque(dsf,i+1,buffer);
	}
	// Inicialización de inodos
	inodo in;
	in.tipo = '0';
	memset(buffer,0,TAM_BLOQUE);
	for (i=0;i<(TAM_BLOQUE/sizeof(inodo));i++) {
		memcpy(buffer+(i*sizeof(inodo)),&in,sizeof(inodo));
	}
	for (i=0;i<sb.n_bloques_inodos;i++) {
		escribir_bloque(dsf,i+sb.n_bloques_mb+1,buffer);
	}
	// Final
	desmontar_basico(dsf);
}*/
