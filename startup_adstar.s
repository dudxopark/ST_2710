## -----------------------------------------
## -----------------------------------------
    .section .text
    .global _Reset_Start
	.type _Reset_Start, @function
_Reset_Start:
	
################################################################################
#	SPM CFG 
################################################################################
# SPM configuration
    ldi  0x700,    %r0
    mvtc 0x0,      %r3
	nop
    ldi  0x21021,  %r0 #ON  #Num of memory bank: 4
    mvtc 0x0,      %r4
	nop

#ISPM Setting..
#START address : 0x0
    ldi  0x702, %r0
    mvtc 0x0,   %r3
	nop
    ldi  0x10000000,  %r0
    mvtc 0x0,   %r4
	nop

#END address : 
    ldi  0x703, %r0
    mvtc 0x0,   %r3
	nop
    ldi  0x100003ff,  %r0
    mvtc 0x0,   %r4
	nop

#setting config reg :
    ldi  0x701, %r0
    mvtc 0x0,   %r3
    ldi  0x111, %r0
    mvtc 0x0,   %r4
	nop

#START address : 0x0
    ldi  0x712, %r0
    mvtc 0x0,   %r3
	nop
    ldi  0x10000400,   %r0
    mvtc 0x0,   %r4
	nop

#END address : 
    ldi  0x713, %r0
    mvtc 0x0,   %r3
	nop
    ldi  0x100007ff,  %r0
    mvtc 0x0,   %r4
	nop

#setting config reg :
    ldi  0x711, %r0
    mvtc 0x0,   %r3
	nop
    ldi  0x111, %r0
    mvtc 0x0,   %r4
	nop

#DSPM Setting..
#START address : 0x18000000
    ldi  0x705, %r0
    mvtc 0x0,   %r3
	nop
    ldi  0x18000000,   %r0
    mvtc 0x0,   %r4
	nop

#END address 
    ldi  0x706, %r0
    mvtc 0x0,   %r3
	nop
    ldi  0x18003fff,  %r0
    mvtc 0x0,   %r4
	nop

#setting config reg :
    ldi  0x704, %r0
    mvtc 0x0,   %r3
	nop
    ldi  0x111, %r0
    mvtc 0x0,   %r4
	nop

#START address 
    ldi  0x715, %r0
    mvtc 0x0,   %r3
	nop
    ldi  0x18004000,   %r0
    mvtc 0x0,   %r4
	nop

#END address 
    ldi  0x716, %r0
    mvtc 0x0,   %r3
	nop
    ldi  0x180077ff,  %r0
    mvtc 0x0,   %r4
	nop

#setting config reg :
    ldi  0x714, %r0
    mvtc 0x0,   %r3
	nop
    ldi  0x111, %r0
    mvtc 0x0,   %r4
	nop
	
/* initialize stack pointer */    
    
    ldi 0x18001000-8,%r8 /*  stack pointer , use internal memory since sdram do not work yet */
    mov %r8,%sp
	
	jal _adStarInit
	
	ldi _stack-8,%r8 /* stack pointer */
	mov %r8,%sp
	
	jal _CacheInit
	jal _crt0main
	jal _init_interrupt

	
    jal     _main

4:  jmp     4b

.size _Reset_Start, .-_Reset_Start
.section ._stack
_stack:    .long     1
