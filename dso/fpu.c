// Inicialización de la FPU, guardando su estado en @s
void far fpu_init (char *s)
{
	asm {
		push ds
		push bx
		lds bx, [bp + 6]
		finit
		fsave ds:[bx]
		fwait
		pop bx
		pop ds
	}
}


// Guarda el estado del FPU en @s
void far fpu_save(char *s)
{
	asm {
		push ds
		push bx
		lds bx, [bp + 6]
		fsave ds:[bx]
		fwait
		pop bx
		pop ds
	}
}


// Sobreescribe el FPU con @s
void far fpu_restore(char *s)
{
	asm {
		push ds
		push bx
		lds bx, [bp + 6]
		frstor ds:[bx]
		fwait
		pop bx
		pop ds
	}
}
