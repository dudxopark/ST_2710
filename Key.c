#include "adstar.h"
#include "Key.h"
#include "cpu.h"
#include "Event.h"

#define MAX_ROW		5
#define MAX_COL		4

typedef union _KEY{ 
	U8 	arKey[4];
	U32	uKey;
}UNION_KEY;

static i64 iTickLatestKeyChanged = (i64)0x7fffffffffffffffLL;
static UNION_KEY unCurrentKey, unLatestKey;	

void SelectKeyCol(U8 nCol)
{
	u8 uHigh, uLow;
	uHigh = 1 << (4 + nCol);
	uLow = (~uHigh) & 0xf0;
	*R_GP2OHIGH = uHigh;
	*R_GP2OLOW = uLow;
}

u8 ReadKey()
{
	u8 uKey = (u8)*R_GP1ILEV & 0x7c;
	uKey = uKey >> 2;
	return uKey;
}

u16 PKeyToVKey(u32 uPKey)
{
/*	
#define MAX_PKEY			18
#define PKEY_NUM0			0x00040000						#define VKEY_NUM0				0x00			
#define PKEY_NUM1			0x00000001                      #define VKEY_NUM1				0x01
#define PKEY_NUM2			0x00000100                      #define VKEY_NUM2				0x02
#define PKEY_NUM3			0x00010000                      #define VKEY_NUM3				0x03
#define PKEY_NUM4			0x01000000                      #define VKEY_NUM4				0x04
#define PKEY_NUM5			0x00000002                      #define VKEY_NUM5				0x05
#define PKEY_NUM6			0x00000200                      #define VKEY_NUM6				0x06
#define PKEY_NUM7			0x00020000                      #define VKEY_NUM7				0x07
#define PKEY_NUM8			0x02000000                      #define VKEY_NUM8				0x08
#define PKEY_NUM9			0x00000004                      #define VKEY_NUM9				0x09
#define PKEY_CANCEL			0x00000400                      #define VKEY_CANCEL				0x0A
#define PKEY_ENTER			0x04000000                      #define VKEY_ENTER				0x0B
#define PKEY_TONE			0x00000008                      #define VKEY_TONE				0x0C
#define PKEY_BRIGHT			0x00000800                      #define VKEY_BRIGHT				0x0D
#define PKEY_CLARITY		0x00080000                      #define VKEY_CLARITY			0x0E
#define PKEY_AMP			0x08000000                      #define VKEY_AMP				0x0F
#define PKEY_MODE			0x00000010                      #define VKEY_MODE				0x10
#define PKEY_DISTRESS		0x00001000                      #define VKEY_DISTRESS			0x11
*/
	int i;
	const u32 arPKey[] = {0x00040000, 0x00000001, 0x00000100, 0x00010000, 0x01000000, 0x00000002, 0x00000200, 0x00020000, 0x02000000, 0x00000004, 0x00000400, 0x04000000, 0x00000008, 0x00000800, 0x00080000, 0x08000000, 0x00000010, 0x00001000};
	for( i = 0 ; i < 18/*MAX_PKEY*/ ; i++ )
	{
		if(arPKey[i] == uPKey)
			return i;
	}
	return VKEY_NONE;
}

void ResetKeyCol()
{
	*R_GP2OLOW= (( 1<<4)|(1<<5)|(1<<6)|(1<<7));
}

U16 ScanKeyCodeByTimer()
{
	//u8 uEvent = 0;
	UNION_KEY unKey;
	U16 i = 0, uCount = 0;
	u32 uMask = 0x01;

	///////////////////////////////////////////////////////////////////////////////
	// 현재 눌러진 모든 키 판별(가능한 것들만)
	ResetKeyCol();
	for(i = 0 ; i < MAX_COL ; i++)			// 4
	{
		SelectKeyCol(i);
		unKey.arKey[i] = ReadKey();		
		
		//ResetKeyCol();
		//delayms(1);
	}
	ResetKeyCol();
	
	///////////////////////////////////////////////////////////////////////////////
	// 예외처리. 키 중복 검사: 키가 중복되어 눌러졌는지 판별할 것. 중복되었을 경우 정상적인 키 입력으로 보지 않고 바로 리턴
	for(i = 0 ; i < 32 ; i++)
	{
		if((unKey.uKey & uMask) != 0)
			uCount++;
		uMask = uMask << 1;
	}	
	if(uCount > 1)			// 중복되어 눌러짐
	{
		//debugprintf("Key overlapped: 0x%08x\r\n", unKey.uKey);
		return uCount;
	}
		
	///////////////////////////////////////////////////////////////////////////////
	// 키 이벤트 생성. 키가 눌러졌는지 떨어졌는지 오래 눌러졌는지
	unCurrentKey.uKey = unKey.uKey;		
	if(unLatestKey.uKey != unCurrentKey.uKey)
	{		
		if(unCurrentKey.uKey == 0x00000000)		
		{
			if(iTickLatestKeyChanged == 0x7fffffffffffffffLL)
			{
				// 길게 눌렀다 떨어짐
				PushEventA(EVENT_VKEY_LPOP, PKeyToVKey(unLatestKey.uKey));
				//debugprintf("Long Key Popped: 0x%08x\r\n", unLatestKey.uKey);
				//uEvent = 4;	
				//EnableBeep(false);
			}
			else
			{	
				// 떨어짐 이벤트 생성
				PushEventA(EVENT_VKEY_SPOP, PKeyToVKey(unLatestKey.uKey));
				//debugprintf("Key Popped: 0x%08x\r\n", unLatestKey.uKey);
				//uEvent = 2;
				//EnableBeep(false);
			}
		}
		else
		{
			// 눌러짐 이벤트 생성
			PushEventA(EVENT_VKEY_SPUSH, PKeyToVKey(unCurrentKey.uKey));
			//debugprintf("Key Pushed: 0x%08x\r\n", unCurrentKey.uKey);			
			//uEvent = 1;
			//EnableBeep(true);
		}
		iTickLatestKeyChanged = GetSystemClock();
	}
	else if(unCurrentKey.uKey != 0x00000000)
	{
		if((GetSystemClock() - iTickLatestKeyChanged) > 1000)			// 누르고 있던 시간이 1000ms 이상이면 길게 누른 것으로 판다
		{
			iTickLatestKeyChanged = 0x7fffffffffffffffLL;
			// 길게 눌러짐 이벤트 생성
			PushEventA(EVENT_VKEY_LPUSH, PKeyToVKey(unCurrentKey.uKey));
			//debugprintf("Long Key Pushed: 0x%08x\r\n", unCurrentKey.uKey);
			//uEvent = 3;
		}		 		
	}
	///////////////////////////////////////////////////////////////////////////////
	// 마무리
	unLatestKey.uKey = unCurrentKey.uKey;
	
	return uCount; 
}

u16	GetLastKey()
{
	return PKeyToVKey(unLatestKey.uKey);
}

void SetKeyBackLight(double dBrightRate)
{
	SetPWM_Duty(1, dBrightRate);
}

void SetKeyBackLightByLevel(U8 nLvl)
{
	double dDivider;
	if(nLvl >= 6)
		nLvl = 5;
	
	dDivider = (double)nLvl / 5;
	SetKeyBackLight(dDivider);
}
