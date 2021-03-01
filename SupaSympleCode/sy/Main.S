.global  MyFunc
MyFunc:
	mov     $9, %eax
	push    %eax
	mov     $6, %eax
	pop     %edx
	add     %edx, %eax

	mov     $2, %eax
	mov     $2, %eax
