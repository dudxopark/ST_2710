
#include "adstar.h"
#include "cpu.h" 

#define R_PWMMOD		((volatile U32*)(0x80022c00))
#define R_PWMCNT		((volatile U32*)(0x80022c04))
#define R_PWMPRD		((volatile U32*)(0x80022c08))
#define R_PWMCON		((volatile U32*)(0x80022c10))
#define R_PWMDUT0		((volatile U32*)(0x80022c2c))
#define R_PWMDUT1		((volatile U32*)(0x80022c30))
#define R_PWMDUT2		((volatile U32*)(0x80022c34))
#define R_PWMDUT3		((volatile U32*)(0x80022c38))
#define R_PWMDUTBASE	0x80022c2c
#define R_PWMDUT(n)		(R_PWMDUTBASE+(n)*0x04)
	
static int uWatchDogPeriod = 197208000;		// Watch Dog 리셋 값: 기본값 - 약 4초

bool IsTimerEnable(int nCh)
{
	volatile U32* r_tmcon = (U32*)TMCTRL_ADDR(nCh);
	return (*r_tmcon & F_TMCTRL_TMEN);	
}

void EnableTimer(int nCh, bool bEnable)
{
	static const int timerintnum[] = {INTNUM_TIMER0, INTNUM_TIMER1, INTNUM_TIMER2, INTNUM_TIMER3};
	U32 r_tmcon = TMCTRL_ADDR(nCh);
	
	if(bEnable)
	{
		*(volatile U32*)r_tmcon |= F_TMCTRL_TMEN;	
		enable_interrupt(timerintnum[nCh], TRUE);
	}
	else
	{
		*(volatile U32*)r_tmcon &= ~F_TMCTRL_TMEN;	
		enable_interrupt(timerintnum[nCh], FALSE);
	}
}

u16 SetTimer_us(int nCh, double us)
{
	//static const int timerintnum[] = {INTNUM_TIMER0,INTNUM_TIMER1,INTNUM_TIMER2,INTNUM_TIMER3};
	if(nCh > MAX_TIMER_CHANNEL)
		return FALSE;
	U32 r_tpcon = TPCTRL_ADDR(nCh);
	U32 r_tmcon = TMCTRL_ADDR(nCh);
	U32 r_tmcnt = TMCNT_ADDR(nCh);
	int conval;
	int i;

	double nanosecpertick;
	U32 cnt;
	U32 pres;
	
	double scaler[] = {2,8,32,128,512,2048,8192,32768,0};
	
	i=0;
	while(1)
	{
		pres = scaler[i];
		if(pres == 0)
			return FALSE;
		nanosecpertick = 1000000000./(get_apb_clock()/pres);
		cnt = ((us * 1000) / nanosecpertick + .5);
		
		if(cnt < 0xffff) // ms is too big to set, adjust PRESACLE.
			break;
		i++;
	}
	 if(i < 3)
		cnt -= 1;

	//reset
	*(volatile U32*)r_tpcon = 1<<1;
	*(volatile U32*)r_tpcon = 0;
	
	conval = i<<1;
	*(volatile U32*)r_tmcnt = cnt;
	*(volatile U32*)r_tmcon = conval/* | F_TMCTRL_TMEN*/;
	
	//enable_interrupt(timerintnum[nCh], TRUE);
	
	return cnt;
}

u16 SetTimerPWM_us(int nCh, double us, double dDutyRate)
{
	u32 uCnt = SetTimer_us(nCh, us);
	
	U32* r_tmcon = (U32*)TMCTRL_ADDR(nCh);	
	U32* r_tmdut = (U32*)TMDUT_ADDR(nCh);
	
	*(volatile U32*)r_tmcon = (*((volatile U32*)TMCTRL_ADDR(nCh))) | F_TMCTRL_TMOD_PWM/* | F_TMCTRL_TMEN*/;
	*(volatile U32*)r_tmdut = uCnt * dDutyRate;	
	
	// 타이머 출력은 타이머 별로 2개씩 정해져 있다. 아래 소스를 수정하여 선택할 수 있다.
	switch(nCh)
	{
	case 0:
		// Port 3.1
		*R_PAF3 &= ~F_PAF3_1_GP1;
		*R_PAF3 |= F_PAF3_1_TM_OUT0;
		*R_GP3DIR |= 	0x02;
		*R_GP3ODIR |=	0x02;
		// Port 9.1
		//*R_PAF9 &= ~F_PAF9_1_GP1;
		//*R_PAF9 |= F_PAF9_1_TM_OUT0;
		break;
	case 1:
		// Port 0.5
		*R_PAF0 &= ~F_PAF0_5_GP5;
		*R_PAF0 |= F_PAF0_5_TM_OUT1;		
		*R_GP0DIR |= 	0x20;
		*R_GP0ODIR |=	0x20;
		// Port 8.3
		//*R_PAF8 &= ~F_PAF8_3_GP3;
		//*R_PAF8 |= F_PAF8_3_TM_OUT1;		
		break;
	case 2:
		// Port 3.5
		*R_PAF3 &= ~F_PAF3_5_GP5;
		*R_PAF3 |= F_PAF3_5_TM_OUT2;
		*R_GP3DIR |= 	0x20;
		*R_GP3ODIR |=	0x20;
		// Port 8.5
		//*R_PAF8 &= ~F_PAF8_5_GP5;
		//*R_PAF8 |= F_PAF8_5_TM_OUT2;		
		break;
	case 3:
		// Port 4.7
		*R_PAF4 &= ~F_PAF4_7_GP7;
		*R_PAF4 |= F_PAF4_7_TM_OUT3;
		*R_GP4DIR |= 	0x80;
		*R_GP4ODIR |=	0x80;
		// Port 8.7
		//*R_PAF8 &= ~F_PAF8_7_GP7;
		//*R_PAF8 |= F_PAF8_7_TM_OUT3;		
		break;		
	}
	return uCnt;
}

