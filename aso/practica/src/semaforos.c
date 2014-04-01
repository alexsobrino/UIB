#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

static int pid = -1;
static int rec = 0;

int crear_sem (int num)
{
	int s;
	s = semget(IPC_PRIVATE,num,IPC_CREAT|0600);
	if (s<0) {
		printf("(semaforos.c) ERROR: imposible crear semáforo.\n");
		exit(1);
	}
	return s;
}

void eliminar_sem (int s)
{
	semctl(s,0,IPC_RMID,0);
}

void esperar_sem (int s, int pos, int flag) 
{
	struct sembuf sbuf;
	if (getpid() != pid) {
		pid = getpid();
		sbuf.sem_num = pos;
		sbuf.sem_op = -1;
		sbuf.sem_flg = flag;
		semop(s,&sbuf,1);
		rec = 0;
	} else {
		rec++;
	}
}

void senalizar_sem (int s, int pos) 
{
	struct sembuf sbuf;
	if (rec == 0) {
		sbuf.sem_num = pos;
		sbuf.sem_op = 1;
		sbuf.sem_flg = 0;
		semop(s,&sbuf,1);
		pid = -1;
	} else {
		rec--;
	}
}

void esperar_cero (int s, int pos)
{
	struct sembuf sbuf;
	sbuf.sem_num = pos;
	sbuf.sem_op = 0;
	sbuf.sem_flg = 0;
	semop(s,&sbuf,1);
}

int inicializar_le (void)
{
	int s;
	int sarg_array[] = {0,0};
	s = crear_sem(2);
	// [0] = escritores
	// [1] = lectores
	semctl(s,0,SETALL,sarg_array);
	return s;
}

int inicializar_mutex (int valor) 
{
	int s;
	s = crear_sem(1);
	semctl(s,0,SETVAL,valor);
	return s;
}

void entrada_lectores (int s)
{
	struct sembuf sbuf[2];
	sbuf[0].sem_num = 0;
	sbuf[0].sem_op = 0;
	sbuf[0].sem_flg = 0;
	sbuf[1].sem_num = 1;
	sbuf[1].sem_op = 1;
	sbuf[1].sem_flg = 0;
	semop(s,sbuf,2);
}

void salida_lectores (int s)
{
	esperar_sem(s,1,0);
}

void entrada_escritores (int s, int m)
{
	struct sembuf sbuf[2];
	senalizar_sem(s,0);
	esperar_cero(s,1);
	esperar_sem(m,0,SEM_UNDO);
}

void salida_escritores (int s, int m)
{
	senalizar_sem(m,0);
	esperar_sem(s,0,0);
}
