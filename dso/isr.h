#ifndef _ISR_H_
#define _ISR_H_

// A�ade la ISR apuntada por 'isr' en el vector de interrupci�n n� 'isrv'
void far *install_isr (void far *, unsigned);

#endif // _ISR_H_
