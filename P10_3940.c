#include "adstar.h"
#include "adstar/uart.h"
#include "adstar/gpio.h"

#include "Common.h"
#include "P10_3940.h"
#include "ST_2710_interrupt.h"
#include "ST_2710_GUI.h"
#include "Event.h"

#include "cpu.h"
#include "util.h"

#include "Key.h"
#include "LCDCtrl.h"

//#define R_GP3ODM		(GPIO_BASE + 0xEC)
#define R_GPxODM(ch) 	((volatile unsigned char*)(GPIO_BASE+0x2C+(0x40*(ch))))

#define TX_KEY(b)		(b)?(*R_GP1OHIGH |= 0x80):(*R_GP1OLOW |= 0x80)
#define	TX_ON(b)		(b)?(*R_GP0OHIGH |= 0x04):(*R_GP0OLOW |= 0x04)
#define	RX_ON(b)		(b)?(*R_GP0OHIGH |= 0x08):(*R_GP0OLOW |= 0x08)
#define	J3E_ON(b)		(b)?(*R_GP0OHIGH |= 0x01):(*R_GP0OLOW |= 0x01)
#define	H3E_ON(b)		(b)?(*R_GP0OHIGH |= 0x02):(*R_GP0OLOW |= 0x02)
#define AM_ON(b)		(b)?(*R_GPxODM(4)|= 0x08):(*R_GPxODM(4)&=0xf7)
#define FM_VHF_ON(b)	(b)?(*R_GP4OHIGH |= 0x08):(*R_GP4OHIGH |= 0x08)			// 시험용으로 출력을 LOW로 만들지 않음
#define	RX_MUTE(b)		(b)?(*R_GP8OHIGH |= 0x02):(*R_GP8OLOW |= 0x02)
#define	AMP_ON(b)		(b)?(*R_GP8OLOW |= 0x01):(*R_GP8OHIGH |= 0x01)
#define	MIC_TONE(b)		(b)?(*R_GP5OHIGH |= 0x02):(*R_GP5OLOW |= 0x02)
//#define	TONE_SIG(b)		(b)?(*R_GP4OHIGH |= 0x80):(*R_GP4OLOW |= 0x80)
#define	BAND1(b)		(b)?(*R_GP6OHIGH |= 0x01):(*R_GP6OLOW |= 0x01)
#define	BAND2(b)		(b)?(*R_GP6OHIGH |= 0x02):(*R_GP6OLOW |= 0x02)
#define	BAND3(b)		(b)?(*R_GP6OHIGH |= 0x04):(*R_GP6OLOW |= 0x04)
#define	BAND4(b)		(b)?(*R_GP7OHIGH |= 0x02):(*R_GP7OLOW |= 0x02)
#define	BAND5(b)		(b)?(*R_GP3OHIGH |= 0x10):(*R_GP3OLOW |= 0x10)
//#define	_1ST_LOCAL(b)	(b)?(*R_GPOHIGH |= 0x80):(*R_GP1OLOW |= 0x80)

u16	g_uBatteryVoltage = 0;
u16	g_uRxTxSig = 0;
u16	g_uRxgReveSig = 0;
u16	g_uVolume = 0;

static u8	uPreviousOpMode = OPMODE_SSB;
static u8	uCurrentOpMode = OPMODE_SSB;
//bool g_bTRxMode = false;

void ProcPTT_On2()
{
	TX_KEY(ON);
}

void ProcPTT_On1()
{
	RX_ON(OFF);
	TX_ON(ON);
	RegisterTCB(50, ProcPTT_On2, 0);
}

void ProcPTT_On()
{
	switch(GetCurrentOpMode())
	{
	case OPMODE_SSB:
	case OPMODE_H3E:
#if	1
		SetTRxMode(ON);
		Beep();
#else
		RX_MUTE(ON);
		RegisterTCB(10, ProcPTT_On1, 0);		
		AMP_ON(OFF);
#endif
		break;
	case OPMODE_AM:			
	case OPMODE_FM:			
	case OPMODE_VHF:	
		BeepBeep();
		break;
	case OPMODE_AMP:
		Beep();
		TX_KEY(OFF);		// 모드 변경 시 바꿔줘야 함
		TX_ON(OFF);			// 모드 변경 시 바꿔줘야 함
		RX_ON(OFF);			// 모드 변경 시 바꿔줘야 함
		RX_MUTE(ON);		// 모드 변경 시 바꿔줘야 함
		AMP_ON(ON);
		break;		
	}
}

