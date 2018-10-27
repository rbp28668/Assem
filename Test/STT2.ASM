; This file is to be used with stt1.asm to test correct operation of the symbol
; table code when 2 files are assembled by a single invocation of the
; assembler.

loc1:	DEFW 22
loc2:	DEFW loc3
loc3:	DEFW loc1


