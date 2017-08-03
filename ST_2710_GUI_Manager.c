
#include "adstar.h"

#include "P10_3940.h"
#include "ResourceManager.h"
#include "ST_2710_Global.h"
#include "ST_2710.h"
#include "ST_2710_GUI.h"
#include "ST_2710_GUI_Comm.h"
#include "ST_2710_GUI_Radio.h"
#include "ST_2710_GUI_VHF.h"
#include "ST_2710_GUI_Amp.h"
#include "ST_2710_GUI_Manager.h"

#include "Event.h"

#include "LCDCtrl.h"
#include "Key.h"
#include "P10_3940.h"

typedef struct _tagGUI_SCREEN
{
	void (*drawproc)();
	int (*eventproc)(u16 uEvent, u16 uParam);
}T_GUI_SCREEN;

void DrawPerformance();
void DrawProc_Config();
bool EventProc_Config(u16 uEvent, u16 uParam);

T_GUI_SCREEN g_arScreen[SCREEN_MAX] = {
{DrawProc_Comm, EventProc_Comm},
{DrawProc_Comm, EventProc_Comm},
{DrawProc_Radio, EventProc_Radio},
{DrawProc_Radio, EventProc_Radio},
{DrawProc_VHF, EventProc_VHF},
{DrawProc_Amp, EventProc_Amp},
{DrawProc_Config, EventProc_Config}		
};

bool (*GetEventProc(int nSel))(u16 uEvent, u16 uParam)
{
	return g_arScreen[nSel].eventproc;
}

void (*GetDrawProc(int nSel))()
{
	return g_arScreen[nSel].drawproc;
}

void RenderRuler()
{
	int i;
	int nWidth = LCD_RESOLUTION_X;
	int nHeight = LCD_RESOLUTION_Y;
	u32 rgbLine = MAKE_COLORREF(0,0,255);
#if	0
	int nDivider = 4;
	for(i = 0 ; i < nDivider ; i++)
	{
		draw_vline(nWidth / nDivider * i, 0, nHeight, rgbLine);
		draw_hline(0, nHeight / nDivider * i, nWidth, rgbLine);
	}
#else
	int nGap = 48;
	for(i = 0 ; i < round((double)nWidth / nGap) ; i++)
	{
		draw_vline(nGap * i, 0, nHeight, rgbLine);
	}
	for(i = 0 ; i < round((double)nHeight / nGap) ; i++)
	{
		draw_hline(0, nGap * i, nWidth, rgbLine);
	}
#endif	
}

void InitGUIManager()
{
	InitGUI_Comm();
	InitGUI_Radio();
	InitGUI_VHF();
	InitGUI_Amp();
}

void DrawProc()
{
	set_draw_target(getbackframe());
#if	0	// 디자인 안
	DrawDesign();
#else
	DrawStatusBar();
	GetDrawProc(GetCurrentScreen())();
	if(GetLastKey() == VKEY_DISTRESS)				// 시험용
		RenderRuler();
/*
	if(((GetSystemClock() % 1000) / 500) < 1)
	{
		char sTmp[2] = {0, 0};
		RECT rcFreq, rcCh;
		rcFreq.x = 128;	rcFreq.y = 272 * 0.5 + 30;	rcFreq.w = 352 * 0.9;	rcFreq.h = 272 * 0.25;
		rcCh.x = 128;	rcCh.y = 272 * 0.25 + 30;	rcCh.w = 352 * 0.9;		rcCh.h = 272 * 0.25;

		U32 rgbFG = MAKE_COLORREF(0,0,0);
		U32 rgbFreq = MAKE_COLORREF(229,115,44);
		egl_font_set_color(Resource_GetFont(IDF_24), rgbFG);
		egl_font_set_color(Resource_GetFont(IDF_64), rgbFG);
		sTmp[0] = 0xdb;
		DrawText(Resource_GetFont(IDF_64), rcCh, sTmp, ALIGN_H_RIGHT | ALIGN_V_LOWER);
		
		egl_font_set_color(Resource_GetFont(IDF_64), rgbFreq);
		sTmp[0] = 0xdb;
		DrawText(Resource_GetFont(IDF_64), rcFreq, sTmp, ALIGN_H_RIGHT | ALIGN_V_LOWER);
	}*/

#endif
	DrawPerformance();
	flip();
	g_uFPS++;
}

