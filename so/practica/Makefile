all:			practica
practica:		src/lectores_escritores.o src/semaforos.o src/practica.c
			gcc -o practica src/practica.c src/lectores_escritores.o src/semaforos.o
lectores_escritores.o:	src/lectores_escritores.c src/lectores_escritores.h
			gcc -c src/lectores_escritores.c
semaforos.o:		src/semaforos.c src/semaforos.h
			gcc -c src/semaforos.c

