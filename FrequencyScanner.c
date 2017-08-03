
#include "FrequencyScanner.h"
#include "ST_2710_Global.h"
#include "adstar.h"
#include "P10_3940.h"

int FreqScan_CompareFreq(const void *a, const void *b)    // 오름차순 비교 함수 구현
{
    FREQ_STRENGTH num1 = *(FREQ_STRENGTH *)a;    // void 포인터를 int 포인터로 변환한 뒤 역참조하여 값을 가져옴
    FREQ_STRENGTH num2 = *(FREQ_STRENGTH *)b;    // void 포인터를 int 포인터로 변환한 뒤 역참조하여 값을 가져옴

    if (num1.nFreq < num2.nFreq)    // a가 b보다 작을 때는
        return -1;      // -1 반환
    
    if (num1.nFreq > num2.nFreq)    // a가 b보다 클 때는
        return 1;       // 1 반환
    
    return 0;    // a와 b가 같을 때는 0 반환
}

int FreqScan_CompareStrength(const void *a, const void *b)    // 내림차순 비교 함수 구현
{
    FREQ_STRENGTH num1 = *(FREQ_STRENGTH *)a;    // void 포인터를 int 포인터로 변환한 뒤 역참조하여 값을 가져옴
    FREQ_STRENGTH num2 = *(FREQ_STRENGTH *)b;    // void 포인터를 int 포인터로 변환한 뒤 역참조하여 값을 가져옴

    if (num1.nStrength < num2.nStrength)    // a가 b보다 작을 때는
        return 1;      // 1 반환
    
    if (num1.nStrength > num2.nStrength)    // a가 b보다 클 때는
        return -1;       // -1 반환
    
    return 0;    // a와 b가 같을 때는 0 반환
}

void FreqScan_ClearScanResult(T_FREQ_SCAN* scan)
{
	if((scan)->arStrength != NULL)
	{
		free((scan)->arStrength);
		(scan)->arStrength = NULL;
	}	
}

void FreqScan_SortScanResult(T_FREQ_SCAN* scan)
{
	qsort((scan)->arStrength, (*scan).nChMax, sizeof(FREQ_STRENGTH), FreqScan_CompareFreq);
}

void FreqScan_StartScan(T_FREQ_SCAN* scan, int nStart, int nMin, int nMax, int ms, int nChMax)
{
    TRACE("주파수 스캔 시작\r\n");
	(*scan).bScan = true;
	(*scan).nScanMin = nMin;
	(*scan).nScanMax = nMax;
	(*scan).nScanMaxTmp = nMax + 1;
	(*scan).nStart = nStart;
    (*scan).nScanNow = nStart;
	(*scan).nScanGap_ms = ms;
	(*scan).nChMax = nChMax;
	
	FreqScan_ClearScanResult(scan);
	(scan)->arStrength = (FREQ_STRENGTH*)malloc(sizeof(FREQ_STRENGTH) * nChMax);
	memset((scan)->arStrength, 0xff, sizeof(FREQ_STRENGTH) * nChMax);
}

void FreqScan_AbortScan(T_FREQ_SCAN* scan)
{
    (*scan).bScan = false;
    debugprintf("주파수 스캔 중지\r\n");
}

int FreqScan_GetCurrentScan(T_FREQ_SCAN* scan)
{
	return (*scan).nScanNow;
}

bool FreqScan_IsOnScaning(T_FREQ_SCAN* scan)
{
    return (*scan).bScan;
}

// 리턴값 0: 정상(계속), 1: 마지막, 2: 비정상
int FreqScan_DoScan(T_FREQ_SCAN* scan)
{
	if(!FreqScan_IsOnScaning(scan))
		return 2;
	
	(*scan).nScanNow++;	
	if((*scan).nScanNow > (*scan).nScanMaxTmp)
	{
		(*scan).nScanNow--;
		FreqScan_AbortScan(scan);
		return 1;
	}	
	if((*scan).nScanNow > (*scan).nScanMax)
	{
		(*scan).nScanMaxTmp = (*scan).nStart;
		(*scan).nScanNow = (*scan).nScanMin - 1;
		return FreqScan_DoScan(scan);
	}
	(scan)->procSetFreq((*scan).nScanNow);
	// 수신 감도 최대로 좋은 것부터 30개 입력할 것
	int nStrength = GetRxgReveSig() * 1024;	
	if(nStrength > (scan)->arStrength[(*scan).nChMax - 1].nStrength)
	{
		//debugprintf("Freq: %d, Strength: %d\r\n", (*scan).nScanNow, nStrength);
		(scan)->arStrength[(*scan).nChMax - 1].nFreq = (*scan).nScanNow;
		(scan)->arStrength[(*scan).nChMax - 1].nStrength = nStrength;
		qsort((scan)->arStrength, (*scan).nChMax, sizeof(FREQ_STRENGTH), FreqScan_CompareStrength);
	}	
	return 0;
}
