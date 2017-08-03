
#include "adstar.h"
#include "Event.h"
#include "CircularQueue.h"
#include "Cpu.h"

i16 g_iRotaryDelta = 0;
CircularQueue qEvent;

void InitEvent()
{
	CreateQueue(&qEvent, 128);
}

void ReleaseEvent()
{
	DestroyQueue(&qEvent);
}

bool FetchEvent(u32* pEvent)
{
	bool bRtn = DeQueue(&qEvent, pEvent);
	return bRtn;
}

void PushEvent(u32 uEvent)
{
	EnQueue(&qEvent, uEvent);
}

bool FetchEventA(u16* pEventID, u16* pEventParam)
{
	u32 uEvent;
	bool bRtn = DeQueue(&qEvent, &uEvent);
	*pEventID = (uEvent & 0xffff0000) >> 16;
	*pEventParam = uEvent & 0xffff;	
	return bRtn;
}

void PushEventA(u16 uEventID, u16 uEventParam)
{
	u32 uEvent;
	uEvent = (((u32)uEventID << 16) | (uEventParam & 0xffff));
	EnQueue(&qEvent, uEvent);
}

void DebugEvent()
{
	TRACE("qEvent Size: %d\r\n", GetQueueSize(&qEvent));
}

void EventMaker4RotaryByTimer()
{
	// 로터리 스위치 변화량 계산
	if(g_iRotaryDelta != 0)
	{
#if	1		// 델타량 있는 그대로
		u8 uDelta = g_iRotaryDelta;
#else
		u8 uDeltaTable[] = {1, 2, 4, 7, 11, 16, 22, 29, 37, 46};
		u8 uDelta = uDeltaTable[9];
		if(g_iRotaryDelta < 10)
			uDelta = uDeltaTable[abs(g_iRotaryDelta) - 1];
		if(g_iRotaryDelta < 0)
			uDelta = -uDelta;
#endif
		PushEventA(EVENT_ROTARY_SW, uDelta);
		g_iRotaryDelta = 0;
	}
}

void EventMaker4AdcByTimer()
{
	// ADC 값 변경 계산
	static u16 arADC[4] = {0};
	u16 nOldADC;
	const int iMaxSample = 10;
	int i;
	for(i = 0 ; i < 4 ; i++)
	{
		nOldADC = arADC[i];
		// ADC값 계산 시 최대/최소값 제외!!!한 평균값으로 산출
		arADC[i] = ReadADCValue_Avr(i, iMaxSample);
		//arADC[i] = ReadADCValue(i);
		if(nOldADC != arADC[i])
		{
			//debugprintf("arADC[%d] is %d\r\n", i, arADC[i]);
			PushEventA(EVENT_BAT_V + i, arADC[i]);			// EVENT_BAT_V, EVENT_V_VOLUME, EVENT_RX_TX_SIG, EVENT_RXG_REVE_SIG
		}
	}
}
