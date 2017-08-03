
#include "Common.h"

#define MAX_TIMER_CALLBACK		4

typedef struct 
{
	U64 uTimeStamp;
	void (*procTCB)(int);
	int param;
}T_TCB;

bool RegisterTCB(U64 uTimeGap, void (*procTCB)(int), int param);

void ExecuteTCB();
