%macro assert 1
	cmp		%1, #0
	b.eq	fail
%endmacro

%macro assert_eq 2 ; todo not clobber reg
	mov		x15, =%2
	cmp		%1, x15
	b.ne	fail
%endmacro

