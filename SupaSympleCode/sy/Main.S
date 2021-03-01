.global  _main
_main:
	mov     $9, %eax
	push    %eax
	mov     $6, %eax
	pop     %edx
	imul    %edx, %eax

	mov     $6, %eax
	mov     $6, %eax
	ret
