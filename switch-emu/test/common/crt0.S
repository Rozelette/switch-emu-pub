.text
.globl    _start

_start:
	bl main
	svc	#0x07