U16 ReadADCValue(u32 nCH)
{
	U16 nRetVal = 0;
	U32 status = 0;
	if((nCH >= 0) && (nCH <= 4))
	{
		*R_ADCCTRL = (nCH<<5) | F_ADCCTRL_APB128 | F_ADCCTRL_EN;
		//*R_ADCCTRL = (nCH<<5) | F_ADCCTRL_APB32 | F_ADCCTRL_EN;
		*R_ADCCTRL |= F_ADCCTRL_STC;
		
		do{
			status = *R_ADCSTAT;
		}while((status & (1<<0))==0);
		
		nRetVal = *R_ADCDATA & 0x3ff;	
		*R_ADCCTRL = 0;
	}
	return nRetVal;
}

u16 ReadADCValue_Avr(u32 nCH, u8 nAvgCnt)
{
	// ADC 값 평균 계산
	int j;
	u16 nADC;
	u16 uMax, uMin;
	u16 uAccumulated, uTmp;	
	ASSERT(nAvgCnt >= 3);

	uMax = 0x0000;
	uMin = 0xffff;
	uAccumulated = 0;
	for(j = 0 ; j < nAvgCnt ; j++)
	{
		uTmp = ReadADCValue(nCH);
		uAccumulated += uTmp;
		if(uTmp > uMax)
			uMax = uTmp;
		if(uTmp < uMin)
			uMin = uTmp;			
	}
	nADC = (int)((double)(uAccumulated - uMax - uMin) / (nAvgCnt - 2) + .5);	
	return nADC;
}

void InitWatchDog(int ms)
{
	*R_WDTCTRL = F_WDTCTRL_WDTMOD_RST|F_WDTCTRL_WDTEN_ENABLE;
	double ns_per_tick = 1000000000. / get_apb_clock();
	uWatchDogPeriod = ((ms * 1000000.) / ns_per_tick + .5);
	//TRACE("ms: %d, ns/tick: %f, result: %d\r\n", ms, ns_per_tick, uWatchDogPeriod);
	ResetWatchDogTimer();
}

void ResetWatchDogTimer()
{
	*R_WDTCNT = uWatchDogPeriod;			
}

void ResetSystem()
{
	// Watch Dog Timer를 0로 설정하여 Watchdog reset를 발생시킴
	*R_WDTCNT = 0;
}

bool SetPWM_Period(double us)
{
	double scaler[] = {1,2,4,8,16,32,64,128,0};

	int i=0;
	double nanosecpertick;
	U32 cnt;
	U32 pres;

	while(1)
	{
		pres = scaler[i];
		if(pres == 0)
			return FALSE;
		nanosecpertick = 1000000000./(get_apb_clock()/pres);
		cnt = ((us * 1000) / nanosecpertick + .5);
		
		if(cnt < 0xffff) // ms is too big to set, adjust PRESACLE.
			break;
		i++;
	}
	if(i < 3)
		cnt -= 1;
	
	//mc_pwm all counter reset & release
	*R_PWMMOD |= (1<<6); 
	*R_PWMMOD &= ~(1<<6); 

	//pwm period 
	*R_PWMPRD = cnt;					// 108 - 1. 
	//*R_PWMDUT1 = cnt / 2;    			// 54, pwm duty 1	for key pad backlight
	//*R_PWMDUT2 = cnt / 2;    			// 54, pwm duty 2	for lcd backlight
	//*R_PWMDUT3 = cnt / 2;    			// 54 - 1, pwm duty 3	for BFO. 456.5kHz
	*R_PWMCON |= (1<<10|1<<9|1<<11);  	// pwm independent output 1,2,3 ch
	*R_PWMMOD = 1<<7|(i&0x07);   		// pwm enable	(APB Clock)
	
	return true;
}

void SetPWM_Duty(int nCh, double dRate)
{
	ASSERT(dRate <= 1);
	u16 uDuty = round(((*R_PWMPRD & 0xffff) - 1) * dRate);
	*((U32*)R_PWMDUT(nCh)) = uDuty;
}

void SetJtagToGPIO(bool bGPIO)
{
	if(bGPIO)
	{
		*R_PAF6 = *R_PAF6 & (~0x003f);
		*R_PAF6 = *R_PAF6 |F_PAF6_2_GP2			|F_PAF6_1_GP1			|F_PAF6_0_GP0;
		*R_PAF7 = *R_PAF7 & (~0x000f);
		*R_PAF7 = *R_PAF7 |F_PAF7_1_GP1 		| F_PAF7_0_GP0;
	}
	else
	{
		*R_PAF6 = *R_PAF6 & (~0x003f);
		*R_PAF6 = *R_PAF6 | F_PAF6_2_TDI 		| F_PAF6_1_TCK			| F_PAF6_0_NTRST;
		*R_PAF7 = *R_PAF7 & (~0x000f);
		*R_PAF7 = *R_PAF7 | F_PAF7_1_TDO 		| F_PAF7_0_TMS;
	}
}

void SetGPIO_Output(int nPortNum, int nBitNum, bool bOutput)
{
	u8 byteValue = 1 << nBitNum;
	*(R_GPODIR(nPortNum)) |= byteValue;
	bOutput?(*(R_GPOHIGH(nPortNum))|=byteValue):(*(R_GPOLOW(nPortNum))|=byteValue);
}

bool GetGPIO_Output(int nPortNum, int nBitNum)
{
	return *R_GPOLEV(nPortNum) & (1 << nBitNum);
}
