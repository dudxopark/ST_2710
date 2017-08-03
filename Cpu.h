
#pragma once

#include "Common.h"

///////////////////////////////////////////////////////////////////////////////////////
// 타이머 관련 함수
bool IsTimerEnable(int nCh);								// 채널별 타이머 구동 중인지 반환하는 함수
void EnableTimer(int nCh, bool bEnable);					// 채널별 타이머 구동/멈춤 함수
u16 SetTimer_us(int nCh, double us);						// us 단위로 타이머 설정하는 함수
u16 SetTimerPWM_us(int nCh, double us, double dDutyRate);	// 타이머의 PWM 설정 함수

///////////////////////////////////////////////////////////////////////////////////////
// PWM 관련 함수
bool SetPWM_Period(double us);								// PWM 주기 설정
void SetPWM_Duty(int nCh, double dRate);					// 채널별 PWM Duty 설정 함수

///////////////////////////////////////////////////////////////////////////////////////
// ADC 관련 함수
U16 ReadADCValue(u32 nCH);									// ADC값 읽는 함수
u16 ReadADCValue_Avr(u32 nCH, u8 nAvgCnt);					// ADC 평균값 읽는 함수. nAvrCnt만큼 ADC값을 읽고 튀는 값(최대값, 최소값) 제거 후 평균 계산

///////////////////////////////////////////////////////////////////////////////////////
// WatchDog 관련 함수
void InitWatchDog(int ms);		// WatchDog 초기화 함수(ms: WatchDog 시간 설정)
void ResetWatchDogTimer();		// WatchDog Reset 함수

///////////////////////////////////////////////////////////////////////////////////////
// GPIO 관련 함수
void SetGPIO_Output(int nPortNum, int nBitNum, bool bOutput);	// GPIO 출력값 설정 함수
bool GetGPIO_Output(int nPortNum, int nBitNum);					// GPIO 출력값 확인 함수

///////////////////////////////////////////////////////////////////////////////////////
// 기타 함수
void ResetSystem();				// 시스템 리셋
void SetJtagToGPIO(bool bGPIO);	// JTAG 포트 사용용도 설정 함수