bool EventProc()
{
	u16 uEventID, uEventParam;

	if(!FetchEventA(&uEventID, &uEventParam))
		return false;	
	if(PreFetchEvent(uEventID, uEventParam))			// 선행처리가 필요한 것들 수행
		return true;	
	
	if(uEventID != 0x000c)
		TRACE("[Event Fetched: 0x%04x - %d]\r\n", uEventID, (int)uEventParam);
	
	if(GetEventProc(GetCurrentScreen())(uEventID, uEventParam))
		return true;	
	
	switch(uEventID)
	{
	case EVENT_VKEY_SPOP:
		EventProc_VkeyPopS(uEventParam);
		break;
	case EVENT_VKEY_SPUSH:		
		EventProc_VkeyPushS(uEventParam);
		break;
	case EVENT_VKEY_LPOP:
		EventProc_VkeyPopL(uEventParam);
		break;
	case EVENT_VKEY_LPUSH:
		EventProc_VkeyPushL(uEventParam);
		break;
	case EVENT_ROTARY_SW:
		break;
	case EVENT_PTT:
		//SetTRxMode(uEventParam);
		SelectTone_Mic(!uEventParam);
		break;
	case EVENT_BK:
		SetBKStatus(uEventParam);
		break;
	case EVENT_REDRAW:
		DrawProc();
		break;			
	}
	return false;
}

void ProcBootUp()
{
	set_draw_target(getfrontframe());	
	drawsurface((SURFACE*)Resource_GetSurface(IDR_BOOTUP), 0, 0);
		
	Delay_ms(500);

	char sVersion[128] = "";

	RECT rcText = {0,0,LCD_RESOLUTION_X,130};
//	egl_font_set_color(Resource_GetFont(IDF_64), MAKE_COLORREF(255,255,255));
//	DrawText(Resource_GetFont(IDF_64), rcText, MODEL_NO, ALIGN_H_CENTER | ALIGN_V_CENTER);
//	rcText.y += Resource_GetFont(IDF_64)->h + 5;
	
	rcText.x = 0;		rcText.y = (LCD_RESOLUTION_Y * 0.75) - Resource_GetFont(IDF_DEFAULT)->h - 3;		rcText.w = LCD_RESOLUTION_X;		rcText.h = 100;
	egl_font_set_color(Resource_GetFont(IDF_DEFAULT), MAKE_COLORREF(255,255,255));

	rcText.y = 180;
	sprintf(sVersion, "Model No: %s", MODEL_NO);
	egl_font_set_color(Resource_GetFont(IDF_24), MAKE_COLORREF(255,255,255));
	DrawText(Resource_GetFont(IDF_24), rcText, sVersion, ALIGN_H_CENTER | ALIGN_V_UPPER);
	
	Delay_ms(500);

	rcText.y += 35;
	sprintf(sVersion, "S/W Version: %s", SW_VERSION);
	DrawText(Resource_GetFont(IDF_DEFAULT), rcText, sVersion, ALIGN_H_CENTER | ALIGN_V_UPPER);
	
	rcText.y += 20;
	sprintf(sVersion, "H/W Version: %s", HW_VERSION);
	DrawText(Resource_GetFont(IDF_DEFAULT), rcText, sVersion, ALIGN_H_CENTER | ALIGN_V_UPPER);
	
	Delay_ms(1000);
}

