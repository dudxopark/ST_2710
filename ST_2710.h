
#pragma once

#include "Common.h"
#include "ST_2710_Global.h"

void ST_2710_Initialize();	// 초기화

void TraceStatus();			// 상태정보 송신
void FlashDump();			// 플래쉬 테스트
void TestYmodem();

void DoSelfTest();
void FactoryReset();
void SaveConfig(T_CONFIG* pConfig);
void LoadConfig(T_CONFIG* pConfig);

void SetBFOByLevel(int nLevel);

bool SetSSBFreq(int nFreq);
bool SetH3EFreq(int nFreq);
bool SetAMFreq(int nFreq);
bool SetFMFreq(int nFreq);
bool SetVHFFreq(int nFreq);

bool SendDistressSignal(bool bSend);
bool SendToneSignal(bool bSend);