void ProcPTT_Off2()
{
	RX_MUTE(OFF);
}

void ProcPTT_Off1()
{
	RX_ON(ON);
	TX_ON(OFF);
	RegisterTCB(50, ProcPTT_Off2, 0);
}

void ProcPTT_Off()
{
	switch(GetCurrentOpMode())
	{
	case OPMODE_SSB:
	case OPMODE_H3E:
#if	1
		SetTRxMode(OFF);
#else
		TX_KEY(OFF);	
		RegisterTCB(10, ProcPTT_Off1, 0);
		AMP_ON(OFF);		// 모드 변경 시 바꿔줘야 함
#endif
		break;
	case OPMODE_AM:			
	case OPMODE_FM:			
	case OPMODE_VHF:	
		break;
	case OPMODE_AMP:
		TX_KEY(OFF);		// 모드 변경 시 바꿔줘야 함
		TX_ON(OFF);			// 모드 변경 시 바꿔줘야 함
		RX_ON(ON);			// 모드 변경 시 바꿔줘야 함
		RX_MUTE(OFF);		// 모드 변경 시 바꿔줘야 함
		AMP_ON(OFF);
		break;		
	}
}

void BeepBeep3()
{
	EnableBeep(OFF);
	SetBeepFreq(2036);
}

void BeepBeep2()
{
	SetBeepFreq(2800);
	EnableBeep(ON);
	RegisterTCB(100, BeepBeep3, 0);	
}

void BeepBeep1()
{
	EnableBeep(OFF);
	RegisterTCB(50, BeepBeep2, 0);	
}

void BeepBeep()
{
	SetBeepFreq(2800);
	EnableBeep(ON);
	RegisterTCB(100, BeepBeep1, 0);
}

int GetFrequency(int nOctave, int nScale)
{
	int nFreq;
	int nOctaveOffset = nOctave - 4;
	nFreq = 440 * pow(2, nOctaveOffset + nScale / 12.);
	debugprintf("Freq: %d\r\n", nFreq);
	return nFreq;
}

void PlayBeep(int nOctave, int nScale, int ms)
{
	SetBeepFreq(GetFrequency(nOctave, nScale));
	EnableBeep(true);	
	Delay_ms(ms);	
	EnableBeep(false);
	Delay_ms(5);	
}

void Beep()
{
	SetBeepFreq(2036);
	EnableBeep(true);
	RegisterTCB(100, EnableBeep, 0);	
}

void ReleaseBoard()
{
	ReleaseEvent();
}

void InitTimer()
{
	// Beep
	set_interrupt(INTNUM_TIMER0, Timer0ISR);
	enable_interrupt(INTNUM_TIMER0, true);

	set_interrupt(INTNUM_TIMER1, Timer1ISR);
	enable_interrupt(INTNUM_TIMER1, true);
	set_timer(1, 1);			// System Clock(1ms)
	
	set_interrupt(INTNUM_TIMER2, Timer2ISR);
	enable_interrupt(INTNUM_TIMER2, true);
	set_timer(2, 100);			// 100ms
	
	// Tone
	set_interrupt(INTNUM_TIMER3, Timer3ISR);
	enable_interrupt(INTNUM_TIMER3, true);
}

