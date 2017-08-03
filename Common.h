// Common.h
#ifndef COMMON_H_
#define COMMON_H_

#include <stdint.h>

#ifndef TRUE
#define TRUE	1
#endif
#ifndef FALSE
#define FALSE	0
#endif

#if 1				// ADSTAR 사용 시
#include "typedef.h"
typedef	 unsigned char		u08;
typedef	 signed long long	i64;
typedef	 signed short		i16;
#else
typedef unsigned char	byte;
typedef unsigned char	bool;
typedef unsigned char	BOOL;

typedef	 unsigned char		u08;
typedef	 unsigned short		u16;
typedef	 unsigned long		u32;
typedef	 unsigned long long	u64;

typedef	 signed char		i08;
typedef	 signed short		i16;
typedef	 signed long		i32;
typedef	 signed long long	i64;
#endif

#define ON					1
#define OFF					0

#define HIGH				1
#define LOW					0

#define YES					1
#define NO					0

#define KHZ					1000
#define MHZ					1000000

#define TICK_FLAG_5000		0x0400
#define TICK_FLAG_2500		0x0200
#define TICK_FLAG_1000		0x0100
#define TICK_FLAG_500		0x0080
#define TICK_FLAG_250		0x0040
#define TICK_FLAG_100		0x0020
#define TICK_FLAG_50		0x0010
#define TICK_FLAG_25		0x0008
#define TICK_FLAG_10		0x0004
#define TICK_FLAG_5			0x0002
#define TICK_FLAG_1			0x0001

#define TOGGLE_FLAG_1000	TICK_FLAG_1000
#define TOGGLE_FLAG_500		TICK_FLAG_500

//#define USE_PWM_FOR_BEEP_N_TONE

void SetSystemClock();												// 시스템 클럭 증가 함수
i64 GetSystemClock();												// 시스템 클럭 반환(ms단위)
void SetTickFlag(u16 uFlag);										// 시간 경과 플래그 셋
bool CheckTickFlag(u16 uFlag);										// 시간 경과 플래그 검사 함수
void ClearTickFlag(u16 uFlag);										// 시간 경과 플래그 Clear 함수
void ToggleToggleFlag(u16 uFlag);									// 토글 플래그 토글
bool CheckToggleValue(u16 uFlag);									// 토글 플래그 검사 함수: SetSystemClock() 내에서 토글시켜주는 플래그 값 반환
void Delay_ms(int nMilliSec);										// Delay 함수. 내부 WDT가 동작하므로 WDT Reset 걸리지 않음.

void TRACE( char* format, ...);										// 디버깅용 Trace 기능 제공
void SendDataCH0(const char* pBuf, int nCount);
void SendDataCH1(const char* pBuf, int nCount);
void SendData(int nCh, const char* pBuf, int nCount);				// 시리얼 데이터 전송함수
	

#endif /* COMMON_H_ */

