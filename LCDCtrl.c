/*********************************************************************************************************/
//				File Name : System.c
//                                                    
//				Date : 2013.11.06     
//                                                    
//				Version : 1.00             
//                                                    
/*********************************************************************************************************/
#include "adstar.h"
#include "LCDCtrl.h"
#include "Common.h"
#include "cpu.h"

#define LCD_OEDAC       ((volatile unsigned long *)(CRTC_BASE + 0x28))

/*********************************************************************************************************/
// Name			: InitLCD
// Argument		: 
// Return		: 
// Description		: 
/*********************************************************************************************************/
void InitLCD()
{
#if 1
	SetLCDBackLight(OFF);
/*
	//PCI .. see the LCD Spec
	*R_GP3ODIR = 1<<4;
	*R_GP3OHIGH= 1<<4;
	delayms(10);
	*R_GP3OLOW = 1<<4;
	*/
#else
	//backlight on
	*R_GP3ODIR = 1<<1;
	*R_GP3OLOW= 1<<1;
	//PCI .. see the LCD Spec
	*R_GP3ODIR = 1<<4;
	*R_GP3OLOW= 1<<4;
	delayms(10);
	*R_GP3OHIGH = 1<<4;
#endif
}

/*********************************************************************************************************/
// Name			: SetLCDBackLight
// Argument		: 
// Return		: 
// Description		: 
/*********************************************************************************************************/
void SetLCDBackLight(bool bOnOff)
{
#if	1
	SetGPIO_Output(3, 6, bOnOff);
#else
	*R_GP3ODIR = 1<<6;
	if(bOnOff == ON)
	{
		*R_GP3OHIGH= 1<<6;
	}
	else
	{
		*R_GP3OLOW= 1<<6;
	}
#endif
}

/*********************************************************************************************************/
// Name			: SetLCDContrast
// Argument		: 
// Return		: 
// Description	: 
/*********************************************************************************************************/
void SetLCDContrast(U8 nVal)
{
	//*LCD_OEDAC = ((*LCD_OEDAC) & (0xFF00FFFF)) | (nVal << 16);
	*LCD_OEDAC = 0;
}

/*********************************************************************************************************/
// Name			: SetLCDBrightness
// Argument		:
// Return		: 
// Description		: 
/*********************************************************************************************************/
void SetLCDBrightness(double dBrightRate)
{
	SetPWM_Duty(2, 1 - dBrightRate);
}

/*********************************************************************************************************/
// Name			: SetLCDBrightnessByLevel
// Argument		: nLvl( 0 ~5 Total 6 Level)
// Return		: 
// Description		: 
/*********************************************************************************************************/
void SetLCDBrightnessByLevel(U8 nLvl)
{
	double dDivider;
	if(nLvl >= 6)
		nLvl = 5;
	
	dDivider = (double)nLvl / 5;
	SetLCDBrightness(dDivider);
}
