#include <time.h>

typedef struct {
	int n_inodo;
	time_t f_creacion;
	time_t f_modificacion;
	time_t f_ultimoacceso;
	int longitud;
} estat;

int mi_write_ficheros (int, int, int, char *);
int mi_read_ficheros (int, int, int, char*);
int mi_truncar_ficheros (int, int);
int mi_stat_ficheros (int, estat *);
