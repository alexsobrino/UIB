#include <time.h>
#include "def.h"

typedef struct {
	int tam_bloque;
	int n_bloques;
	int n_bloque_sb;
	int n_bloque_mb;
	int n_bloques_mb;
	int n_bloque_inodos;
	int n_bloques_inodos;
	int n_bloque_datos;
	int l_inodos;
} superbloque;

typedef struct {
	char tipo; // 0: libre  1: fichero  2: directorio
	time_t f_creacion;
	time_t f_modificacion;
	time_t f_ultimoacceso;
	int longitud;
	int n_inodo;
	int punteros[MAX_PTROS];
} inodo;

int leer_sb_sf (void);
int escribir_sb_sf (void);
int leer_sb (superbloque *);
int escribir_sb (superbloque *);
int montar (char *);
int desmontar (void);
int leer_inodo (int, inodo *);
int escribir_inodo (int, inodo *);
int leer_mb (int);
int escribir_mb (int, int);
int escribir_mb_sf (void);
int reservar_inodo (void);
int reservar_bloque (void);
int liberar_bloque (int);
int div_c (int, int);