void DrawStatusBar()
{
	RECT rcStatusBar = {0, 0, LCD_RESOLUTION_X, 48};
	//RECT rcStatusBar = {0, 236, 360, 40}; 
	int nVoltage;
	char sVoltage[10] = "";

	char* sMode;
	char* arMode[] = {"SSB", "H3E", "AM", "FM", "VHF", "AMP"};
	int nMode = GetCurrentOpMode();
	
	u32 rgbFG = MAKE_COLORREF(30,30,50);
	u32 rgbBG = MAKE_COLORREF(10,10,20);
	u32 rgbSeperator = MAKE_COLORREF(5,5,10);
	u32 rgbText = MAKE_COLORREF(255,255,255);
	
	sMode = arMode[nMode];
	nVoltage = (int)(GetVoltage() * 10 + .5);
	sprintf(sVoltage, "%d.%0dV  ", nVoltage / 10, nVoltage % 10);

	draw_rectfill(rcStatusBar.x, rcStatusBar.y, rcStatusBar.w, rcStatusBar.h, rgbBG);
	draw_hline(rcStatusBar.x, rcStatusBar.y + rcStatusBar.h, rcStatusBar.x + rcStatusBar.w, rgbSeperator);
	
	egl_font_set_color(Resource_GetFont(IDF_DEFAULT), rgbText);	
	
	int nPosX;
	RECT rcText;
	rcText.x = rcStatusBar.x;
	rcText.y = rcStatusBar.y;
	rcText.w = 64;
	rcText.h = rcStatusBar.h;
	switch(nMode)
	{
	case 0:
	case 1:
		nPosX = nMode * rcText.w;		draw_rectfill(rcText.x + nPosX, rcText.y, rcText.w, rcText.h, rgbFG);
		rcText.x = 0;					draw_vline(rcText.x + rcText.w, rcText.y + 5, rcText.y + rcText.h - 5, rgbSeperator);			DrawText(Resource_GetFont(IDF_24), rcText, "SSB", ALIGN_H_CENTER);
		rcText.x = rcText.w;			draw_vline(rcText.x + rcText.w, rcText.y + 5, rcText.y + rcText.h - 5, rgbSeperator);			DrawText(Resource_GetFont(IDF_24), rcText, "H3E", ALIGN_H_CENTER);
		break;
	case 2:
	case 3:
	case 4:
		nPosX = (nMode - 2) * rcText.w;	draw_rectfill(rcText.x + nPosX, rcText.y, rcText.w, rcText.h, rgbFG);		
		rcText.x = 0;					draw_vline(rcText.x + rcText.w, rcText.y + 5, rcText.y + rcText.h - 5, rgbSeperator);			DrawText(Resource_GetFont(IDF_24), rcText, "AM", ALIGN_H_CENTER);
		rcText.x = rcText.w;			draw_vline(rcText.x + rcText.w, rcText.y + 5, rcText.y + rcText.h - 5, rgbSeperator);			DrawText(Resource_GetFont(IDF_24), rcText, "FM", ALIGN_H_CENTER);
		rcText.x = rcText.w * 2;		draw_vline(rcText.x + rcText.w, rcText.y + 5, rcText.y + rcText.h - 5, rgbSeperator);			DrawText(Resource_GetFont(IDF_24), rcText, "VHF", ALIGN_H_CENTER);
		break;
	case 5:
		nPosX = (nMode - 5) * rcText.w;	draw_rectfill(rcText.x + nPosX, rcText.y, rcText.w, rcText.h, rgbFG);			
		rcText.x = 0;					draw_vline(rcText.x + rcText.w, rcText.y + 5, rcText.y + rcText.h - 5, rgbSeperator);			DrawText(Resource_GetFont(IDF_24), rcText, "AMP", ALIGN_H_CENTER);
		break;			
	}	
#if	0
	//char sPTT[7] = "";	
	char sBK[6] = "";
	char sMute[8] = "";
	char sTone[5] = "";
	char sBeep[5] = "";
	if(GetTRxMode())
		strcpy(sPTT, "PTT ON");
	if(GetBKStatus())
		strcpy(sBK, "BK ON");
	if(IsRxMute())
		strcpy(sMute , "Rx Mute");		
	if(IsToneEnable())
		strcpy(sTone, "Tone");
	if(IsBeepEnable())
		strcpy(sBeep, "Beep");
	
	//bitfont_draw(Resource_GetFont(IDF_DEFAULT), rcStatusBar.x + rcStatusBar.w - 300, rcStatusBar.y + 10, sPTT);
	bitfont_draw(Resource_GetFont(IDF_DEFAULT), rcStatusBar.x + rcStatusBar.w - 250, rcStatusBar.y + 10, sBK);
	bitfont_draw(Resource_GetFont(IDF_DEFAULT), rcStatusBar.x + rcStatusBar.w - 200, rcStatusBar.y + 10, sMute);
	bitfont_draw(Resource_GetFont(IDF_DEFAULT), rcStatusBar.x + rcStatusBar.w - 150, rcStatusBar.y + 10, sTone);
	bitfont_draw(Resource_GetFont(IDF_DEFAULT), rcStatusBar.x + rcStatusBar.w - 100, rcStatusBar.y + 10, sBeep);
	bitfont_draw(Resource_GetFont(IDF_DEFAULT), rcStatusBar.x + rcStatusBar.w - 50, rcStatusBar.y + 10, sVoltage);
#else
	if(GetTRxMode())
	{
		drawsurface(Resource_GetSurface(IDR_ICO_PTT), rcStatusBar.x + rcStatusBar.w - 6 * 48 + 8, rcStatusBar.y + 8);
	}
	if(GetBKStatus())
	{
		drawsurface(Resource_GetSurface(IDR_ICO_BK), rcStatusBar.x + rcStatusBar.w - 5 * 48 + 8, rcStatusBar.y + 8);
	}
	if(IsRxMute())
	{
		drawsurface(Resource_GetSurface(IDR_ICO_RXMUTEON), rcStatusBar.x + rcStatusBar.w - 4 * 48 + 8, rcStatusBar.y + 8);
	}
	else
	{
		drawsurface(Resource_GetSurface(IDR_ICO_RXMUTEOFF), rcStatusBar.x + rcStatusBar.w - 4 * 48 + 8, rcStatusBar.y + 8);
	}
	if(IsBeepEnable())
	{
		drawsurface(Resource_GetSurface(IDR_ICO_BEEP), rcStatusBar.x + rcStatusBar.w - 3 * 48 + 8, rcStatusBar.y + 8);
	}
	if(IsToneEnable())
	{
		drawsurface(Resource_GetSurface(IDR_ICO_TONEON), rcStatusBar.x + rcStatusBar.w - 3 * 48 + 8, rcStatusBar.y + 8);
	}
	DrawText(Resource_GetFont(IDF_24), rcStatusBar, sVoltage, ALIGN_H_RIGHT);
#endif			
}

