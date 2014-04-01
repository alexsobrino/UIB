#include "def.h"
#include "directorio.h"
int main (void)
{
	if (montar("disco.imagen") == -1) {
		return -1;
	}
	int i;
	char nombre[MAX_NOMBRE];
	//for (i=0;i<10;i++) {
	//	sprintf(nombre,"/cliente-%d.dat",i);
		if (mi_create_directorio("/alesito.dat") == -1) {
			return -1;
		}
	//}
	if (mi_write_directorio("/alesito.dat",0,20,"HOLA MANOLO COMO ESTAS YO MUY BIEN GEGEGE OBAAAALLA") < 20) {
		printf("LA CAGAMOS BURLANKASTER\n");
	}
	memset(nombre,'\0',MAX_NOMBRE);
	int tmp = mi_read_directorio("/alesito.dat",1,18,nombre);
	printf("BUFFY [%d] %s\n",tmp,nombre);
	if (desmontar() == -1) {
		return -1;
	}
	return 0;
}
