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

#if 1				// ADSTAR ��� ��
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

void SetSystemClock();												// �ý��� Ŭ�� ���� �Լ�
i64 GetSystemClock();												// �ý��� Ŭ�� ��ȯ(ms����)
void SetTickFlag(u16 uFlag);										// �ð� ��� �÷��� ��
bool CheckTickFlag(u16 uFlag);										// �ð� ��� �÷��� �˻� �Լ�
void ClearTickFlag(u16 uFlag);										// �ð� ��� �÷��� Clear �Լ�
void ToggleToggleFlag(u16 uFlag);									// ��� �÷��� ���
bool CheckToggleValue(u16 uFlag);									// ��� �÷��� �˻� �Լ�: SetSystemClock() ������ ��۽����ִ� �÷��� �� ��ȯ
void Delay_ms(int nMilliSec);										// Delay �Լ�. ���� WDT�� �����ϹǷ� WDT Reset �ɸ��� ����.

void TRACE( char* format, ...);										// ������ Trace ��� ����
void SendDataCH0(const char* pBuf, int nCount);
void SendDataCH1(const char* pBuf, int nCount);
void SendData(int nCh, const char* pBuf, int nCount);				// �ø��� ������ �����Լ�
	

#endif /* COMMON_H_ */