void EventProc_VkeyPopS(u8 uVkey)
{
	// 시험용 코드
/*	switch(uVkey)
	{
	case VKEY_NUM1:
		LoadConfig(&g_stOV);
		break;
	case VKEY_NUM2:
		SaveConfig(&g_stOV);
		break;
	case VKEY_NUM3:
		FactoryReset();
		break;
	case VKEY_NUM4:
		DoSelfTest();
		break;		
	}*/
	
	if(uVkey == VKEY_DISTRESS)
	{				
		SendDistressSignal(false);
	}
	else if(uVkey == VKEY_MODE)
	{
		int nMode = GetCurrentOpMode();
		nMode++;
		if(nMode >= 5)
			nMode = 0;
		SetCurrentOpMode(nMode);
	}
	else if(uVkey == VKEY_BRIGHT)
	{
		static int nDelta = -1;
		if(abs(g_stOV.nBright) == 5)
			nDelta = -nDelta;
		g_stOV.nBright += nDelta;
		SetKeyBackLightByLevel(abs(g_stOV.nBright));
		SetLCDBrightnessByLevel(abs(g_stOV.nBright));
	}
	else if(uVkey == VKEY_AMP)
	{
		if(GetCurrentOpMode() != OPMODE_AMP)
			SetCurrentOpMode(OPMODE_AMP);
		else 
			SetCurrentOpMode(GetPreviousOpMode());
	}
}

void EventProc_VkeyPopL(u8 uVkey)
{
	if(uVkey == VKEY_DISTRESS)
	{				
		SendDistressSignal(false);
	}
}

void EventProc_VkeyPushS(u8 uVkey)
{
	if(uVkey == VKEY_DISTRESS)
	{				
		SendDistressSignal(true);
	}
}

void EventProc_VkeyPushL(u8 uVkey)
{
	switch(uVkey)
	{
	case VKEY_ENTER:
		if(GetCurrentScreen() != SCREEN_SETTING)
			SetCurrentScreen(SCREEN_SETTING);
		else
			SetCurrentScreen(GetPreviousScreen());
		break;
	case VKEY_BRIGHT:
		g_stOV.nBright = DEFAULT_BRIGHT;
		SetKeyBackLightByLevel(abs(g_stOV.nBright));
		SetLCDBrightnessByLevel(abs(g_stOV.nBright));
		break;
	}
}

