
#include "Common.h"

typedef struct
{
	int (*procIteratorCh)(int nIndex);
	bool (*procSetFreq)(int nFreq);
	
	bool bScan;
	int nScanMin;
	int nScanMax;
	int nScanMaxTmp;
	int nStart;
	int nScanNow;
	int nScanGap_ms;
}T_CH_SCAN;

void ChScan_StartScan(T_CH_SCAN* scan, int nStart, int nMin, int nMax, int ms);

void ChScan_AbortScan(T_CH_SCAN* scan);

int ChScan_GetCurrentScan(T_CH_SCAN* scan);

bool ChScan_IsOnScaning(T_CH_SCAN* scan);

// ���ϰ� 0: ����(���), 1: ������, 2: ������
int ChScan_DoScan(T_CH_SCAN* scan);