void InitGPIO()
{
	//	   		=	7						|6						|5						|4						|3						|2						|1						|0											
	*R_PAF0 	=	F_PAF0_7_GP7			|F_PAF0_6_SFLASH_DQ2	|F_PAF0_5_SFLASH_DQ1	|F_PAF0_4_SFLASH_CS		|F_PAF0_3_GP3			|F_PAF0_2_GP2			|F_PAF0_1_GP1			|F_PAF0_0_GP0;
	*R_PAF1	 	= 	F_PAF1_7_GP7			|F_PAF1_6_GP6			|F_PAF1_5_GP5			|F_PAF1_4_GP4			|F_PAF1_3_GP3			|F_PAF1_2_GP2			|F_PAF1_1_UART_RX0		|F_PAF1_0_UART_TX0; 
	*R_PAF2	  	=	F_PAF2_7_GP7			|F_PAF2_6_GP6			|F_PAF2_5_GP5			|F_PAF2_4_GP4			|F_PAF2_3_GP3			|F_PAF2_2_GP2			|F_PAF2_1_GP1			|F_PAF2_0_GP0; 
	*R_PAF3	 	=	F_PAF3_7_MC_PWML3		|F_PAF3_6_GP6			|F_PAF3_5_MC_PWML2		|F_PAF3_4_GP4			|F_PAF3_3_MC_PWML1		|F_PAF3_2_GP2			|F_PAF3_1_GP1			|F_PAF3_0_GP0;
	*R_PAF4	 	=	F_PAF4_7_GP7		/*	|F_PAF4_6_GP6		*/	|F_PAF4_5_GP5			|F_PAF4_4_GP4			|F_PAF4_3_GP3			|F_PAF4_2_GP2			|F_PAF4_1_GP1			|F_PAF4_0_EIRQ0;
	*R_PAF5	 	=	F_PAF5_7_CRTC_CLK_OUT	|F_PAF5_6_DISP_EN		|F_PAF5_5_HSYNC			|F_PAF5_4_VSYNC		/*	|F_PAF5_3_GP3		*/	|F_PAF5_2_GP2			|F_PAF5_1_GP1			|F_PAF5_0_GP0;
	*R_PAF6	 	=	F_PAF6_7_R7				|F_PAF6_6_R6			|F_PAF6_5_R5			|F_PAF6_4_R4			|F_PAF6_3_R3			|F_PAF6_2_GP2			|F_PAF6_1_GP1			|F_PAF6_0_GP0;
	*R_PAF7	 	=	F_PAF7_7_G7				|F_PAF7_6_G6			|F_PAF7_5_G5			|F_PAF7_4_G4			|F_PAF7_3_G3			|F_PAF7_2_G2			|F_PAF7_1_GP1		/*	|F_PAF7_0_GP0*/;
	*R_PAF8	 	=	F_PAF8_7_B7				|F_PAF8_6_B6			|F_PAF8_5_B5			|F_PAF8_4_B4			|F_PAF8_3_B3			|F_PAF8_2_GP2			|F_PAF8_1_GP1			|F_PAF8_0_GP0;	
	//*R_PAF9	 	=																															 F_PAF9_2_SFLASH_DQ3	|F_PAF9_1_SFLASH_CLK	|F_PAF9_0_SFLASH_DQ0;	
	
	*R_GP0DIR = 	0x0f;					*R_GP0ODIR =	0x0f;							*R_GP0IDIR =	0x80;
	*R_GP1DIR = 	0x80;					*R_GP1ODIR =	0x80;							*R_GP1IDIR =	0x7c;
	*R_GP2DIR = 	0xff;					*R_GP2ODIR =	0xff;							*R_GP2IDIR =	0x00;
	*R_GP3DIR = 	0x54;					*R_GP3ODIR =	0x54;							*R_GP3IDIR =	0x03;
	*R_GP4DIR = 	0xbc;					*R_GP4ODIR =	0xbc;							*R_GP4IDIR =	0x02;
	*R_GP5DIR = 	0x03;					*R_GP5ODIR =	0x03;							*R_GP5IDIR =	0x04;	
	*R_GP6DIR = 	0x07;					*R_GP6ODIR =	0x07;							*R_GP6IDIR =	0x00;
	*R_GP7DIR = 	0x02;					*R_GP7ODIR =	0x02;							*R_GP7IDIR =	0x00;
	*R_GP8DIR = 	0x03;					*R_GP8ODIR =	0x03;							*R_GP8IDIR =	0x00;

	*R_GP4PUEN	= *R_GP4PUEN | (1 << 0);					// IRQ0 Pull-up Enable

	*R_GP1PUEN	= *R_GP1PUEN | (1 << 2) | (1 << 1);			// RX0/TX0 Pull-up Enable
	*R_GP1PUS	= *R_GP1PUS | (1 << 2) | (1 << 1);
}