// 선행 처리가 필요한 항목들 실행
bool PreFetchEvent(u16 uEventID, u16 uEventParam)
{
	bool bRtnVal = false;
	switch(uEventID)
	{
	case EVENT_VKEY_SPOP:
		//EnableBeep(false);
		break;
	case EVENT_VKEY_SPUSH:		
		if((uEventParam != VKEY_DISTRESS) && (uEventParam != VKEY_TONE))
			Beep();
			//EnableBeep(true);
		break;
	case EVENT_VKEY_LPOP:
		//EnableBeep(false);
		break;
	case EVENT_VKEY_LPUSH:
		//EnableBeep(true);		// LPUSH가 유효한 경우에만 소리나도록 변경할 것
		Beep();
		break;	
	case EVENT_BAT_V:
		SetVoltageMeasured(uEventParam);
		bRtnVal = true;
		break;		
	case EVENT_V_VOLUME:
		SetVolumeMeasured(uEventParam);
		bRtnVal = true;
		break;		
	case EVENT_RX_TX_SIG:			
		SetRxTxSigMeasured(uEventParam);			
		bRtnVal = true;
		break;
	case EVENT_RXG_REVE_SIG:	
		SetRxgReveSigMeasured(uEventParam);
		bRtnVal = true;
		break;
	}
	return bRtnVal;
}

void DrawPerformance()
{
#if	1
	RECT rcClient = {0,0,LCD_RESOLUTION_X,LCD_RESOLUTION_Y};
	RECT rcBox = rcClient;
	char sFPS[64];
	sprintf(sFPS, "LPS: %04d/s, FPS: %04d/s @ clock: %lld", g_uLatestLPS, g_uLatestFPS, GetSystemClock());
	egl_font_set_color(Resource_GetFont(IDF_DEFAULT), MAKE_COLORREF(255,255,255));
	rcBox.x = rcClient.x - 1;	rcBox.y = rcClient.y;	DrawText(Resource_GetFont(IDF_DEFAULT), rcBox, sFPS, ALIGN_H_RIGHT | ALIGN_V_LOWER);
	rcBox.x = rcClient.x + 1;	rcBox.y = rcClient.y;	DrawText(Resource_GetFont(IDF_DEFAULT), rcBox, sFPS, ALIGN_H_RIGHT | ALIGN_V_LOWER);
	rcBox.x = rcClient.x;	rcBox.y = rcClient.y - 1;	DrawText(Resource_GetFont(IDF_DEFAULT), rcBox, sFPS, ALIGN_H_RIGHT | ALIGN_V_LOWER);
	rcBox.x = rcClient.x;	rcBox.y = rcClient.y + 1;	DrawText(Resource_GetFont(IDF_DEFAULT), rcBox, sFPS, ALIGN_H_RIGHT | ALIGN_V_LOWER);
	
	egl_font_set_color(Resource_GetFont(IDF_DEFAULT), MAKE_COLORREF(0,0,0));
	DrawText(Resource_GetFont(IDF_DEFAULT), rcClient, sFPS, ALIGN_H_RIGHT | ALIGN_V_LOWER);
#else
	int nX, nY;	
	nX = 375;
	nY = 250;
	char sFPS[64];
	sprintf(sFPS, "LPS: %04d/s @ clock: %d", g_uLatestLPS, GetSystemClock());
	egl_font_set_color(Resource_GetFont(IDF_DEFAULT), MAKE_COLORREF(255,255,255));
	bitfont_draw(Resource_GetFont(IDF_DEFAULT), nX + 1, nY, sFPS);
	bitfont_draw(Resource_GetFont(IDF_DEFAULT), nX - 1, nY, sFPS);
	bitfont_draw(Resource_GetFont(IDF_DEFAULT), nX, nY + 1, sFPS);
	bitfont_draw(Resource_GetFont(IDF_DEFAULT), nX, nY - 1, sFPS);	
	egl_font_set_color(Resource_GetFont(IDF_DEFAULT), MAKE_COLORREF(0,0,0));
	bitfont_draw(Resource_GetFont(IDF_DEFAULT), nX, nY, sFPS);
#endif
}
