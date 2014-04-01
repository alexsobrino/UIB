#ifndef _KB_H_
#define _KB_H_

void kb_init (void);
void _kb_init (void);
void kb_end (void);
void set_kb_hook (void far (*hook)(void));

#endif // _KB_H_
