/******************************************************************************
 Copyright (C) 2011      Advanced Digital Chips Inc. 
						http://www.adc.co.kr
 Author : Software Team.

******************************************************************************/
#include "adstar.h"

static void SpiFlashInit()
{
	register int addr;
	addr = 0x80000000 ;
	*(volatile unsigned int*)addr = 0x180;//7bit:write disable
	//quad mode
	*(volatile unsigned int*)(addr+0x10)= 0x6; // CMD, Serial Flash write enable.
	*(volatile unsigned int*)(addr+0x2c)= 0x25C; // Status write, Quad enable, Write protect enable.
	*(volatile unsigned int*)addr = 0x182;//7bit:write disable
	*(volatile unsigned int*)(addr+4) = 0x0; //baudrate
	//*(volatile unsigned int*)(addr+8)= 0xf;

	//*(volatile unsigned int*)0x80000010 = 0x38;
	//*(volatile unsigned int*)0x8000000c = 0x1; //performance enhance mode
}

static void SDramInit()
{
	register int addr;
	addr = 0x80000400 ;
	register int ahbclock;
	*(volatile unsigned int*)addr = 0x10059;   //16MBytes, CAS Lantency 2  modify lab
	//*(volatile unsigned int*)addr = 0x10049;   //8MBytes, CAS Lantency 2  modify lab
	ahbclock = get_ahb_clock();
	//*(volatile unsigned int*)(addr+4) = (0x4<<8)|((ahbclock/1000000)-1);     //FEEDBACK Clock control	
	*(volatile unsigned int*)(addr+4) = 0xc2f; //FEEDBACK Clock control	// 48MHz 
}

/*
	PLL Table
X-IN :		12000000							
									
"               N
O , M"		6			8			10				
1	180		N/A			N/A			N/A				
1	120		N/A			N/A			144,000,000		
1	81		162,000,000 121,500,000 97,200,000 		
1	48		96,000,000 	72,000,000 	57,600,000 		
2	180		N/A			N/A			N/A				
3	81		54,000,000 	40,500,000 	32,400,000 		
2	48		48,000,000 	36,000,000 	28,800,000 		
4	48		24,000,000 	18,000,000 	14,400,000 		

==========================================================

	PLL Table
X-IN :		12000000							
O	M            N
			1		2		4			6
1	180		N/A		N/A		N/A			N/A
1	120		N/A		N/A		N/A			160,000,000 
1	81		N/A		N/A		162,000,000 108,000,000 
1	48		N/A		N/A		96,000,000 	64,000,000 
	2		180		N/A		N/A			N/A
3	81		N/A		N/A		54,000,000 	36,000,000 
2	48		N/A		N/A		48,000,000 	32,000,000 
4	48		N/A		N/A		24,000,000 	16,000,000 

==========================================================
OS[2:0]	O	  M	
000		1	180	
001		1	120	
010		1	 81	
011		1	 48	
100		2	180	
101		3	 81	
110		2	 48	
111		4	 48	

IS[2:0]	N
000		1
001		2
010		4
011		6
100		8
101		10
110		12
111		16

OSC 12Mhz:	96Mhz(0x33), 32.4Mhz(0x55), 28Mhz(0x65), 24Mhz(0x73), 18Mhz(0x74)
OSC 10Mhz:	101.25(0x24),48Mhz(0x35) 45Mhz(0x53), 33.75Mhz(0x54), 27Mhz(0x55), 24Mhz(0x65)
OSC 8Mhz : 108Mhz(0x23), 96Mhz(0x32), 64Mhz(0x33),	54Mhz(0x52), 48Mhz(0x62), 32Mhz(0x63), 24Mhz(0x72), 16Mhz(0x73)

*/

static void spm_load();

static ISPM void PMUInit()
{
	PMU_WRITE_ENABLE;
	*(volatile unsigned int*)R_CLKCON= 0x0;       	// external clock		
	*(volatile unsigned int*)R_BODCON= 0x0;       	// BOD disable    
	*(volatile unsigned int*)R_PLLCON= 0x5426;		// PLL1 49.302M, PLL0 98.604M	@OSC 14.608Mhz
	//*(volatile unsigned int*)R_PLLCON= 0x6434; 		// PLL1 48M, PLL0 96M	@OSC 16Mhz
	
	register int i asm( "%r10" ) = 5000;
	while ( i-- )
	{
		asm( "nop" );
	}
	
	*(volatile unsigned int*)R_CLKCON = F_CLKCON_CRTC_DSC2|F_CLKCON_CRTC_PLL1|F_CLKCON_SCLK_PLL0;//CRTC=CRTDOT/2, CRTDOT=PLL1, SYSTEM-CLOCK=PLL0	
	PMU_WRITE_DISABLE;
}

void usb_clock_init()
{
	PMU_WRITE_ENABLE;
	//*(volatile unsigned int*)0x80023C20 |= (1<<3);//USB Enable, use PLL0/2
	*(volatile unsigned int*)R_CLKCON |= F_CLKCON_USBCLK_ENABLE|F_CLKCON_USBCLK_SRC_DIV2|F_CLKCON_USBSRC_CRTC;//USB Enable,use CRTC clock
	*(volatile unsigned int*)R_USBPHYCON= F_USBPHYCON_USBDEVICE|F_USBPHYCON_RCV_ENABLE|F_USBPHYCON_PDPUL_ENABLE;//Bit8 : DEVICE(0), USB Receive Enable
	PMU_WRITE_DISABLE;
}

