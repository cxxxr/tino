bits 64
section .text

; rdi, rsi

; rax, eax, ax, ah:al
; rsi, esi, si, sil
; rcx, ecx, cx, ch:cl

; void write_io_port8(uint16 addr, uint8 data)
global write_io_port8
write_io_port8:
	mov dx, di
	mov al, sil
	out dx, al
	ret

; uint8 read_io_port8(uint16 addr)
global read_io_port8
read_io_port8:
	mov edx, ecx
	in al, dx
	ret

extern kernel_stack
extern kernel_entry

global call_kernel
call_kernel:
	mov rsp, kernel_stack + 1024 * 1024
	call kernel_entry

global load_gdtr
load_gdtr:
	lgdt [rdi]
	ret
