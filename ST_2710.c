
#include "adstar.h"

#include "ST_2710.h"
#include "ST_2710_GUI.h"
#include "Event.h"
#include "P10_3940.h"
#include "LCDCtrl.h"
#include "Key.h"
#include "ResourceManager.h"

#include "adstar/serialflash.h"

#define CONFIG_SAVE_ADDR		0xF000


#include "ymodem.h"

void TraceStatus()
{
	char* arMode[] = {"SSB", "H3E", "AM", "FM", "VHF", "AMP"};
	debugprintf("모드(%s)/전압(%.2f)/볼륨(%.2f)/송수신세기(%.2f)/감도(%.2f)\r\n", arMode[g_stOV.nOpMode], GetVoltage(), GetVolume(), GetRxTxSig(), GetRxgReveSig());
		
	debugprintf("밝기: %d, 명료도: %d, 키음: %d, 출력(내/외): %d/%d, 언어: %d\r\n", g_stOV.nBright, g_stOV.nClarify, g_stOV.bUseKeySound, g_stOV.bUseInSpk, g_stOV.bUseOutSpk, g_stOV.nLanguage);
}

void MemDump(BYTE* pMem, int nSize)
{
	int i;
	debugprintf("@0x%x:\r\n", pMem);
	for(i = 0 ; i < nSize ; i++)
	{
		debugprintf("%02x ", pMem[i]);
		if((i % 16) == 15)
			debugprintf("\r\n");
	}
	debugprintf("\r\n");
}

// Flash 동작 시험
void FlashDump()
{ 
#define MAX_TO_READ			128
	u8 binBuff[MAX_TO_READ];			// 내부 Flash Application binary를 위한 버퍼
	void* pAddr = (void*)(60*1024);		// 0~60k for bootloader, 60k~ for app & variable
	//flash_read((U32)pAddr, (BYTE*)binBuff, MAX_TO_READ);
	dcache_invalidate_way();
	memcpy((void*)binBuff, (const void*)pAddr, MAX_TO_READ);
	
	MemDump(binBuff, MAX_TO_READ);
}

//extern SURFACE* g_pSurfBase;

void TestYmodem()
{
	u8 buf[1024 * 400];
	int nSize = Ymodem_Receive(buf);
	debugprintf("TestYmodem returned: %d\r\n", nSize);
	if(nSize > 0)
	{
		MemDump(buf, 128);
#if	1
		Resource_SetResource(IDR_TEST, RESOURCE_TYPE_JPG, buf, nSize);
#else
		if(g_pSurfBase != NULL)
		{
			release_surface(g_pSurfBase);
			g_pSurfBase = NULL;
		}
		g_pSurfBase = loadjpgp((U8*)buf, nSize); 
		//g_pSurfBase = loadbmpp((U8*)buf);
		debugprintf("g_pSurfBase: 0x%04x\r\n", g_pSurfBase);
#endif
	}
}

void DoSelfTest()
{
	TRACE("DoSelfTest()!!\r\n");
}

void ST_2710_Initialize()
{
	FactoryReset();
}

void FactoryReset()
{
	g_stOV.nOpMode = 0;
	g_stOV.nOpMode = 0;			
	g_stOV.nTxMode = 1;			
	g_stOV.nRxMode = 1;			
	g_stOV.nBright = DEFAULT_BRIGHT;	
	g_stOV.nClarify = 0;			
	g_stOV.bUseKeySound = false;
	g_stOV.bUseInSpk = true;		
	g_stOV.bUseOutSpk = false;
	TRACE("FactoryReset()!!\r\n");
}

void SaveConfig(T_CONFIG* pConfig)
{
	TRACE("SaveConfig()!!\r\n");
	TraceStatus();	
	
	int i;
	int nLen = sizeof(T_CONFIG);
	int nStart = CONFIG_SAVE_ADDR/flash_get_sectorsize();			// 시작 주소는 정확히 맞춰줘야 함.
	int sectors = (CONFIG_SAVE_ADDR + nLen)/flash_get_sectorsize();
	if((CONFIG_SAVE_ADDR + nLen) % flash_get_sectorsize())
		sectors++;
	debugprintf("erase sectors...\r\n");
	for(i=nStart;i<sectors;i++)
	{
		debugprintf("%d sector\r\n",i);
		flash_erase_sector(i,1);
	}
	debugstring("update....\r\n");
	flash_write(CONFIG_SAVE_ADDR, (BYTE*)pConfig, nLen);
	debugstring("completed\r\n");
	
	//flash_write(CONFIG_SAVE_ADDR, (BYTE*)pConfig, sizeof(T_CONFIG));
}

void LoadConfig(T_CONFIG* pConfig)
{
	flash_read(CONFIG_SAVE_ADDR, (BYTE*)pConfig, sizeof(T_CONFIG));
	TRACE("LoadConfig()!!\r\n");
	TraceStatus();	
}

void SetBFOByLevel(int nLevel)
{
		
}

bool SetSSBFreq(int nFreq)
{
	return false;
}

bool SetH3EFreq(int nFreq)
{
	return false;
}

bool SetAMFreq(int nFreq)
{
	return false;
}

bool SetFMFreq(int nFreq)
{
	return false;
}

bool SetVHFFreq(int nFreq)
{
	return false;
}

bool SendDistressSignal(bool bSend)
{
	if(bSend)
	{
		SetToneFreq(2100);
		EnableTone(TRUE);
		SelectTone_Mic(TRUE);
		//SetH3EFreq(GetChannel(&cmComm, 1, -14));
		SetTRxMode(ON);
	}
	else
	{
		EnableTone(FALSE);
		SelectTone_Mic(false);
		SetTRxMode(OFF);
	}
	return true;
}

bool SendToneSignal(bool bSend)
{
	if(bSend)
	{
		SetToneFreq(1400);
		EnableTone(true);
		SelectTone_Mic(true);
		//SetTRxMode(ON);
	}
	else
	{
		EnableTone(false);
		SelectTone_Mic(false);
		//SetTRxMode(OFF);
	}
	return true;
}