void usb_clock_hostinit()
{
	PMU_WRITE_ENABLE;	
	*(volatile unsigned int*)R_CLKCON |= F_CLKCON_USBCLK_ENABLE|F_CLKCON_USBCLK_SRC_DIV2|F_CLKCON_USBSRC_CRTC;//USB Enable,use CRTC clock
	*(volatile unsigned int*)R_USBPHYCON= F_USBPHYCON_USBHOST|F_USBPHYCON_RCV_ENABLE|F_USBPHYCON_PDPUL_ENABLE;//Bit8 : DEVICE(0), USB Receive Enable
	PMU_WRITE_DISABLE;		
}

void crtc_clock_init()
{
	PMU_WRITE_ENABLE;
	*(volatile unsigned int*)R_CLKCON |=F_CLKCON_CRTC_ENABLE; // CRTC Clock Enable
	PMU_WRITE_DISABLE;
}
void sound_clock_init()
{
	PMU_WRITE_ENABLE;
	if(SND_OUTPUT_CHANNEL>1)//DIGITAL MOD
		*(vU32*)R_SNDCLKCON = F_SNDCLKCON_DMCLKSRC_PLLCLK;//PLL0
	else
		*(vU32*)R_SNDCLKCON = F_SNDCLKCON_I2S_APBCLK|F_SNDCLKCON_I2S_SRCCLK4; //54/4=13Mhz
	PMU_WRITE_DISABLE;
}
void adStarInit()
{
	spm_load();
	PMUInit();
	SpiFlashInit();
	SDramInit();
}

/*
	defined at linker-script.
*/
extern unsigned int _rom_data_start[];
extern unsigned int _ram_data_start[];
extern unsigned int _ram_data_end[];

extern unsigned int _rom_ispm_start[];
extern unsigned int _ram_ispm_start[];
extern unsigned int _ram_ispm_end[];

extern unsigned int _rom_dspm_start[];
extern unsigned int _ram_dspm_start[];
extern unsigned int _ram_dspm_end[];

extern unsigned int _bss_start[];
extern unsigned int _bss_end[];
/*
void data_section_copy();
asm(".section .text");
asm("    .global _data_section_copy");
asm("	.type _data_section_copy, @function");
asm("_data_section_copy:");
asm("	push %r0-%r2");
asm("	ldi	__rom_data_start,%r0");
asm(" 	ldi	__ram_data_start,%r1");
asm(" 	ldi	__ram_data_end,%r2");
asm("_1:	cmp  %r2, %r1");
asm("   jnc  __data_copy_end");
asm("   ld    (%r0, 0), %r3");
asm("   st    %r3, (%r1, 0)");
asm("	addq 4, %r0");
asm("   addq 4, %r1");
asm("   jmp  _1");
asm("__data_copy_end:");
asm("	pop %r0-%r2");
asm("jplr");
asm(".size _data_section_copy, .-_data_section_copy");

void bss_init();
asm(".section .text");
asm("    .global _bss_init");
asm("	.type _bss_init, @function");
asm("_bss_init:");
asm("	push %r0-%r2");
asm("	ldi	__bss_start,%r0");
asm(" 	ldi	__bss_end,%r1");
asm(" 	ldi	0,%r2");
asm("_2:	cmp  %r1, %r0");
asm("   jnc  __bss_init_end");
asm("   st    %r2, (%r0, 0)");
asm("	addq 4, %r0");
asm("   jmp  _2");
asm("__bss_init_end:");
asm("	pop %r0-%r2");
asm("jplr");
asm(".size _bss_init, .-_bss_init");
*/
	

/*!
 * \brief
 * Write brief comment for crt0main here.
 *  
 * Data, ISPM, DSPM copy
 * BSS initialize
 * 
 */
 
//void crt0main() __attribute__((weak)) __attribute__((section(".ispm")));

static void spm_load()
{
	register unsigned int* src;
	register unsigned int* dest;
	register unsigned int* destend;
	register int len;
	
	/* ISPM copy */
	src = _rom_ispm_start;
	dest = _ram_ispm_start;
	destend = _ram_ispm_end;
	len = (unsigned int)destend - (unsigned int)dest;
	if(len>0)
		memcpy(dest,src,len);
	
	/* DSPM copy */
	src = _rom_dspm_start;
	dest = _ram_dspm_start;
	destend = _ram_dspm_end;
	len = (unsigned int)destend - (unsigned int)dest;
	if(len>0)
		memcpy(dest,src,len);
}

void crt0main()
{
	register unsigned int* src;
	register unsigned int* dest;
	register unsigned int* destend;
	register int len;
	
	/* data copy */
	if(_rom_data_start < (U32*)0x20000000)
	{
		src = _rom_data_start;
		dest = _ram_data_start;
		destend = _ram_data_end;
		len = (unsigned int)destend - (unsigned int)dest;
		memcpy((void*)dest,(void*)src,(int)len);
	}

	/* BSS init
	*/
	dest = _bss_start;
	destend = _bss_end;
	len = (unsigned int)destend - (unsigned int)dest;
	if(len>0)
		memset(dest,0,len);

	
	//for C++
	/*
	typedef void (*pfunc) ();
	extern pfunc __ctors[];
    extern pfunc __ctors_end[];
    pfunc *p;
	for (p = __ctors_end; p > __ctors; )
		(*--p) ();
	*/
}
