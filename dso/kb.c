#include <stdlib.h>

#include "sems.h"
#include "isr.h"
#include "boolean.h"

unsigned char key;
static unsigned char kbhit;
static void far *old_int09;
static semaphore *kbs;

static void far (*kb_hook)(void) = NULL;


void set_kb_hook (void far (*hook)(void))
{
	kb_hook = hook;
}


// Lectura de teclado (reentrante!)
static unsigned char kbread (void)
{
	unsigned char c;
	sem_wait(kbs);
	if (kbhit) {
		kbhit = FALSE;
		c = key;
	} else {
		c = 0;
  }
	sem_signal(kbs);
	return c;
}


// ISR de gestión de teclado
static void interrupt kb_isr (void)
{
	asm {
		in al, 60h	// Código de la tecla (@60h puerto teclado)
		cmp al, 80h	// ¿Se ha pulsado o soltado?
		ja ret_kb	  // Si tecla soltada -> ret_kb
		mov bl, byte ptr TRUE
		mov kbhit, bl
		mov key, al	// guardamos el código
	}
	if (kb_hook != NULL) kb_hook();
ret_kb:
	asm {
    // Enviamos el ACK
		in al, 61h	// Estado del teclado
		or al, 80h	// Activamos "disable keyboard"
		out 61h, al
		and al, 7Fh	// Desactivamos el "disable keyboard"
		out 61h, al
		mov al, 20h
		out 20h, al
	}
}


// Instalación de la ISR de control de teclado
void _kb_init(void)
{
	kbhit = FALSE;
	old_int09 = install_isr(kb_isr, 0x09);
}


// Inicialización del semáforo
void kb_init(void)
{
	kbs = sem_create(1);
	_kb_init();
}


// Restauramos la ISR original
void kb_end(void)
{
	install_isr(old_int09, 0x09);
}
