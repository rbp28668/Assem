

;test for blank lines and comments

;Start the data area
	DATA	

;Now test  numbers in the different bases (assuming DEFW works)
	COMMENT	'Numbers in different bases'
	DEFW	10
	DEFW	1010b
	DEFW	1010B
	DEFW 	12q
	DEFW	12Q
	DEFW	0Ah
	DEFW	0AH
	
	RADO
	
	DEFW	10.
	DEFW	10d
	DEFW	10D

	RADB
	DEFW	1010
	
	RADO
	DEFW	12	

	RADD
	DEFW 	10

	RADH	
	DEFW	0A

	RADIX 10	;test for radix always taking a decimal expression.
	RADD	
	DEFW	10
        
;Now test expressions
	COMMENT 'Basic arith/logic ops'
	DEFW	(10)		;basic parentheses
	DEFW	+10		;unary plus
	DEFW	6+4		;binary plus
	DEFW	29+-19		;unary minus
	DEFW	33-23		;binary minus
	DEFW	5*2		;multiplication
	DEFW	50/5		;division
	DEFW	80 >> 3		;shift right
	DEFW	5 << 1		;shift left
	DEFW	1+NOT-10	;logical not
	DEFW	0DAh AND 2Eh	;logical and
	DEFW	8 OR 2		;logical or
	DEFW	0DEh XOR 0D4h	;logical xor
	COMMENT 'Basic conditional exprs'
	DEFW	11+(5=5)	;equality
	DEFW	10+(5=6)	;equality (failed)
	DEFW	11+(5<>6)	;inequality
	DEFW	10+(5=6)	;inequality (failed)
	DEFW	11+(5<6)	;test less than
	DEFW	10+(5<5)	;
	DEFW	10+(5<4)	;
	DEFW	11+(6>5)	;test greater than
	DEFW	10+(5>5)
	DEFW	10+(4>5)
	DEFW	11+(5<=6)	;test less equ than
	DEFW	11+(5<=5)	;
	DEFW	10+(5<=4)	;
	DEFW	11+(6>=5)	;test greater equ than
	DEFW	11+(5>=5)
	DEFW	10+(4>=5)
	
	COMMENT 'Test CODE and DATA'
	CODE
	DEFW	10
	DATA
	DEFW	10

	COMMENT 'Test global labels public'
pub1:	DEFW	10
	PUBLIC	pub1		;make already declared label public
	PUBLIC	pub2		;make label public declared later on
pub2:	DEFW	10
	
	COMMENT 'Test external defn'
	EXTERNAL Ext.entry

	COMMENT 'Test DEFB,DEFW,DEFM and DEFS'
	DEFB	10
	DEFW	10
	DEFM	'ABCDEFG'
mark1:	DEFS	10
mark2:

	COMMENT 'Test org and reloc'
	ORG	10
	RELOC

	COMMENT 'Test Phase'
	PHASE 256

	COMMENT	'Test conditional assembly'

	COND 0
	;comments
	COMMENT 'This should not appear (#1)'

	ENDC

	COND 0
	;comments
	COMMENT 'This should not appear (#1)'
	
	ELSE
	;comments
	COMMENT 'This should appear (#1)'
	ENDC


	COND 1
	;comments
	COMMENT 'This should appear (#2)'

	ENDC

	COND 1
	;comments
	COMMENT 'This should appear (#3)'
	
	ELSE
	;comments
	COMMENT 'This should not appear (#2)'
	ENDC
	
	COMMENT 'This should appear (#4)'

	COMMENT 'Test Modules and local scope'
	CODE
	ORG 1234H-4
	DEFW 10
	DEFW 10
	MODULE mod1
local:
	ENDMOD

local:	;this should not clash with local decl in module

	PUBLIC mod2		;Declare module as public before module defn.
	MODULE mod2
	ENDMOD

	COMMENT 'Test EQU'
	Test EQU	10
	DEFW	Test

	
	;Test include file
	CODE
b4inc.code:
	DATA
b4inc.data:
	INCLUDE test.inc
	CODE
after.inc.code:
	DATA
after.inc.data:
	COMMENT 'Returned from include file'
;Now test a specific processor.

	CODE
	INCLUDE z80.asm
