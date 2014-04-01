#ifndef _TIMER_H_
#define _TIMER_H_

void time_init (void);
void time_kill (void);
void interrupt timer_isr (void);
unsigned long int get_elaptime (void);
void delay_until (unsigned long int);

#endif // _TIMER_H_
