all:			practica
practica:		lectores_escritores.o semaforos.o practica.c
			gcc -o practica practica.c lectores_escritores.o semaforos.o
lectores_escritores.o:	lectores_escritores.c lectores_escritores.h
			gcc -c lectores_escritores.c
semaforos.o:		semaforos.c semaforos.h
			gcc -c semaforos.c

