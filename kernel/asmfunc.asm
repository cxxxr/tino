bits 64
section .text

; rdi, rsi

; rax, eax, ax, ah:al
; rsi, esi, si, sil
; rcx, ecx, cx, ch:cl
; rdi, edi, di, dil

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
	mov edx, edi
	in al, dx
	ret

global write_io_port32
write_io_port32:
	mov dx, di
	mov eax, esi
	out dx, eax
	ret

global read_io_port32
read_io_port32:
	mov dx, di
	in eax, dx
	ret

extern kernel_stack
extern kernel_entry

global call_kernel
call_kernel:
	mov rsp, kernel_stack + 1024 * 1024
	mov rdi, rcx
	call kernel_entry

global load_gdtr
load_gdtr:
	lgdt [rdi]
	ret

global set_ds_all
set_ds_all:
	mov ds, di
	mov es, di
	mov fs, di
	mov gs, di
	ret

global set_cs_ss
set_cs_ss:
	push rbp
	mov rbp, rsp
	mov ss, si
	mov rax, .next
	push rdi
	push rax
	o64 retf
.next:
	mov rsp, rbp
	pop rbp
	ret

global set_cr3
set_cr3:
	mov cr3, rdi
	ret

global get_cr3
get_cr3:
	mov rax, cr3
	ret
