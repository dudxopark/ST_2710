
#include "typedef.h"
#include "text.h"
#include "Common.h"
#include "Event.h"
#include "adstar.h"
#include "cpu.h"

#include "P10_3940.h"
#include "U2429.h"
#include "ST_2710_GUI.h"
#include "ST_2710_GUI_Manager.h"
#include "ST_2710.h"
#include "ST_2710_Global.h"

#include "adstar/pmu.h"
#include "adstar/uart.h"
#include "adstar/gpio.h"

//#include "LCDCtrl.h"
#include "key.h"

T_CONFIG g_stOV;

int main()
{
	InitEvent();
	InitBoard();
	
	InitGUI();									// �׷���, ä��, ����, �⺻ �������� �ʱ�ȭ
	InitGUIManager();

	ST_2710_Initialize();

	ProcBootUp();								// ���� ����, ���÷ΰ�, �ʱ����� �׸� ����, ���� Ȯ�� ��

	SetJtagToGPIO(FALSE);
	debugprintf(START_GREETING);
	debugprintf("AHB: %d, APB: %d\r\n", get_ahb_clock(), get_apb_clock());
	
	while(1)
	{
		DoPowerCtrl();											// ������ư ª�� ������ On, ��� ������ Off
			
		if(CheckTickFlag(TICK_FLAG_5000))
		{
		//	TestYmodem();
		}
		if(CheckTickFlag(TICK_FLAG_1000))
		{
			//TraceStatus();
			g_uLatestFPS = g_uFPS;
			g_uFPS = 0;
			
			g_uLatestLPS = g_uLPS;
			g_uLPS = 0;
		}
		if(CheckTickFlag(TICK_FLAG_500))
		{
		}
		if(CheckTickFlag(TICK_FLAG_100))
		{
			PushEventA(EVENT_TIMERTICK_100MS, 0);
			DrawProc();			
		}	
		if(CheckTickFlag(TICK_FLAG_50))
		{			
			//debugprintf("lps: %d, mode: %d\r\n", g_uLatestLPS, GetCurrentOpMode());
			//U2429_SetVolume(1, GetVolume());
			//ADF4351_WriteData(GetVolume() * rand());
		}					
		if(CheckTickFlag(TICK_FLAG_10))
		{
			//DebugEvent();
		}		
		EventProc();
		g_uLPS++;
		ResetWatchDogTimer();
	}
	ReleaseBoard();
	
	return 0;
}
