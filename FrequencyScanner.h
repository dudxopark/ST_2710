
#include "Common.h"
	
typedef struct{
	int nFreq;
	int nStrength;
}FREQ_STRENGTH;

typedef struct
{
	bool (*procSetFreq)(int nFreq);
	FREQ_STRENGTH* arStrength;
	int nChMax;
	bool bScan;
	int nScanMin;
	int nScanMax;
	int nScanMaxTmp;
	int nStart;
	int nScanNow;
	int nScanGap_ms;
}T_FREQ_SCAN;

void FreqScan_StartScan(T_FREQ_SCAN* scan, int nStart, int nMin, int nMax, int ms, int nChMax);

void FreqScan_AbortScan(T_FREQ_SCAN* scan);

int FreqScan_GetCurrentScan(T_FREQ_SCAN* scan);

bool FreqScan_IsOnScaning(T_FREQ_SCAN* scan);

// ���ϰ� 0: ����(���), 1: ������, 2: ������
int FreqScan_DoScan(T_FREQ_SCAN* scan);

void FreqScan_ClearScanResult(T_FREQ_SCAN* scan);
void FreqScan_SortScanResult(T_FREQ_SCAN* scan);
