; Include file for 8051 microprocessor to name SFRs and bit addresses.

	
; 8051 Special Function Registers (SFRs)
	ACC	equ	0E0H	;Accumulator
	B	equ	0F0H	;Multiplication Register
	DPH	equ	083H	;Data Pointer (High byte)
	DPL	equ	082H	;Data Pointer (Low byte)
	IE	equ	0A8H	;Interrupt Enable
	IP	equ	0B8H	;Interrupt Priority
	P0	equ	080H	;Port 0
	P1	equ	090H	;Port 1
	P2	equ	0A0H	;Port 2
	P3	equ	0B0H	;Port 3
	PSW	equ	0D0H	;Program Status Word
	SBUF	equ	099H	;Serial Port Buffer
	SCON	equ	098H	;Serial Port Controller
	SP	equ	081H	;Stack Pointer
	TCON	equ	088H	;Timer Control
	TH0	equ	08CH	;Timer 0 (high byte)
	TH1	equ	08DH	;Timer 1 (high byte)
	TL0	equ	08AH	;Timer 0 (low byte)
	TL1	equ	08BH	;Timer 1 (low byte)
	TMOD	equ	089H	;Timer Mode

;Named bit positions
CY	equ	0D7H	;PSW.7	Carry Flag
AC	equ	0D6H	;PSW.6	Auxiliary Carry Flag
F0	equ	0D5H	;PSW.5	Flag 0 (user definable)
RS1	equ	0D4H	;PSW.4	Register bank select bit 1
RS0	equ	0D3H	;PSW.3	Register bank select bit 0
OV	equ	0D2H	;PSW.2	Overflow flag
P	equ	0D0H	;PSW.0	Parity flag

TF1	equ	08FH	;TCON.7	Timer 1 Overflow Flag	
TR1	equ	08EH	;TCON.6	Timer 1 Run Control Bit
TF0	equ	08DH	;TCON.5	Timer 0 Overflow Flag
TR0	equ	08CH	;TCON.4	Timer 0 Run Control Bit
IE1	equ	08BH	;TCON.3	Interrupt 1 Edge Flag
IT1	equ	08AH	;TCON.2	Interrupt 1 Type Control Bit
IE0	equ	089H	;TCON.1	Interrupt 0 Edge Flag
IT0	equ	088H	;TCON.0	Interrupt 0 Type Control Bit

SM0	equ	09FH	;SCON.7	Serial Mode Control Bit 0
SM1	equ	09EH	;SCON.6	Serial Mode Control Bit 1
SM2	equ	09DH	;SCON.5	Serial Mode Control Bit 2
REN	equ	09CH	;SCON.4	Receiver Enable
TB8	equ	09BH	;SCON.3	Transmit Bit 8
RB8	equ	09AH	;SCON.2	Receive Bit 8
TI	equ	099H	;SCON.1	Transmit Interrupt Flag
RI	equ	098H	;SCON.0	Receive Interrupt Flag

EA	equ	0AFH	;IE.7	Enable All Interrupts
ES	equ	0ACH	;IE.4	Enable Serial Port Interrupt
ET1	equ	0ABH	;IE.3	Enable Timer 1 Interrupt
EX1	equ	0AAH	;IE.2	Enable External Interrupt 1
ET0	equ	0A9H	;IE.1	Enable Timer 0 Interrupt
EX0	equ	0A8H	;IE.0	Enable External Interrupt 0

RD	equ	0B7H	;P3.7	Read data for External Memory
WR	equ	0B6H	;P3.6	Write Data for External Memory
T1	equ	0B5H	;P3.5	Timer/Counter 1 External Flag
T0	equ	0B4H	;P3.4	Timer/Counter 0 External Flag
INT1	equ	0B3H	;P3.3	Interrupt 1 Input Pin
INT0	equ	0B2H	;P3.2	Interrupt 0 Input Pin
TXD	equ	0B1H	;P3.1	Serial Port Transmit Pin
RXD	equ	0B0H	;P3.0	Serial Port Receive Pin

PS	equ	0BCH	;IP.4	Priority of Serial Port Interrupt
PT1	equ	0BBH	;IP.3	Priority of Timer 1 Interrupt
PX1	equ	0BAH	;IP.2	Priority of External Interrupt 1
PT0	equ	0B9H	;IP.1	Priority of Timer 0 Interrupt
PX0	equ	0B8H	;IP.0	Priority of External Interrupt 0

