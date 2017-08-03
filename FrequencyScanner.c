
#include "FrequencyScanner.h"
#include "ST_2710_Global.h"
#include "adstar.h"
#include "P10_3940.h"

int FreqScan_CompareFreq(const void *a, const void *b)    // �������� �� �Լ� ����
{
    FREQ_STRENGTH num1 = *(FREQ_STRENGTH *)a;    // void �����͸� int �����ͷ� ��ȯ�� �� �������Ͽ� ���� ������
    FREQ_STRENGTH num2 = *(FREQ_STRENGTH *)b;    // void �����͸� int �����ͷ� ��ȯ�� �� �������Ͽ� ���� ������

    if (num1.nFreq < num2.nFreq)    // a�� b���� ���� ����
        return -1;      // -1 ��ȯ
    
    if (num1.nFreq > num2.nFreq)    // a�� b���� Ŭ ����
        return 1;       // 1 ��ȯ
    
    return 0;    // a�� b�� ���� ���� 0 ��ȯ
}

int FreqScan_CompareStrength(const void *a, const void *b)    // �������� �� �Լ� ����
{
    FREQ_STRENGTH num1 = *(FREQ_STRENGTH *)a;    // void �����͸� int �����ͷ� ��ȯ�� �� �������Ͽ� ���� ������
    FREQ_STRENGTH num2 = *(FREQ_STRENGTH *)b;    // void �����͸� int �����ͷ� ��ȯ�� �� �������Ͽ� ���� ������

    if (num1.nStrength < num2.nStrength)    // a�� b���� ���� ����
        return 1;      // 1 ��ȯ
    
    if (num1.nStrength > num2.nStrength)    // a�� b���� Ŭ ����
        return -1;       // -1 ��ȯ
    
    return 0;    // a�� b�� ���� ���� 0 ��ȯ
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
    TRACE("���ļ� ��ĵ ����\r\n");
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
    debugprintf("���ļ� ��ĵ ����\r\n");
}

int FreqScan_GetCurrentScan(T_FREQ_SCAN* scan)
{
	return (*scan).nScanNow;
}

bool FreqScan_IsOnScaning(T_FREQ_SCAN* scan)
{
    return (*scan).bScan;
}

// ���ϰ� 0: ����(���), 1: ������, 2: ������
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
	// ���� ���� �ִ�� ���� �ͺ��� 30�� �Է��� ��
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