// 파워키가 눌러진 상태를 반환. 0: 안눌러짐, 1: 눌러짐, 2: 눌러진 채로 2000ms 경과
u8 CheckPowerKeyStatus()
{
	static i64 iTickLatestKeyChanged = (i64)0x7fffffffffffffffLL;
	static u8 btLatestStatus = false;
	u8 btRtnVal = 0;
	u8 btCurrentStatus = false;
	if((*R_GP4ILEV & 0x02) == 0x00)
		btCurrentStatus = true;
	if(btCurrentStatus)													// 눌러진 상태
	{
		btRtnVal = 1;
		if(btCurrentStatus != btLatestStatus)							// 변경(눌러짐)되었으면 변경된 시간 기록
			iTickLatestKeyChanged = GetSystemClock();		
		else if((GetSystemClock() - iTickLatestKeyChanged) > 1000)		// 누르고 있던 시간이 2000ms 이상이면 길게 누른 것으로 판다
			btRtnVal = 2;
	}	
	btLatestStatus = btCurrentStatus;
	return btRtnVal;
}

void DoPowerCtrl()
{
	u8 btStatus = CheckPowerKeyStatus();
	if(btStatus == 2)			// 전원 버튼 길게 누름
	{
		SetGPIO_Output(4, 2, LOW);
		//*R_GP4OLOW |= 0x04;
		SetLCDBackLight(OFF);
		SetKeyBackLight(0);
	}	
	else if(btStatus == 1)		// 전원 버튼 짧게 누름
	{
		SetGPIO_Output(4, 2, HIGH);
		//*R_GP4OHIGH |= 0x04;
	}
}

void SetBeepFreq(int nFreq)
{
	double dTimerPeriod;
#ifdef	USE_PWM_FOR_BEEP_N_TONE			// PWM 이용
	dTimerPeriod = (1 / (double)nFreq) * 1000000;
	SetTimerPWM_us(0, dTimerPeriod, 0.5);
#else
	nFreq = nFreq << 1;			// H->L->H를 반복해야 한 주기이므로 주파수의 2배가 되어야 함.
	dTimerPeriod = (1 / (double)nFreq) * 1000000;
	//debugprintf("SetTimer0 with ms: %.3f\r\n", dTimerPeriod);
	SetTimer_us(0, dTimerPeriod);
#endif
}

void EnableBeep(bool bEnable)
{
//#ifdef	USE_PWM_FOR_BEEP_N_TONE			// PWM 이용
	EnableTimer(0, bEnable);
//#else
//	enable_interrupt(INTNUM_TIMER0, bEnable);
//#endif
}

bool IsBeepEnable()
{
	return IsTimerEnable(0);
}

void SetToneFreq(int nFreq)
{
	double dTimerPeriod;
#ifdef	USE_PWM_FOR_BEEP_N_TONE			// PWM 이용
	dTimerPeriod = (1 / (double)nFreq) * 1000000;
	SetTimerPWM_us(3, dTimerPeriod, 0.5);
#else
	nFreq = nFreq << 1;			// H->L->H를 반복해야 한 주기이므로 주파bEnable수의 2배가 되어야 함.
	dTimerPeriod = (u32)((1 / (double)nFreq) * 1000000 + .5);
	//debugprintf("SetTimer3 with ms: %d\r\n", dTimerPeriod);
	SetTimer_us(3, dTimerPeriod);
#endif
}

void EnableTone(bool bEnable)
{
//#ifdef	USE_PWM_FOR_BEEP_N_TONE			// PWM 이용
	EnableTimer(3, bEnable);
//#else
//	enable_interrupt(INTNUM_TIMER3, bEnable);
//#endif
}

bool IsToneEnable()
{
	return IsTimerEnable(3);
}

bool IsRxMute()
{
	return (*R_GP8OLEV & 0x02);
}

