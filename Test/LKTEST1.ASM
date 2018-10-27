; This is a test to check linking of relocatable file. This file is designed
; to be linked with lktest2.asm
		
		Reloc

		External Print.A		; Print.A is an external routine

		Public 	Test1

		Module	Test1
		LD A,0
		CALL	Print.A	
		RET
		Endmod
