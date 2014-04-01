// Limpia la pantalla
void clearscreen (unsigned char atr)
{
	asm {
		push es
		push ax
		push cx
		push di
		mov ax, 0b800h
		mov es, ax
		mov ax, word ptr atr
		mov ah, al
    // 32 en ASCII es ''
		mov al, 32
		mov di, 0
    // 80*25 = 2000
		mov cx, 2000
		cld
		rep stosw
		pop di
		pop cx
		pop ax
		pop es
	}
}


// writetxt: escribe en la posición (x,y) el texto apuntado por  el puntero 's'
//                 con los atributos de texto 'atr' (definidos en 'screen.h')
void writetxt (unsigned char atr, unsigned char x, unsigned char y, char *s)
{
	asm {
		push es
		push ds
		push ax
		push bx
		push cx
		push dx
		push si
		push di
    // @0B800h dirección de memoria del buffer de video para 80x25
		mov ax, 0b800h
		mov es, ax
		mov ax, word ptr atr
		mov bl, al
		mov ax, word ptr x
		mov dl, al
		xor dh, dh
		mov ax, word ptr y
		mov cl, al
		xor ch, ch
		lds si, dword ptr s
		mov ax, cx
		shl ax, 1
		shl ax, 1
		shl ax, 1
		shl ax, 1
		shl ax, 1
		shl ax, 1
		shl cx, 1
		shl cx, 1
		shl cx, 1
		shl cx, 1
		add ax, cx
		add ax, dx
		shl ax, 1
		mov di, ax
		cld
		mov ah, bl
	}
otro:
	asm {
    // load del string ax <- @ds:si
		lodsb
		or al, al
		jz fin
    // @es:di <- ax
		stosw
		jmp otro
	}
fin:
	asm {
		pop di
		pop si
		pop dx
		pop cx
		pop bx
		pop ax
		pop ds
		pop es
	}
}


// writeuint: escribe en la posición (x,y) el entero sin signo 'i' con los
//                  atributos de texto 'atr' (definidos en 'screen.h')
void writeuint (unsigned char atr, unsigned char x, unsigned char y, unsigned int i)
{
	int rem;
  // unsigned int como máximo vale 65535, por tanto, necesitamos
  // 6 caracteres, "65535\0"
	char s[6];
	int j = 5;
	s[j] = '\0';
  // De entero a carácter
	while (j != 0) {
		rem = i % 10;
		i = i / 10;
		// 48 en ASCII es 0
		s[--j] = 48 + rem;
	}
	writetxt(atr,x,y,s);
}


// writelong: escribe en la posición (x,y) el entero long 'l' con los
//                  atributos de texto 'atr' (definidos en 'screen.h')
void writelong (unsigned char atr, unsigned char x, unsigned char y, unsigned long int l)
{
	int rem;
  // unsigned long int como máximo vale 2^32 = 4294967296, por tanto,
  // necesitamos 11 caracteres, "4294967296\0"
	char s[11];
	int j = 10;
	s[j] = '\0';
  // De entero a caracter
	while (j != 0) {
		rem = l % 10;
		l = l / 10;
		// 48 en ASCII es 0
		s[--j] = 48 + rem;
	}
	writetxt(atr,x,y,s);
}
