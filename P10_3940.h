#pragma once

#include "Common.h"

#define OPMODE_SSB		0
#define OPMODE_H3E		1
#define OPMODE_AM		2
#define OPMODE_FM		3
#define OPMODE_VHF		4
#define OPMODE_AMP		5

#define TIME_FOR_NOTE			500
#define NOTE_EIGHTH				(TIME_FOR_NOTE * 0.125)
#define NOTE_EIGHTH_DOTTED		(TIME_FOR_NOTE * 0.1875)
#define NOTE_QUARTER 			(TIME_FOR_NOTE * 0.25)
#define NOTE_QUARTER_DOTTED		(TIME_FOR_NOTE * 0.375)
#define NOTE_HALF				(TIME_FOR_NOTE * 0.5)
#define NOTE_HALF_DOTTED		(TIME_FOR_NOTE * 0.75)
#define NOTE_WHOLE				(TIME_FOR_NOTE * 1.0)
#define NOTE_WHOLE_DOTTED		(TIME_FOR_NOTE * 1.5)

enum {
SCALE_A = 0, SCALE_A_SHARP, SCALE_B, SCALE_C, SCALE_C_SHARP, SCALE_D, SCALE_D_SHARP, SCALE_E, SCALE_F, SCALE_F_SHARP, SCALE_G, SCALE_G_SHARP
};

void PlayBeep(int nOctave, int nScale, int ms);

void InitGPIO();
void InitBoard();
void ReleaseBoard();

void DoPowerCtrl();
u8 CheckPowerKeyStatus();

void SetBeepFreq(int nFreq);
void EnableBeep(bool bEnable);
bool IsBeepEnable();

void SetToneFreq(int nFreq);
void EnableTone(bool bEnable);
bool IsToneEnable();

void SetCurrentOpMode(int nOperationMode);
int GetCurrentOpMode();
int GetPreviousOpMode();

void SetTRxMode(bool bTxOn);
bool GetTRxMode();

void SetBKStatus(bool bBKStatus);
bool GetBKStatus();

bool IsRxMute();

void SelectTone_Mic(bool bTone);

double GetVoltage();					// 전압 측정 결과 반환
double GetRxTxSig();					// 송수신신호세기 반환
double GetRxgReveSig();					// 감도 반환
double GetVolume();						// 볼륨값 반환

void SetVoltageMeasured(u16 uData);		// 측정된 ADC(전압) 값 설정
void SetRxTxSigMeasured(u16 uData);		// 측정된 ADC(송수신신호세기) 값 설정
void SetRxgReveSigMeasured(u16 uData);	// 측정된 ADC(감도) 값 설정
void SetVolumeMeasured(u16 uData);		// 측정된 ADC(볼륨) 값 설정

void InitUART();
void InitTimer();
void ISR_PTT();
void ISR_BK();
void InitPWM();

void ProcPTT_On();
void ProcPTT_Off();

void Beep();
void BeepBeep();

u8 ReadRotarySW();
bool ReadPTT();

void EnableSysIRQ();

void EventMakerByTimer();
void EventMaker4RotaryByTimer();
void EventMaker4AdcByTimer();
