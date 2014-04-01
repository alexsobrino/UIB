typedef struct {
	int tipo;
	int tamano;
	int posicion;
	char nombre[MAX_NOMBRE];
	char info[TAM_BLOQUE];
} c_mensaje;

typedef struct {
	long pid;
	c_mensaje contenido;
} mensaje;

int crear_cola (int);
void eliminar_cola (int);
