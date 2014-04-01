#ifndef _ISR_H_
#define _ISR_H_

// Añade la ISR apuntada por 'isr' en el vector de interrupción nº 'isrv'
void far *install_isr (void far *, unsigned);

#endif // _ISR_H_
