# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/sem.h>

static int lectores_escritores;
static int mutex;

int nuevo_sem(int llave, int num)
{
	int s;

	s = semget(llave, num, IPC_CREAT|0600);
	if (s < 0 ) {
		puts("Error creando semaforos");
		exit(0);
	}
	return s;
}

void eliminar_sem(int s)
{
	semctl(s, 0, IPC_RMID, 0);
}

void inicializar_sem(int s, int valor)
{
	semctl(s, 0, SETVAL, valor);
}

void esperar_sem(int s)
{
	struct sembuf sbuf;

	sbuf.sem_num = 0;
	sbuf.sem_op = -1;
	//sbuf.sem_flg = SEM_UNDO;
	sbuf.sem_flg = 0;
	semop(s, &sbuf, 1);
}


void senalizar_sem(int s)
{
	struct sembuf sbuf;

	sbuf.sem_num = 0;
	sbuf.sem_op = 1;
	sbuf.sem_flg = 0;
	semop(s, &sbuf, 1);
}
