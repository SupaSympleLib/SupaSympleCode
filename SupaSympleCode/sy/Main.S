.global  _main
_main:
	mov     $9, %eax
	push    %eax
	mov     $6, %eax
	pop     %edx
	imul    %edx, %eax

	ret
