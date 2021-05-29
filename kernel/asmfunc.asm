bits 64
section .text

global out8
out8:
	mov dx, di
	mov al, sil
	out dx, al
	ret

global in8
in8:
	mov dx, dil
	in al, dx
	ret
