#include "def.h"

typedef struct {
	char nombre[MAX_NOMBRE];
	int n_inodo;
} entrada;

int montar (char *);
int desmontar ();
int mi_create_directorio (char *);
int mi_read_directorio (char *, int, int, char *);
int mi_write_directorio (char *, int, int, char *);
int mi_unlink_directorio (char *);
int mi_ls_directorio (char *, char *);
int mi_mkdir_directorio (char *);
int mi_stat_directorio (char *, char *);
int mi_cat_directorio (char *, int, char *);
