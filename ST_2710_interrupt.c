
#include "adstar.h"

#include "ST_2710_interrupt.h"
#include "P10_3940.h"
#include "Key.h"
#include "Event.h"
#include "util.h"

void Timer0ISR()		// Beep
{	
	CRITICAL_ENTER();

#ifndef	USE_PWM_FOR_BEEP_N_TONE
	// GPIO 4.4 Beep Signal
	if(*R_GP4OLEV & 0x10)
		*R_GP4OLOW |= 0x10;
	else
		*R_GP4OHIGH |= 0x10;
#endif
	
	CRITICAL_EXIT();
}

void Timer1ISR()
{	
	CRITICAL_ENTER();

	SetSystemClock();
	
	ExecuteTCB();
	
	CRITICAL_EXIT();
}

void Timer2ISR()	// Key/Rotary SW/ADC
{	
	CRITICAL_ENTER();
	
	ScanKeyCodeByTimer();
	EventMaker4RotaryByTimer();
	EventMaker4AdcByTimer();
	
	CRITICAL_EXIT();
}

void Timer3ISR()	// Tone
{
	CRITICAL_ENTER();	
	
#ifndef	USE_PWM_FOR_BEEP_N_TONE
	// GPIO 4.5 Tone Signal
	if(*R_GP4OLEV & 0x20)
		*R_GP4OLOW |= 0x20;
	else
		*R_GP4OHIGH |= 0x20;
#endif
	
	CRITICAL_EXIT();
}

void ISR_BK()
{
	static bool bPreStatus = FALSE;
	
	CRITICAL_ENTER();	
	bool bCurStatus = FALSE;
	
	if((*R_GP5ILEV & (1<<2)) == 0x00)		// Low¸é Signal in
		bCurStatus = TRUE;
	
	if((bCurStatus == TRUE) && (bPreStatus == FALSE))
	{
		PushEventA(EVENT_BK, SIGNAL_ON);
		//debugprintf("BK Signal\r\n");
	}
	else if((bCurStatus == FALSE) && (bPreStatus == TRUE))
	{
		PushEventA(EVENT_BK, SIGNAL_OFF);
		//debugprintf("BK Non-Signal\r\n");
	}
	bPreStatus = bCurStatus;	
	*R_GP5EDS = (1<<2);			// Clear
	CRITICAL_EXIT();
}

void ISR_PTT()
{
	static bool bPrevLocalPTT = FALSE;
	
	CRITICAL_ENTER();	
	bool bCurLocalPTT = ReadPTT();
	
	if((bCurLocalPTT == TRUE) && (bPrevLocalPTT == FALSE))
	{
		ProcPTT_On();
		PushEventA(EVENT_PTT, SIGNAL_ON);
		//debugprintf("PTT-ON\r\n");
	}
	else if((bCurLocalPTT == FALSE) && (bPrevLocalPTT == TRUE))
	{
		ProcPTT_Off();
		PushEventA(EVENT_PTT, SIGNAL_OFF);
		//debugprintf("PTT-OFF\r\n");
	}
	bPrevLocalPTT = bCurLocalPTT;	
	*R_GP0EDS = (1<<7);			// Clear

	CRITICAL_EXIT();
}

void EIRQ0ISR()
{
	u8	uRotateValue;
	
	CRITICAL_ENTER();
	
	//debugstring("EIRQ0 Interrupt\r\n");				
	//if(*R_INTPEND(0) & (1<<INTNUM_EIRQ0))
	
	uRotateValue = ReadRotarySW() & 0x03;
	
	if(uRotateValue == 0x01)
	{
		//debugstring("Turned Right\r\n");
		g_iRotaryDelta++;
	}
	else if(uRotateValue == 0x02)
	{
		//debugstring("Turned Left\r\n");
		g_iRotaryDelta--;
	}
	
	*R_INTPENDCLR = (INTNUM_EIRQ0 + 0x20);
	CRITICAL_EXIT();
}



