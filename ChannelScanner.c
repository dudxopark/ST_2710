
#include "ChannelScanner.h"
#include "ST_2710_Global.h"
#include "adstar.h"

void ChScan_StartScan(T_CH_SCAN* scan, int nStart, int nMin, int nMax, int ms)
{
    TRACE("��ĵ ����\r\n");
	(*scan).bScan = true;
	(*scan).nScanMin = nMin;
	(*scan).nScanMax = nMax;
	(*scan).nScanMaxTmp = nMax + 1;
	(*scan).nStart = nStart;
    (*scan).nScanNow = nStart;
	(*scan).nScanGap_ms = ms;
}

void ChScan_AbortScan(T_CH_SCAN* scan)
{
    (*scan).bScan = false;
    debugprintf("��ĵ ����\r\n");
}

int ChScan_GetCurrentScan(T_CH_SCAN* scan)
{
	return (*scan).nScanNow;
}

bool ChScan_IsOnScaning(T_CH_SCAN* scan)
{
    return (*scan).bScan;
}

// ���ϰ� 0: ����(���), 1: ������, 2: ������
int ChScan_DoScan(T_CH_SCAN* scan)
{
	if(!ChScan_IsOnScaning(scan))
		return 2;
	int nFreq;
	(*scan).nScanNow++;	
	if((*scan).nScanNow > (*scan).nScanMaxTmp)
	{
		(*scan).nScanNow--;
		ChScan_AbortScan(scan);
		return 1;
	}	
	if((*scan).nScanNow > (*scan).nScanMax)
	{
		(*scan).nScanMaxTmp = (*scan).nStart;
		(*scan).nScanNow = (*scan).nScanMin - 1;
		return ChScan_DoScan(scan);
	}
	if((nFreq = (scan)->procIteratorCh((*scan).nScanNow)) == INVALID_FREQUENCY)
		return ChScan_DoScan(scan);
	debugprintf("ä��: %d, ���ļ�: %d\r\n", (*scan).nScanNow, nFreq);
	(scan)->procSetFreq(nFreq);
	
	return 0;
}
