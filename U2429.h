#pragma once

#include "Common.h"

// 데이터 쓰기
void U2429_WriteData(U16 uData);
// 채널(1 or 2)별 볼륨(0.00 ~ 1.00) 설정 함수
void U2429_SetVolume(int nCh, double dVolume);
// 채널(1 or 2)별 볼륨 db(-83 ~ 0) 설정 함수
void U2429_SetVolumeByNegativeDB(int nCh, int nDB);
