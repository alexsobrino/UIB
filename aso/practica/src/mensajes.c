#include <sys/ipc.h>

int crear_cola (int id) 
{
	int msg;
	if ((msg = msgget(id,0600|IPC_CREAT)) < 0) {
		printf("(mensajes.c) ERROR: imposible crear la cola.\n");
		return -1;
	}
	return msg;
}

void eliminar_cola (int id) {
	int msg;
	if ((msg = msgctl(id,IPC_RMID,0)) < 0) {
		printf("(mensajes.c) ERROR: imposible eliminar la cola [id: %d].\n",id);
	}
}

