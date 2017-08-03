
#pragma once

#include "Common.h"

///////////////////////////////////////////////////////////////////////////////////////
// Ÿ�̸� ���� �Լ�
bool IsTimerEnable(int nCh);								// ä�κ� Ÿ�̸� ���� ������ ��ȯ�ϴ� �Լ�
void EnableTimer(int nCh, bool bEnable);					// ä�κ� Ÿ�̸� ����/���� �Լ�
u16 SetTimer_us(int nCh, double us);						// us ������ Ÿ�̸� �����ϴ� �Լ�
u16 SetTimerPWM_us(int nCh, double us, double dDutyRate);	// Ÿ�̸��� PWM ���� �Լ�

///////////////////////////////////////////////////////////////////////////////////////
// PWM ���� �Լ�
bool SetPWM_Period(double us);								// PWM �ֱ� ����
void SetPWM_Duty(int nCh, double dRate);					// ä�κ� PWM Duty ���� �Լ�

///////////////////////////////////////////////////////////////////////////////////////
// ADC ���� �Լ�
U16 ReadADCValue(u32 nCH);									// ADC�� �д� �Լ�
u16 ReadADCValue_Avr(u32 nCH, u8 nAvgCnt);					// ADC ��հ� �д� �Լ�. nAvrCnt��ŭ ADC���� �а� Ƣ�� ��(�ִ밪, �ּҰ�) ���� �� ��� ���

///////////////////////////////////////////////////////////////////////////////////////
// WatchDog ���� �Լ�
void InitWatchDog(int ms);		// WatchDog �ʱ�ȭ �Լ�(ms: WatchDog �ð� ����)
void ResetWatchDogTimer();		// WatchDog Reset �Լ�

///////////////////////////////////////////////////////////////////////////////////////
// GPIO ���� �Լ�
void SetGPIO_Output(int nPortNum, int nBitNum, bool bOutput);	// GPIO ��°� ���� �Լ�
bool GetGPIO_Output(int nPortNum, int nBitNum);					// GPIO ��°� Ȯ�� �Լ�

///////////////////////////////////////////////////////////////////////////////////////
// ��Ÿ �Լ�
void ResetSystem();				// �ý��� ����
void SetJtagToGPIO(bool bGPIO);	// JTAG ��Ʈ ���뵵 ���� �Լ�

