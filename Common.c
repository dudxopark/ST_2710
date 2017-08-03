
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "Common.h"
#include "adstar.h"
#include "cpu.h"

volatile i64 g_uSystemTick	= 0;					// 시스템 클럭 값 저장 변수(시스템 구동 후 ms 단위)
volatile u16 g_uTickFlag	= 0x0000;				// LSB부터 1ms, 5ms, 10ms, 50ms, 100ms, 500ms, 1000ms
volatile u16 g_uToggleFlag 	= 0x0000; 
/*
void putchar1(char c);

void SendData(int nCh, const char* pBuf, int nCount)
{
	void (*fp)(char c);
	int i = 0;
	if(nCh == 1)
		fp = putchar1;
	else
		fp = putchar;
				
	while( i < nCount )
	{
		fp(pBuf[i++]);
	}
}

void SendDataCH0(const char* pBuf, int nCount)
{
	int i = 0;
	while( i < nCount )
	{ 
		putchar(pBuf[i]);
		i++;
	}
}

void SendDataCH1(const char* pBuf, int nCount)
{	
	int i = 0;
	while( i < nCount )
	{
		putchar1(pBuf[i++]);
	}
}
*/
void TRACE(char* format, ...)
{
#if	0
	char str[128] = {"$LUTXT,"};
	va_list arg;
	va_start(arg, format);
	if(format[0] == '\r' && format[1] == '\n')
	vsprintf(str + 7, format + 2, arg);
	else
	vsprintf(str + 7, format, arg);
	va_end(arg);

	char* pData = str + strlen(str);
	char buf[6];
	sprintf(buf, "*%02X\r\n", NMEA_CalcCheckSum(str));
	strcpy(pData, buf);
	
	SendDataCH0(str, strlen(str));	
#else
	char str[128];
	va_list arg;
	va_start(arg, format);
	vsprintf(str, format, arg);
	va_end(arg);

	debugprintf(str, strlen(str));
#endif
}

i64 GetSystemClock()
{
	return g_uSystemTick;
}

void SetSystemClock()
{
	g_uSystemTick++;
	
	int nSystemClock = GetSystemClock();
	/*if((nSystemClock % 1) == 0)
		SetTickFlag(TICK_FLAG_1);	*/
	if((nSystemClock % 5) == 1)
		SetTickFlag(TICK_FLAG_5);
	if((nSystemClock % 10) == 2)
		SetTickFlag(TICK_FLAG_10);	
	if((nSystemClock % 25) == 3)
		SetTickFlag(TICK_FLAG_25);
	if((nSystemClock % 50) == 5)
		SetTickFlag(TICK_FLAG_50);
	if((nSystemClock % 100) == 7)
		SetTickFlag(TICK_FLAG_100);
	if((nSystemClock % 250) == 13)
		SetTickFlag(TICK_FLAG_250);
	if((nSystemClock % 500) == 17)
	{
		ToggleToggleFlag(TOGGLE_FLAG_500);
		SetTickFlag(TICK_FLAG_500);
	}
	if((nSystemClock % 1000) == 19)
	{
		ToggleToggleFlag(TOGGLE_FLAG_1000);
		SetTickFlag(TICK_FLAG_1000);
	}
	if((nSystemClock % 2500) == 23)
		SetTickFlag(TICK_FLAG_2500);
	if((nSystemClock % 5000) == 23)
		SetTickFlag(TICK_FLAG_5000);
}

void SetTickFlag(u16 uFlag)
{
	g_uTickFlag = uFlag | g_uTickFlag;
}

bool CheckTickFlag(u16 uFlag)
{
	if(uFlag & g_uTickFlag)
	{
		ClearTickFlag(uFlag);
		return TRUE;
	}
	return FALSE;	
}

void ClearTickFlag(u16 uFlag)
{
	g_uTickFlag = (~uFlag & g_uTickFlag);
}

void ToggleToggleFlag(u16 uFlag)
{
	if(g_uToggleFlag & uFlag)
		g_uToggleFlag &= ~uFlag;
	else
		g_uToggleFlag |= uFlag;
}

bool CheckToggleValue(u16 uFlag)
{
	if(g_uToggleFlag & uFlag)
		return TRUE;
	return FALSE;		
}

void Delay_ms(int nMilliSec)
{
	int nStartTick = GetSystemClock();
	while(1)
	{
		int nCurTick = GetSystemClock();
		if((nCurTick - nStartTick) > nMilliSec)
			break;
		ResetWatchDogTimer();
	}
}
