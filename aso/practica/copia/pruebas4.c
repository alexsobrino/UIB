#include "def.h"
#include "directorio.h"
int main (void)
{
	if (montar("disco.imagen") == -1) {
		return -1;
	}
	int i;
	char nombre[MAX_NOMBRE];
	for (i=0;i<10;i++) {
		sprintf(nombre,"/cliente-%d.dat",i);
		if (mi_create_directorio(nombre) == -1) {
			return -1;
		}
	}
	if (desmontar() == -1) {
		return -1;
	}
	return 0;
}
