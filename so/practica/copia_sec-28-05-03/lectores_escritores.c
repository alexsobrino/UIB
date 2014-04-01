# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/sem.h>

static int lectores_escritores;
static int mutex;

int nuevo_sem2(int llave, int num)
{
	int s;

	s = semget(llave, num, IPC_CREAT|0600);
	if (s < 0 ) {
		puts("Error creando semaforos");
		exit(0);
	}
	return s;
}


void esperar_sem2(int s, int pos, int flag)
{
	struct sembuf sbuf;

	sbuf.sem_num = pos;
	sbuf.sem_op = -1;
	sbuf.sem_flg = flag;
	semop(s, &sbuf, 1);
}


void senalizar_sem2(int s, int pos)
{
	struct sembuf sbuf;

	sbuf.sem_num = pos;
	sbuf.sem_op = 1;
	sbuf.sem_flg = 0;
	semop(s, &sbuf, 1);
}

void esperar_cero2(int s, int pos)
{
	struct sembuf sbuf;

	sbuf.sem_num = pos;
	sbuf.sem_op = 0;
	sbuf.sem_flg = 0;
	semop(s, &sbuf, 1);
}



void inicializar_le(void)
{
	int clave = 50; // Uso 50 sólo para ejemplos
	int sarg_array[] = {0, 0};
	int sarg = 1;

	lectores_escritores = nuevo_sem2(clave, 2);
	// [0] = escritores
	// [1] = lectores
	semctl(lectores_escritores, 0, SETALL, sarg_array);

	mutex = nuevo_sem2(clave+1, 1);
	semctl(mutex, 0, SETVAL, sarg);

}

void entrada_lectores(void)
{
	struct sembuf sbuf[2];
	// [0] = escritores
	// [1] = lectores

	// wait for zero escritores
	sbuf[0].sem_num = 0;
	sbuf[0].sem_op = 0;
	sbuf[0].sem_flg = 0;

	// señalizar lectores
	sbuf[1].sem_num = 1;
	sbuf[1].sem_op = 1;
	sbuf[1].sem_flg = 0;

	semop(lectores_escritores, sbuf, 2);
}

void salida_lectores(void)
{
	esperar_sem2(lectores_escritores, 1, 0);
}

void entrada_escritores(void)
{
	struct sembuf sbuf[2];
	// [0] = escritores
	// [1] = lectores
	// señalizar escritores
	senalizar_sem2(lectores_escritores, 0);
	// esperar 0 de lectores
	esperar_cero2(lectores_escritores, 1);
	esperar_sem2(mutex, 0, SEM_UNDO);
}

void salida_escritores(void)
{
	senalizar_sem2(mutex, 0);
	esperar_sem2(lectores_escritores, 0, 0);
}

