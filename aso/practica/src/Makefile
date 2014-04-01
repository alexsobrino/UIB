# General
all:			mi_mkfs simulador agente mi_ls mi_cat mi_rm mi_stat
clean:
			rm -f *.o
			
# Binarios
mi_mkfs:		mi_mkfs.c bloques.o ficheros_basico.o semaforos.o
			gcc -o mi_mkfs mi_mkfs.c bloques.o ficheros_basico.o semaforos.o
simulador:		simulador.c bloques.o ficheros_basico.o ficheros.o directorio.o cliente.o cliente_lib.o mensajes.o semaforos.o servidor.o
			gcc -o simulador simulador.c bloques.o ficheros_basico.o ficheros.o directorio.o cliente.o cliente_lib.o mensajes.o semaforos.o servidor.o
agente: 		agente.c bloques.o ficheros_basico.o ficheros.o directorio.o cliente.o cliente_lib.o mensajes.o semaforos.o servidor.o
			gcc -o agente agente.c bloques.o ficheros_basico.o ficheros.o directorio.o cliente.o cliente_lib.o mensajes.o semaforos.o servidor.o
mi_ls: 			mi_ls.c bloques.o ficheros_basico.o ficheros.o directorio.o semaforos.o 
			gcc -o mi_ls mi_ls.c bloques.o ficheros_basico.o ficheros.o directorio.o semaforos.o 
mi_cat:			mi_cat.c bloques.o ficheros_basico.o ficheros.o directorio.o semaforos.o
			gcc -o mi_cat mi_cat.c bloques.o ficheros_basico.o ficheros.o directorio.o semaforos.o
mi_rm:			mi_rm.c bloques.o ficheros_basico.o ficheros.o directorio.o semaforos.o
			gcc -o mi_rm mi_rm.c bloques.o ficheros_basico.o ficheros.o directorio.o semaforos.o
mi_stat: 		mi_stat.c bloques.o ficheros_basico.o ficheros.o directorio.o semaforos.o
			gcc -o mi_stat mi_stat.c bloques.o ficheros_basico.o ficheros.o directorio.o semaforos.o

# Pruebas
pruebas1:		pruebas1.c bloques.o ficheros_basico.o
			gcc -o pruebas1 pruebas1.c ficheros_basico.o bloques.o
pruebas2:		pruebas2.c bloques.o ficheros_basico.o
			gcc -o pruebas2 pruebas2.c ficheros_basico.o bloques.o
pruebas3:		pruebas3.c bloques.o ficheros_basico.o ficheros.o
			gcc -o pruebas3 pruebas3.c ficheros.o ficheros_basico.o bloques.o
pruebas4: 		pruebas4.c bloques.o ficheros_basico.o ficheros.o directorio.o
			gcc -o pruebas4 pruebas4.c bloques.o ficheros_basico.o ficheros.o directorio.o

# Objetos
bloques.o: 		bloques.c bloques.h
			gcc -c bloques.c
ficheros_basico.o: 	ficheros_basico.c ficheros_basico.h
			gcc -c ficheros_basico.c
ficheros.o:		ficheros.c ficheros.h
			gcc -c ficheros.c 
directorio.o:		directorio.c directorio.h
			gcc -c directorio.c
cliente.o: 		cliente.c cliente.h
			gcc -c cliente.c
cliente_lib.o: 		cliente_lib.c cliente_lib.h
			gcc -c cliente_lib.c
mensajes.o:		mensajes.c mensajes.h
			gcc -c mensajes.c
semaforos.o:		semaforos.c
			gcc -c semaforos.c
servidor.o:		servidor.c servidor.h
			gcc -c servidor.c
