
#include "util.h"
#include "adstar.h"

T_TCB arTCB[MAX_TIMER_CALLBACK] = {{-1, NULL}};

bool RegisterTCB(U64 uTimeGap, void (*procTCB)(int), int param)
{
	int i;
	for(i = 0 ; i < MAX_TIMER_CALLBACK ; i++)
	{
		if(arTCB[i].procTCB == NULL)
		{
			arTCB[i].uTimeStamp = GetSystemClock() + uTimeGap;
			arTCB[i].procTCB = procTCB;
			arTCB[i].param = param;
			return TRUE;
		}
	}
	return FALSE;
}

void ExecuteTCB()
{
	int param;
	void (*procTCB)() = NULL;
	U64 uTimeStamp = GetSystemClock();
	int i;
	for(i = 0 ; i < MAX_TIMER_CALLBACK ; i++)
	{
		if(arTCB[i].procTCB == NULL)
			continue;
		if(uTimeStamp < arTCB[i].uTimeStamp)
			continue;
		
		procTCB = arTCB[i].procTCB;
		param = arTCB[i].param;
		arTCB[i].procTCB = NULL;
		arTCB[i].uTimeStamp = -1;
		procTCB(param);
	}	
}
