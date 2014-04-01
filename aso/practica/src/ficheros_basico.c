#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include "def.h"
#include "bloques.h"
#include "ficheros_basico.h"
#include "semaforos.h"

static superbloque *sb;
static char *mb;
int shmd_mb,shmd_sb;
int mutex;

int leer_sb_sf (void)
{
	char buffer[TAM_BLOQUE];
	esperar_sem(mutex,0,0);
	if (leer_bloque(0,buffer) == -1) {
		printf("(ficheros_basico.c|leer_sb_sf) ERROR: error al leer el bloque.\n");
		return -1;
	}
	memcpy(sb,buffer,sizeof(superbloque));
	senalizar_sem(mutex,0);
	if (DEBUG == 1) {
		printf("(ficheros_basico.c|leer_sb_sf) DEBUG: superbloque leído del sistema de ficheros.\n");
	}
	return 0;
}

int escribir_sb_sf ()
{
	char buffer[TAM_BLOQUE];
	memset(buffer,0,TAM_BLOQUE);
	esperar_sem(mutex,0,0);
	memcpy(buffer,sb,sizeof(superbloque));
	if (escribir_bloque(0,buffer) == -1) {
		printf("(ficheros_basico.c|escribir_sb_sf) ERROR: error al escribir el bloque.\n");
		senalizar_sem(mutex,0);
		return -1;
	} else {
		senalizar_sem(mutex,0);
		if (DEBUG == 1) {
			printf("(ficheros_basico.c|escribir_sb_sf) DEBUG: superbloque leído del sistema de ficheros.\n");
		}
		return 0;
	}
}

int leer_sb (superbloque *sb_a)
{
	memcpy(sb_a,sb,sizeof(superbloque));
	if (DEBUG == 1) {
		printf("(ficheros_basico.c|leer_sb) DEBUG: superbloque leído de memoria.\n");
	}
	return 0;
}

int escribir_sb (superbloque *sb_a)
{
	memcpy(sb,sb_a,sizeof(superbloque));
	/*if (escribir_sb_sf() == -1) {
		printf("(ficheros_basico.c|escribir_sb) ERROR: error al escribir el superbloque en disco.\n");
		return -1;
	}*/
	if (DEBUG == 1) {
		printf("(ficheros_basico.c|escribir_sb) DEBUG: superbloque escrito en memoria y disco.\n");
	}
	return 0;
}

int crear_shm (void)
{
	// memoria compartida para el superbloque
	shmd_sb = shmget(IPC_PRIVATE,sizeof(superbloque),0666);
	if (shmd_sb < 0) {
		printf("(ficheros_basico.c|crear_shm) ERROR: Imposible crear la memoria compartida.\n");
		return -1;
	}
	sb = (superbloque *) shmat(shmd_sb,NULL,0);
	if (DEBUG == 1) {
		printf("(ficheros_basico.c|crear_shm) DEBUG: memoria compartida para el superbloque creada [id: %d].\n",shmd_sb);
	}
	// inicializo la variable compartida sb
	leer_sb_sf();
	// memoria compartida para el mapa de bits
	shmd_mb = shmget(IPC_PRIVATE,div_c(sb->n_bloques,8*TAM_BLOQUE),0666);
	if (shmd_mb < 0) {
		printf("(ficheros_basico.c|crear_shm) ERROR: Imposible crear la memoria compartida.\n");
		return -1;
	}
	mb = (char *) shmat(shmd_mb,NULL,0);
	if (DEBUG == 1) {
		printf("(ficheros_basico.c|crear_shm) DEBUG: memoria compartida para el mapa de bits creada [id: %d].\n",shmd_mb);
	}
	// inicializo el mapa de bits compartido
	int i;
	char buffer[TAM_BLOQUE];
	for (i=0;i<sb->n_bloques_mb;i++) {
		memset(buffer,0,TAM_BLOQUE);
		leer_bloque(i+sb->n_bloque_mb,buffer);
		memcpy(mb+(i*TAM_BLOQUE),buffer,TAM_BLOQUE);
	}
	return 0;
}

void remover_shm ()
{
	shmctl(shmd_sb,IPC_RMID,0);
	shmctl(shmd_mb,IPC_RMID,0);
	if (DEBUG == 1) {
		printf("(ficheros_basico.c|remover_shm) DEBUG: memoria compartida eliminada.\n");
	}
}

int montar_basico (char *nombre)
{
	if (montar_bloques(nombre) == -1) {
		printf("(ficheros_basico.c|montar_basico) ERROR: problema al montar el sistema de ficheros [nombre: %s].\n",nombre);
		return -1;
	}
	if (DEBUG == 1) {
		printf("(ficheros_basico.c|montar_basico) DEBUG: sistema de ficheros montado.\n");
	}
	if (crear_shm() == -1) {
		printf("(ficheros_basico.c|montar_basico) ERROR: problema al crear la memoria compartida.\n");
		return -1;
	}
	mutex = inicializar_mutex(1);
	//le = inicializar_le();
	if (DEBUG == 1) {
		printf("(ficheros_basico.c|montar_basico) DEBUG: memoria compartida creada.\n");
	}
	return 0;
}