void SetCurrentOpMode(int nOperationMode)
{
	switch(nOperationMode)
	{
	case OPMODE_SSB:
		BAND1(OFF);		BAND2(OFF);		BAND3(OFF);		BAND4(OFF);		BAND5(OFF);		
		J3E_ON(ON);		H3E_ON(OFF);	AM_ON(OFF);		FM_VHF_ON(OFF);
		SetCurrentScreen(SCREEN_J3E);
		break;
	case OPMODE_H3E:			
		BAND1(OFF);		BAND2(OFF);		BAND3(OFF);		BAND4(OFF);		BAND5(OFF);
		J3E_ON(OFF);	H3E_ON(ON);		AM_ON(OFF);		FM_VHF_ON(OFF);
		SetCurrentScreen(SCREEN_H3E);
		break;
	case OPMODE_AM:			
		BAND1(OFF);		BAND2(OFF);		BAND3(OFF);		BAND4(OFF);		BAND5(ON);
		J3E_ON(ON);		H3E_ON(OFF);	AM_ON(ON);		FM_VHF_ON(OFF);
		SetCurrentScreen(SCREEN_AM);
		break;
	case OPMODE_FM:			
		BAND1(OFF);		BAND2(OFF);		BAND3(ON);		BAND4(OFF);		BAND5(OFF);		
		J3E_ON(ON);		H3E_ON(OFF);	AM_ON(OFF);		FM_VHF_ON(ON);
		SetCurrentScreen(SCREEN_FM);
		break;
	case OPMODE_VHF:					
		BAND1(OFF);		BAND2(OFF);		BAND3(OFF);		BAND4(ON);		BAND5(OFF);		
		J3E_ON(ON);		H3E_ON(OFF);	AM_ON(OFF);		FM_VHF_ON(ON);
		SetCurrentScreen(SCREEN_VHF);
		break;
	case OPMODE_AMP:			
		BAND1(OFF);		BAND2(OFF);		BAND3(OFF);		BAND4(OFF);		BAND5(OFF);		
		J3E_ON(OFF);	H3E_ON(OFF);	AM_ON(OFF);		FM_VHF_ON(OFF);
		SetCurrentScreen(SCREEN_AMP);
		break;
	default:
		BAND1(OFF);		BAND2(OFF);		BAND3(OFF);		BAND4(OFF);		BAND5(OFF);
		J3E_ON(OFF);	H3E_ON(OFF);	AM_ON(OFF);		FM_VHF_ON(OFF);
		break;	
	}
	uPreviousOpMode = uCurrentOpMode;
	uCurrentOpMode = nOperationMode;	
}

int GetCurrentOpMode()
{
	return uCurrentOpMode;
}

int GetPreviousOpMode()
{
	return uPreviousOpMode;
}

void SetTRxMode(bool bTxOn)
{
#if	1
	if(bTxOn)
	{
		RX_MUTE(ON);
		RegisterTCB(10, ProcPTT_On1, 0);		
		AMP_ON(OFF);
	}
	else
	{
		TX_KEY(OFF);	
		RegisterTCB(10, ProcPTT_Off1, 0);
		AMP_ON(OFF);		// 모드 변경 시 바꿔줘야 함
	}
#else		
	int nOpMode = GetCurrentOpMode();	
	switch(nOpMode)
	{
	case OPMODE_SSB:
	case OPMODE_H3E:
		AMP_ON(ON);
		break;
	case OPMODE_AM:			
	case OPMODE_FM:			
	case OPMODE_VHF:		
		RX_MUTE(bTxOn);
		TX_ON(bTxOn);	
		RX_ON(!bTxOn);
		TX_KEY(bTxOn);
	
		AMP_ON(ON);
		//MIC_TONE(ON);
		break;
	case OPMODE_AMP:
		TX_KEY(OFF);
		TX_ON(OFF);	
		RX_ON(!bTxOn);	
		RX_MUTE(ON);
		AMP_ON(!bTxOn);
		//MIC_TONE(OFF);		
		break;		
	}
#endif
	//g_bTRxMode = bTxOn;
}

bool GetTRxMode()
{	
	//return g_bTRxMode;
	return (*R_GP0OLEV & 0x04);				// TxOn 상태 반환
}
bool g_bBKStatus = FALSE;

void SetBKStatus(bool bBKStatus)
{
	if(bBKStatus)
		RX_MUTE(ON);
	else
		RX_MUTE(OFF);		
	g_bBKStatus = bBKStatus;
}

bool GetBKStatus()
{
	return g_bBKStatus;
}

void SelectTone_Mic(bool bTone)
{
	if(bTone)
		MIC_TONE(ON);
	else
		MIC_TONE(OFF);
}

u8 ReadRotarySW()
{
	u8 nRetVal = 0;
	nRetVal = *R_GP3ILEV & ((1<<1)|(1<<0));
	return nRetVal;
}

