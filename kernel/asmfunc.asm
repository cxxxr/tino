bits 64
section .text

; rdi, rsi

; rax, eax, ax, ah:al
; rsi, esi, si, sil
; rcx, ecx, cx, ch:cl

; void out8(uint16 addr, uint8 data)
global out8
out8:
	mov dx, di
	mov al, sil
	out dx, al
	ret

; uint8 in8(uint16 addr)
global in8
in8:
	mov edx, ecx
	in al, dx
	ret