int desmontar_basico (void)
{
	escribir_sb_sf();
	escribir_mb_sf();
	if (desmontar_bloques() == -1) {
		printf("(ficheros_basico.c|desmontar_basico) ERROR: problema al desmontar el sistema de ficheros.\n");
		return -1;
	}
	remover_shm();
	eliminar_sem(mutex);
	//eliminar_sem(le);
	if (DEBUG == 1) {
		printf("(ficheros_basico.c|desmontar_basico) DEBUG: superbloque y mapa de bits escritos en disco, sistema de ficheros desmontado y memoria compartida eliminada correctamente.\n");
	}
	return 0;
}

int leer_inodo (int n_inodo, inodo *in) 
{
	int n_bloque = sb->n_bloque_inodos + (n_inodo/(TAM_BLOQUE/sizeof(inodo)));
	int n_inodo_bloque = n_inodo % (TAM_BLOQUE/sizeof(inodo));
	char buffer[TAM_BLOQUE];
	memset(buffer,0,TAM_BLOQUE);
	//entrada_lectores(le);
	esperar_sem(mutex,0,0);
	if (leer_bloque(n_bloque,buffer) == -1) {
		printf("(ficheros_basico.c|leer_inodo) ERROR: imposible leer el inodo %d en el bloque %d.\n",n_inodo,n_bloque);
		return -1;
	}
	memcpy(in,&buffer[n_inodo_bloque*sizeof(inodo)],sizeof(inodo));
	senalizar_sem(mutex,0);
	//salida_lectores(le);
	if (DEBUG == 1) {
		printf("((ficheros_basico.c|leer_inodo) DEBUG: leido inodo %d [n_inodo: %d  siguiente: %d  tipo: %c  longitud: %d].\n",n_inodo,in->n_inodo,in->punteros[0],in->tipo,in->longitud);
	}
	return 0;
}

int escribir_inodo (int n_inodo, inodo *in)
{
	int n_bloque = sb->n_bloque_inodos + (n_inodo/(TAM_BLOQUE/sizeof(inodo)));
	int n_inodo_bloque = n_inodo % (TAM_BLOQUE/sizeof(inodo));
	char buffer[TAM_BLOQUE];
	memset(buffer,0,TAM_BLOQUE);
	//entrada_escritores(le);
	esperar_sem(mutex,0,0);
	if (leer_bloque(n_bloque,buffer) == -1) {
		printf("(ficheros_basico.c|escribir_inodo) ERROR: imposible leer el bloque %d.\n",n_bloque);
		return -1;
	}
	memcpy(&buffer[n_inodo_bloque*sizeof(inodo)],in,sizeof(inodo));
	if (escribir_bloque(n_bloque,buffer) == -1) {
		printf("(ficheros_basico.c|escribir_inodo) ERROR: imposible escribir el inodo %d en el bloque %d.\n",n_inodo,n_bloque);
		return -1;
	}
	//salida_escritores(le);
	senalizar_sem(mutex,0);
	if (DEBUG == 1) {
		printf("(ficheros_basico.c|escribir_inodo) DEBUG: escrito inodo %d en el bloque %d.\n",n_inodo,n_bloque);
	}
	return 0;
}

int liberar_inodo (int n_inodo)
{
	inodo in;
	int i;
	if (leer_inodo(n_inodo,&in) == -1) {
		printf("(ficheros_basico.c|liberar_inodo) ERROR: imposible leer inodo %d.\n",n_inodo);
		return -1;
	}
	for (i=0;i<MAX_PTROS;i++) {
		if (in.punteros[i] > 0) {
			if (escribir_mb(in.punteros[i],0) == -1) {
				printf("(ficheros_basico.c|liberar_inodo) ERROR: imposible liberar en el mapa de bits el bloque %d.\n",in.punteros[i]);
				return -1;
			}
			in.punteros[i] = -1;
		}
	}
	in.tipo = '0';
	in.f_modificacion = time(NULL);
	in.f_ultimoacceso = time(NULL);
	in.longitud = 0;
	esperar_sem(mutex,0,0);	
	in.punteros[0] = sb->l_inodos;
	sb->l_inodos = n_inodo;
	if (DEBUG == 1) {
		printf("(ficheros_basico.c|liberar_inodo) DEBUG: inodo %d liberado [n_inodo: %d  siguiente: %d].\n",n_inodo,in.n_inodo,in.punteros[0]);
	}
	if (escribir_inodo(n_inodo,&in) == -1) {
		printf("(ficheros_basico.c|liberar_inodo) ERROR: imposible escribir el inodo %d.\n",n_inodo);
		return -1;
	} 
	/*if (escribir_sb_sf() == -1) {
		printf("(ficheros_basico.c|liberar_inodo) ERROR: imposible escribir el superbloque.\n");
		return -1;
	}*/
	return 0;
}

