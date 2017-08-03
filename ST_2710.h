
#pragma once

#include "Common.h"
#include "ST_2710_Global.h"

void ST_2710_Initialize();	// �ʱ�ȭ

void TraceStatus();			// �������� �۽�
void FlashDump();			// �÷��� �׽�Ʈ
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
