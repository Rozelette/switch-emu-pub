.global _Reset
_Reset:
 LDR x0, =stack_top
 MOV sp, x0
 BL c_entry
 B .