bool ReadPTT()
{
	if( (1<<7) == (*R_GP0ILEV & (1<<7)))
		return FALSE;
	else
		return TRUE;
}

double GetVoltage()
{
	// 1/(6800/(220 + 82000 + 6800)) * 3.3
	//return (g_uBatteryVoltage / 1023. * 43.200882352941176470588235294119);
	//return (g_uBatteryVoltage / 1023. * 3.3 * 13.240485383342526199669056811914);
	// 112 @5V, 585 @25V, Gap = 473 @20V, 23.65/1V
	return ((g_uBatteryVoltage - 112) / 23.65 + 5);
}

double GetRxTxSig()
{
	double dRtn = g_uRxTxSig / (1023 - (1023 * 0.7 / 3.3));			// 다이오드 보정값 적용
	if(dRtn > 1)
		dRtn = 1;
	return dRtn;
}

double GetRxgReveSig()
{
	double dRtn = g_uRxgReveSig / (1023 - (1023 * 0.7 / 3.3));			// 다이오드 보정값 적용
	if(dRtn > 1)
		dRtn = 1;
	return dRtn;
}

double GetVolume()
{
	return g_uVolume / 1023.;
}

void SetVoltageMeasured(u16 uData)
{
	g_uBatteryVoltage = uData;
}

void SetRxTxSigMeasured(u16 uData)
{
	g_uRxTxSig = uData;
}

void SetRxgReveSigMeasured(u16 uData)
{
	g_uRxgReveSig = uData;
}

void SetVolumeMeasured(u16 uData)
{
	g_uVolume = uData;
}

void EnableSysIRQ()
{
	set_interrupt(INTNUM_EIRQ0, EIRQ0ISR);
	*R_EINTMOD = F_EINTMOD_0MOD_REDGE;
	enable_interrupt(INTNUM_EIRQ0, TRUE);
	
	//set_interrupt(INTNUM_EIRQ1, EIRQ1ISR);
	//*R_EINTMOD |= F_EINTMOD_1MOD_FEDGE;		
	//enable_interrupt(INTNUM_EIRQ1, TRUE);

	// PTT Interrupt 설정
	// Port Any(Rising&Falling) Edge Detect Register Set	
	*R_GP0RED = (1<<7);
	*R_GP0FED = (1<<7);	
	set_interrupt(INTNUM_GPIOA, ISR_PTT);
	enable_interrupt(INTNUM_GPIOA, TRUE);
	
	// BK Interrupt 설정
	// Port Any(Rising&Falling) Edge Detect Register Set	
	*R_GP5RED = (1<<2);
	*R_GP5FED = (1<<2);
	set_interrupt(INTNUM_GPIOF, ISR_BK);
	enable_interrupt(INTNUM_GPIOF, TRUE);
}

void InitUART()
{
	uart_config(0, 115200, DATABITS_8, STOPBITS_1, UART_PARNONE);
	//ClearComm0Buff();
	set_debug_channel(0);	
}

void InitPWM()
{
	SetPWM_Period(1000000/456500.);
	SetPWM_Duty(1, 0.5);
	SetPWM_Duty(2, 0.5);
	SetPWM_Duty(3, 0.5);
}

