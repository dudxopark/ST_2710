#pragma once

#include "Common.h"

// ������ ����
void U2429_WriteData(U16 uData);
// ä��(1 or 2)�� ����(0.00 ~ 1.00) ���� �Լ�
void U2429_SetVolume(int nCh, double dVolume);
// ä��(1 or 2)�� ���� db(-83 ~ 0) ���� �Լ�
void U2429_SetVolumeByNegativeDB(int nCh, int nDB);
