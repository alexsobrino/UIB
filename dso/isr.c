#include <dos.h>

void far *install_isr (void far *isr, unsigned isrv_num)
{
	void far *old_isrv;
	unsigned sg, of;

	sg = FP_SEG(isr);
	of = FP_OFF(isr);
	// La IVT está en el segmento 0
	asm push es
	asm xor ax, ax
	asm mov es, ax // es = 0
  // Cada vector de interrupción ocupa 4 bytes
  // Para calcular el 'offset' multiplicamos por 4
	isrv_num *= 4;
	asm cli
	asm mov bx, isrv_num
	asm mov ax, word ptr es:[bx]
	asm mov word ptr old_isrv, ax
	asm mov ax, word ptr es:[bx+2]
	asm mov word ptr old_isrv + 2, ax
	asm mov ax, of
	asm mov word ptr es:[bx], ax
	asm mov ax, sg
	asm mov word ptr es:[bx+2], ax
	asm sti
	asm pop es
	return old_isrv;
}