void InitBoard()
{
	crtc_clock_init();
	
	InitGPIO();		
	InitUART();
	
	InitPWM();
	InitLCD();	
	
	InitTimer();
	InitWatchDog(2000);
	EnableSysIRQ();
		
	EnableBeep(false);
	EnableTone(false);
	
	SetBeepFreq(2036);
	SetToneFreq(2100);
	SetToneFreq(1400);
	
	SelectTone_Mic(false);
	
	SetCurrentOpMode(OPMODE_AM);
	SetTRxMode(OFF);
	SetBKStatus(OFF);
	
#if	0	// 할아버지의 낡은 시계
	PlayBeep(4, SCALE_G, NOTE_QUARTER);
	
	PlayBeep(5, SCALE_C, NOTE_QUARTER);
	PlayBeep(4, SCALE_B, NOTE_EIGHTH);
	PlayBeep(5, SCALE_C, NOTE_EIGHTH);
	PlayBeep(5, SCALE_D, NOTE_QUARTER);	
	PlayBeep(5, SCALE_C, NOTE_EIGHTH);
	PlayBeep(5, SCALE_D, NOTE_EIGHTH);
	
	PlayBeep(5, SCALE_E, NOTE_QUARTER);
	PlayBeep(5, SCALE_F, NOTE_EIGHTH);	
	PlayBeep(5, SCALE_E, NOTE_EIGHTH);
	PlayBeep(4, SCALE_A, NOTE_QUARTER);		
	PlayBeep(5, SCALE_D, NOTE_EIGHTH);	
	PlayBeep(5, SCALE_D, NOTE_EIGHTH);	
	
	PlayBeep(5, SCALE_C, NOTE_QUARTER);	
	PlayBeep(5, SCALE_C, NOTE_EIGHTH);	
	PlayBeep(5, SCALE_C, NOTE_EIGHTH);	
	PlayBeep(4, SCALE_B, NOTE_QUARTER);	
	PlayBeep(4, SCALE_A, NOTE_EIGHTH);	
	PlayBeep(4, SCALE_B, NOTE_EIGHTH);	

	PlayBeep(5, SCALE_C, NOTE_HALF_DOTTED);	

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif 0	// 슈퍼마리오 메인테마
	PlayBeep(5, SCALE_E, NOTE_EIGHTH);
	PlayBeep(5, SCALE_E, NOTE_EIGHTH);
	Delay_ms(NOTE_EIGHTH);
	PlayBeep(5, SCALE_E, NOTE_EIGHTH);
	Delay_ms(NOTE_EIGHTH);
	PlayBeep(5, SCALE_C, NOTE_EIGHTH);
	PlayBeep(5, SCALE_E, NOTE_QUARTER);
	
	PlayBeep(5, SCALE_G, NOTE_QUARTER);
	Delay_ms(NOTE_QUARTER);
	PlayBeep(4, SCALE_G, NOTE_QUARTER);
	Delay_ms(NOTE_QUARTER);
	
	PlayBeep(5, SCALE_C, NOTE_QUARTER_DOTTED);
	PlayBeep(4, SCALE_G, NOTE_EIGHTH);
	Delay_ms(NOTE_QUARTER);
	PlayBeep(4, SCALE_E, NOTE_QUARTER);
	
	PlayBeep(4, SCALE_E, NOTE_EIGHTH);
	PlayBeep(4, SCALE_A, NOTE_QUARTER);
	PlayBeep(4, SCALE_B, NOTE_QUARTER);
	PlayBeep(4, SCALE_A_SHARP, NOTE_EIGHTH);
	PlayBeep(4, SCALE_A, NOTE_QUARTER);	
	
	PlayBeep(4, SCALE_G, NOTE_QUARTER / 3. * 2);	
	PlayBeep(5, SCALE_E, NOTE_QUARTER / 3. * 2);	
	PlayBeep(5, SCALE_G, NOTE_QUARTER / 3. * 2);
	PlayBeep(5, SCALE_A, NOTE_QUARTER);	
	PlayBeep(5, SCALE_F, NOTE_EIGHTH);
	PlayBeep(5, SCALE_G, NOTE_EIGHTH);
	
	Delay_ms(NOTE_EIGHTH);
	PlayBeep(5, SCALE_E, NOTE_QUARTER);	
	PlayBeep(5, SCALE_C, NOTE_EIGHTH);	
	PlayBeep(5, SCALE_D, NOTE_EIGHTH);	
	PlayBeep(4, SCALE_B, NOTE_QUARTER_DOTTED);	
	
	PlayBeep(5, SCALE_C, NOTE_QUARTER_DOTTED);	
	PlayBeep(4, SCALE_G, NOTE_EIGHTH);	
	Delay_ms(NOTE_QUARTER);
	PlayBeep(4, SCALE_E, NOTE_QUARTER);	
	
	PlayBeep(4, SCALE_E, NOTE_EIGHTH);	
	PlayBeep(4, SCALE_A, NOTE_QUARTER);	
	PlayBeep(4, SCALE_B, NOTE_QUARTER);	
	PlayBeep(4, SCALE_A_SHARP, NOTE_EIGHTH);			
	PlayBeep(4, SCALE_A, NOTE_QUARTER);			
#endif
}