int leer_mb (int n_bloque)
{
	int n_bit = n_bloque % 8;
	esperar_sem(mutex,0,0);
	if ((*(mb+(n_bloque/8)) & (128>>n_bit)) != 0) {
		senalizar_sem(mutex,0);
		if (DEBUG == 1) {
			printf("(ficheros_basico.c|leer_mb) DEBUG: bloque %d ocupado.\n",n_bloque);
		}
		return 1;
	} else {
		senalizar_sem(mutex,0);
		if (DEBUG == 1) {
			printf("(ficheros_basico.c|leer_mb) DEBUG: bloque %d libre.\n",n_bloque);
		}
		return 0;
	}
}

int escribir_mb (int n_bloque, int valor)
{
	int n_bit = n_bloque % 8;
	if (valor == 0) {
		if (DEBUG == 1) {
			printf("(ficheros_basico.c|escribir_mb) DEBUG: liberando bloque.\n",n_bloque);
		}
		esperar_sem(mutex,0,0);
		*(mb+(n_bloque/8)) = *(mb+(n_bloque/8)) & (~(128>>n_bit));
		senalizar_sem(mutex,0);
	} else if (valor == 1) {
		if (DEBUG == 1) {
			printf("(ficheros_basico.c|escribir_mb) DEBUG: ocupando bloque.\n",n_bloque);
		}
		esperar_sem(mutex,0,0);
		*(mb+(n_bloque/8)) = *(mb+(n_bloque/8)) | (128>>n_bit);
		senalizar_sem(mutex,0);
	} else {
		printf("(ficheros_basico.c|escribir_mb) ERROR: valor %d imposible.\n",valor);
		return -1;
	}
	/*if (escribir_mb_sf() == -1) {
		printf("(ficheros_basico.c|escribir_mb) ERROR: imposible escribir el mapa de bits en el disco.\n");
		return -1;
	}*/
	return 0;
}

int escribir_mb_sf (void)
{
	int i;
	esperar_sem(mutex,0,0);
	for (i=0;i<sb->n_bloques_mb;i++) {
		if (escribir_bloque(i+sb->n_bloque_mb,&mb[i*TAM_BLOQUE]) == -1) {
			printf("(ficheros_basico.c|escribir_mb_sf) ERROR: imposible escribir bloque %d.\n",i+sb->n_bloque_mb);
			senalizar_sem(mutex,0);
			return -1;
		}
		if (DEBUG == 1) {
			printf("(ficheros_basico.c|escribir_mb_sf) DEBUG: escrito bloque %d correspondiente al mapa de bits.\n",i+sb->n_bloque_mb);
		}
	}
	senalizar_sem(mutex,0);
	return 0;
}

int reservar_inodo (void)
{
	inodo in;
	int n_inodo;
	n_inodo = sb->l_inodos;
	if (n_inodo == -1) {
		printf("(ficheros_basico.c|reservar_inodo) ERROR: todos los inodos ocupados.\n");
	}
	if (leer_inodo(n_inodo,&in) == -1) {
		printf("(ficheros_basico.c|reservar_inodo) ERROR: imposible leer inodo %d.\n",n_inodo);
		return -1;
	}
	sb->l_inodos = in.punteros[0];
	in.punteros[0] = -1;
	in.f_ultimoacceso = in.f_modificacion = time(NULL);
	if (escribir_inodo(n_inodo,&in) == -1) {
		return -1;
	}
	if (DEBUG == 1) {
		printf("(ficheros_basico.c|reservar_inodo) DEBUG: inodo %d reservado.\n",in.n_inodo);
	}
	//escribir_sb_sf();
	return n_inodo;
}

int reservar_bloque (void)
{
	int n_bloque;
	for (n_bloque=sb->n_bloque_datos;n_bloque<sb->n_bloques;n_bloque++) {
		if (DEBUG == 1) {
			printf("(ficheros_basico.c|reservar_bloque) DEBUG: analizando bloque %d.\n",n_bloque);
		}
		if (leer_mb(n_bloque) == 0) {
			if (escribir_mb(n_bloque,1) == -1) {
				printf("(ficheros_basico.c|reservar_bloque) ERROR: imposible ocupar el bloque %d.\n",n_bloque);
			}
			if (DEBUG == 1) {
				printf("(ficheros_basico.c|reservar_bloque) DEBUG: bloque reservado %d.\n",n_bloque);
			}
			return n_bloque;
		}
	}
	printf("(ficheros_basico.c|reservar_bloque) ERROR: sistema de ficheros lleno.\n");
	return -1;
}

int liberar_bloque (int n_bloque)
{
	if (escribir_mb(n_bloque,0) == -1) {
		printf("(ficheros_basico.c|liberar_bloque) ERROR: imposible liberar el bloque %d.\n",n_bloque);
		return -1;
	} else {
		if (DEBUG == 1) {
			printf("(ficheros_basico.c|liberar_bloque) DEBUG: liberado bloque %d.\n",n_bloque);
		}
		return 0;
	}
}

int div_c (int num, int den)
{
	if (num % den == 0) {
		return num/den;
	} else {
		return (num/den)+1;
	}
}
