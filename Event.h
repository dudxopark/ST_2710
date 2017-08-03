
#pragma once

///////////////////////////////////////////////////////////////////////////////////
// 이벤트 상위 2 바이트
#define EVENT_VKEY_SPOP			0x0000
#define EVENT_VKEY_SPUSH		0x0001
#define EVENT_VKEY_LPOP			0x0002
#define EVENT_VKEY_LPUSH		0x0003
#define EVENT_BAT_V				0x0004
#define EVENT_V_VOLUME			0x0005
#define EVENT_RX_TX_SIG			0x0006
#define EVENT_RXG_REVE_SIG		0x0007
#define EVENT_ROTARY_SW			0x0008
#define EVENT_PTT				0x0009
#define EVENT_BK				0x000A

#define EVENT_REDRAW			0x000B
//#define EVENT_DISTRESS			0x000B

#define EVENT_TIMERTICK_100MS	0x000C
#define EVENT_CH_SCAN			0x000D
#define EVENT_FREQ_SCAN			0x000E
#define EVENT_UPDATEDATA		0x000F
#define EVENT_UPDATE_SCREEN		0x0010


#define EVENT_APP_BASE			0x1000

///////////////////////////////////////////////////////////////////////////////////
// 이벤트 하위 2 바이트
#define VKEY_NUM0				0x0000
#define VKEY_NUM1				0x0001
#define VKEY_NUM2				0x0002
#define VKEY_NUM3				0x0003
#define VKEY_NUM4				0x0004
#define VKEY_NUM5				0x0005
#define VKEY_NUM6				0x0006
#define VKEY_NUM7				0x0007
#define VKEY_NUM8				0x0008
#define VKEY_NUM9				0x0009
#define VKEY_CANCEL				0x000A
#define VKEY_ENTER				0x000B
#define VKEY_TONE				0x000C
#define VKEY_BRIGHT				0x000D
#define VKEY_CLARITY			0x000E
#define VKEY_AMP				0x000F
#define VKEY_MODE				0x0010
#define VKEY_DISTRESS			0x0011
#define VKEY_NONE				0xffff
                                  
#define SIGNAL_OFF				0x0000
#define SIGNAL_ON				0x0001

#include "Common.h"

void InitEvent();									// 이벤트 초기화
void ReleaseEvent();								// 이벤트 종료처리

bool FetchEvent(u32* pEvent);						// 이벤트 인출
void PushEvent(u32 uEvent);							// 이벤트 삽입
bool FetchEventA(u16* pEventID, u16* pEventParam);	// 이벤트 인출
void PushEventA(u16 uEventID, u16 uEventParam);		// 이벤트 삽입

void DebugEvent();

extern i16 g_iRotaryDelta;

void EventMaker4RotaryByTimer();
void EventMaker4AdcByTimer();
