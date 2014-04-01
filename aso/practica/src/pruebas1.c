#include <string.h>
#include "bloques.h"
#include "ficheros_basico.h"

int main (void) 
{
	int dsf;
	dsf = montar_basico("disco.imagen");
	char buffer[TAM_BLOQUE];
	int i,j;
	for(i=0;i<100;i++) {
		memset(buffer,0,TAM_BLOQUE);
		escribir_bloque(dsf,i,buffer);
	}
	memset(buffer,0,TAM_BLOQUE);
	buffer[0] = 'a';
	escribir_bloque(dsf,1,buffer);
	memset(buffer,0,TAM_BLOQUE);
	leer_bloque(dsf,1,buffer);
	printf("buffer: %s",buffer);
	desmontar_basico(dsf);
	
	printf("Tamaño inodo: %d",sizeof(inodo));
}
