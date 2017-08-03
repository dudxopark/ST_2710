
#include "adstar.h"
#include "egl/font/egl_font.h"

#include "P10_3940.h"
#include "ST_2710_GUI.h"
#include "ST_2710_GUI_Comm.h"

#include "Text.h"
#include "Event.h"

#include "LCDCtrl.h"
#include "P10_3940.h"
#include "ST_2710_Global.h"

#include "ResourceManager.h"

//SURFACE* g_pSurfBase = NULL;

u16 g_uLatestLPS, g_uLPS = 0;			// Loop Per Second. 초당 루프 회전수 측정
u16 g_uLatestFPS, g_uFPS = 0;			// Frame Per Second. 초당 화면 재생률 측정을 위한 변수

static int nPreviousScreen = SCREEN_SETTING;
static int nCurrentScreen = SCREEN_AM;

void SetCurrentScreen(int nScreen)
{
	nPreviousScreen = nCurrentScreen;
	nCurrentScreen = nScreen;
	PushEventA(EVENT_UPDATE_SCREEN, nCurrentScreen);
	PushEventA(EVENT_UPDATEDATA, false);
}

int GetCurrentScreen()
{
	return nCurrentScreen;
}

int GetPreviousScreen()
{
	return nPreviousScreen;
}

void InitGUI()
{
	setscreen(SCREEN_480x272, SCREENMODE_RGB565);

	SURFACE *frame1 = createframe(LCD_RESOLUTION_X, LCD_RESOLUTION_Y, 16);
	SURFACE *frame2 = createframe(LCD_RESOLUTION_X, LCD_RESOLUTION_Y, 16);
	
	memset(frame1->pixels, 0x00, frame1->pitch * frame1->h);
	memset(frame2->pixels, 0x00, frame2->pitch * frame2->h);

	setdoubleframebuffer(frame1, frame2);
	set_draw_target(getbackframe());	
	draw_rectfill(0, 0, LCD_RESOLUTION_X, LCD_RESOLUTION_Y, MAKE_COLORREF(0,0,0));
	set_draw_target(getfrontframe());	
	draw_rectfill(0, 0, LCD_RESOLUTION_X, LCD_RESOLUTION_Y, MAKE_COLORREF(0,0,0));
	flip();	
	
	Resource_Initialize();
	Resource_InitializeFont();
	
	SetLCDBackLight(ON);
}

void ReleaseGUI()
{
	Resource_ReleaseFont();
	Resource_Release();
}

void IntToDoubleStrWithCommaAndFillZero(int nValue, int nMaxValue, int nDotPos, char* pValue)
{
	// 최대 999,999.999999 까지
	int nThousand = 0, nOne;
	int nInteger, n10;
	if(nValue == 0xffffffff)
	{
		pValue[0] = '\0';
		return;
	}
	n10 = pow(10, nDotPos);
	nInteger = nValue / n10;		// 정수값		
	nThousand = nInteger / 1000;
	nOne = nInteger % 1000;

	char sRemainder[10] = "";
	char sOne[15] = "";
	if(nDotPos != 0)			// 소수점 이하 처리
	{
		int nRemainder;
		nRemainder = nValue % n10;		// 소수점 이하
		char sFommatter[15];
		sprintf(sFommatter, ".%%0%dd", nDotPos);			// To make .%01d/.%02d/.%03d/....
		sprintf(sRemainder, sFommatter, nRemainder);
	}			
	int nMaxInteger = nMaxValue / n10;		// 정수값
	int nMaxThousand = nMaxInteger / 1000;;
	int nMaxDotPos = 1;
	while(nMaxInteger /= 10)
	{
		nMaxDotPos++;
	}
	if(nMaxThousand == 0)
	{
		char sFommatter[15];
		sprintf(sFommatter, "%%0%dd", nMaxDotPos);			// To make %01d/%02d/%03d/....
		sprintf(sOne, sFommatter, nOne);
	}
	else
	{
		char sFommatter[15];
		sprintf(sFommatter, "%%0%dd,%%03d", nMaxDotPos - 3);			// To make %01d/%02d/%03d/....
		sprintf(sOne, sFommatter, nThousand, nOne);
	}
	sprintf(pValue, "%s%s", sOne, sRemainder);
}

void IntToDoubleStrWithComma(int nValue, int nDotPos, char* pValue)
{
#if	1
	// 최대 999,999.999999 까지
	int nThousand, nOne;
	int nInteger, nRemainder, n10;
	if(nValue == 0xffffffff)
	{
		pValue[0] = '\0';
		return;
	}
	n10 = pow(10, nDotPos);
	nInteger = nValue / n10;		// 정수값
	nRemainder = nValue % n10;		// 소수점 이하
	
	nThousand = nInteger / 1000;
	nOne = nInteger % 1000;

	char sRemainder[10] = "";
	char sOne[15] = "";
	if(nDotPos != 0)
	{
		char sFommatter[15];
		sprintf(sFommatter, ".%%0%dd", nDotPos);			// To make .%01d/.%02d/.%03d/....
		sprintf(sRemainder, sFommatter, nRemainder);
	}	
	if(nThousand == 0)
		sprintf(sOne, "%d", nOne);
	else
		sprintf(sOne, "%d,%03d", nThousand, nOne);
	
	if(strcmp(sOne, "0") == 0)			// 소수점 이하만 있으면
		sprintf(pValue, "%s", sRemainder);
	else
		sprintf(pValue, "%s%s", sOne, sRemainder);
#else
	int nThousand, nOne, nRemainder;
	nThousand = nValue / 10000;
	nOne = nValue % 10000 / 10;
	nRemainder = nValue % 10;
	if(nValue == 0xffffffff)
		pValue[0] = '\0';
	else if(nThousand == 0)
		sprintf(pValue, "%d.%01d", nOne, nRemainder);
	else
		sprintf(pValue, "%d,%03d.%01d", nThousand, nOne, nRemainder);		
#endif
}

void DrawText(EGL_FONT* font, RECT rcClient, const char* str, U8 uOption)
{
	// uOption ALIGN_LEFT = 0x01, ALIGN_H_RIGHT = 0x02, ALIGN_H_CENTER = 0x03
	int nMargin = 5;
	int nPosX, nPosY;
	int nWidth = text_width(font, str);
	int nHeight = font->h;
	if(uOption & ALIGN_H_LEFT)
		nPosX = rcClient.x + nMargin;
	else if(uOption & ALIGN_H_RIGHT)
		nPosX = rcClient.x + rcClient.w - nWidth - nMargin;
	else
		nPosX = round((rcClient.x + rcClient.x + rcClient.w - nWidth) / 2);
	
	if(uOption & ALIGN_V_UPPER)
		nPosY = rcClient.y + nMargin;
	else if(uOption & ALIGN_V_LOWER)
		nPosY = rcClient.y + rcClient.h - nHeight - nMargin;
	else
		nPosY = round((rcClient.y + rcClient.y + rcClient.h - nHeight) / 2);
	
	if(font == Resource_GetFont(IDF_DEFAULT))
		bitfont_draw(font, nPosX, nPosY, str);
	else
		bmpfont_draw(font, nPosX, nPosY, str);
}

void DrawRectFill(RECT rcClient, u32 uColor)
{
	draw_rectfill(rcClient.x, rcClient.y, rcClient.w, rcClient.h, uColor);
}

int nSelectedMenu = 0;
#define MAIN_MENU_SETUP_SIZE			8	

void DrawAllMenuItem(MENU *pMenu, U8 nSize, U8 nSelIdx, U32 nBClr, U32 nFClr, U32 nSelBClr, U32 nSelFClr, bool bRedraw);


void DrawSingleMenuItem(MENU *pMenu, bool bSelected, U32 nBClr, U32 nFClr, U32 nSelBClr, U32 nSelFClr, bool bRedraw)
{
//	char strTmp[50];
//	char strTmp2[50];
//	char strTmp3[50];
//	U8 nIdx = 0;
//	U8 nTmpVal = 0;

	int nStrW = 0;
	int nTxtYPos = pMenu->m_nMnuY+(pMenu->m_nMnuH - Resource_GetFont(IDF_DEFAULT)->h)/2;
	int nTmpW = 0;
	if(pMenu != NULL)
	{
		// Draw Background
		if(bRedraw == TRUE)
		{
			draw_rectfill(pMenu->m_nMnuX, pMenu->m_nMnuY, pMenu->m_nMnuW, pMenu->m_nMnuH, nBClr);
		}

		//nStrW =  text_width(Resource_GetFont(IDF_DEFAULT), pMenu->m_pName);
		nStrW = Resource_GetFont(IDF_DEFAULT)->text_width(Resource_GetFont(IDF_DEFAULT), pMenu->m_pName);
		// Draw Title
		if(bSelected == TRUE)
		{
			if(pMenu->m_nVarType == 0)
				nTmpW = nStrW;
			else
				nTmpW = (pMenu->m_nVarX - pMenu->m_nNameX);

			//draw_rectfill(pMenu->m_nNameX, nTxtYPos, nTmpW+10, Resource_GetFont(IDF_DEFAULT)->h, nSelBClr);
			draw_rectfill(pMenu->m_nMnuX, pMenu->m_nMnuY, pMenu->m_nMnuW, pMenu->m_nMnuH, nSelBClr);
			egl_font_set_color(Resource_GetFont(IDF_DEFAULT), nSelFClr);
			//bmpfont_draw( Resource_GetFont(IDF_DEFAULT) ,pMenu->m_nNameX ,nTxtYPos ,pMenu->m_pName);
			bitfont_draw( Resource_GetFont(IDF_DEFAULT) ,pMenu->m_nNameX ,nTxtYPos ,pMenu->m_pName);
		}
		else
		{
			egl_font_set_color(Resource_GetFont(IDF_DEFAULT), nFClr);
			//bmpfont_draw( Resource_GetFont(IDF_DEFAULT), pMenu->m_nNameX, nTxtYPos, pMenu->m_pName);
			bitfont_draw( Resource_GetFont(IDF_DEFAULT), pMenu->m_nNameX, nTxtYPos, pMenu->m_pName);
		}
			
#if	0
		// Draw variable
		if(pMenu->m_nVarSize >0 )
		{

			if(pMenu->m_nVarType == VT_DSP_LPF_TYPE)
			{
				memset(strTmp,0,50);
				sprintf(strTmp,"%d.%dKHz", *((U8 *)pMenu->m_pVarValue)/10,*((U8 *)pMenu->m_pVarValue)%10);
				//egl_font_set_color(G_pFnt_24Pt,nFClr);
				//bmpfont_draw( G_pFnt_24Pt ,pMenu->m_nVarX ,nTxtYPos ,strTmp);
			}
			else if(pMenu->m_nVarType == VT_ATU_VER)
			{
				memset(strTmp,0,50);
				nTmpVal = *((U8 *)pMenu->m_pVarValue);

				if(nTmpVal == 0)
				{
					sprintf(strTmp,"None");
				}
				else
				{
					sprintf(strTmp,"%02d.%d", nTmpVal/10, nTmpVal%10);
				}
				//egl_font_set_color(G_pFnt_24Pt,nFClr);
				//bmpfont_draw( G_pFnt_24Pt ,pMenu->m_nVarX ,nTxtYPos ,strTmp);
			}
			else if(pMenu->m_nVarType == VT_PRG_VER)
			{
				memset(strTmp,0,50);

				sprintf(strTmp,"M:v%02d.%02d, C:v%02d.%02d, DSP:v%02d.%d", SOFT_VER_MAJ, SOFT_VER_MIN, G_SysCfg.m_MicomVer.m_nMajor, G_SysCfg.m_MicomVer.m_nMinor, G_SysCfg.m_nDSPVer/10, G_SysCfg.m_nDSPVer%10);

				//egl_font_set_color(G_pFnt_24Pt,nFClr);
				//bmpfont_draw( G_pFnt_24Pt ,pMenu->m_nVarX ,nTxtYPos ,strTmp);
			}else if(pMenu->m_nVarType == VT_GPS_POSITION_LAT)
			{
				memset(strTmp,0,50);
				GetLatString(strTmp, G_SysCfg.m_GPSData.m_nEditLatDeg, G_SysCfg.m_GPSData.m_nEditLatExt);
			}else if(pMenu->m_nVarType == VT_GPS_POSITION_LON)
			{
				memset(strTmp,0,50);
				GetLonString(strTmp, G_SysCfg.m_GPSData.m_nEditLonDeg, G_SysCfg.m_GPSData.m_nEditLonExt);
			}else if(pMenu->m_nVarType == VT_GPS_UTC)
			{
				memset(strTmp,0,50);
				GetUtcString(strTmp, G_SysCfg.m_GPSData.m_nEditUTCValue);
			}else if(pMenu->m_nVarType == VT_DSC_DIST_FREQS)
			{
				memset(strTmp,0,50);
				GetDistFreqMenuString(strTmp, G_SysCfg.m_DistCompose.m_nDsc);
				//GetLonString(strTmp, G_SysCfg.m_GPSData.m_nEditLonDeg, G_SysCfg.m_GPSData.m_nEditLonExt);
			}else if(pMenu->m_nVarType == VT_NORMAL_MMSI)
			{
				memset(strTmp,0,50);
				GetNormalMMSIString(strTmp, (U8 *)pMenu->m_pVarValue);
			}else if(pMenu->m_nVarType == VT_CALL_DSC_FREQ)
			{
				memset(strTmp,0,50);
				GetCallDscFreqString(strTmp, *((U32 *)pMenu->m_pVarValue));
			}else if(pMenu->m_nVarType == VT_SUB_COMM_FREQ)
			{
				memset(strTmp,0,50);
				GetCallSubCommString(strTmp, (CHInfo*)pMenu->m_pVarValue);

			}else if(pMenu->m_nVarType == VT_SUB_BROAD_COMM_FREQ)
			{
				memset(strTmp,0,50);
				GetCallSubBroadCommString(strTmp, (CHInfo*)pMenu->m_pVarValue);
			}
			else if(pMenu->m_nVarType == VT_SUB_COMM_MODE)
			{
				memset(strTmp, 0, 50);
				GetCallModeString(strTmp, G_SysCfg.m_CallCompose.m_nMode);

			}else if(pMenu->m_nVarType == VT_AREA)
			{
				memset(strTmp, 0, 50);
				GetAreaString(strTmp, (U8*)pMenu->m_pVarValue);
			}else if(pMenu->m_nVarType == VT_DROBOSE_TO_MMSI)
			{
				memset(strTmp, 0, 50);
				if(G_SysCfg.m_DroboseCompose.m_ucFormat == F_INDIVIDUAL)
					GetNormalMMSIString(strTmp, (U8*)pMenu->m_pVarValue);
				else if(G_SysCfg.m_DroboseCompose.m_ucFormat == F_ALLSHIPS)
					strcpy(strTmp, "ALLSHIP");
				else
				{
					GetAreaString(strTmp, (U8*)pMenu->m_pVarValue);
				}

				
			}else if(pMenu->m_nVarType == VT_DROBOSE_DIST_MMSI)
			{
				memset(strTmp, 0, 50);
				GetDistMMSIString(strTmp, (U8*)pMenu->m_pVarValue);
			}else if(pMenu->m_nVarType == VT_DIST_POS_LAT)
			{
				memset(strTmp, 0, 50);
				GetLatString(strTmp, G_SysCfg.m_GPSData.m_nEditLatDeg, G_SysCfg.m_GPSData.m_nEditLatExt);
			}else if(pMenu->m_nVarType == VT_DIST_POS_LON)
			{
				memset(strTmp,0,50);
				GetLonString(strTmp, G_SysCfg.m_GPSData.m_nEditLonDeg, G_SysCfg.m_GPSData.m_nEditLonExt);
			}
			else if(pMenu->m_nVarType == VT_DIST_UTC)
			{
				memset(strTmp, 0, 50);
				GetDscUtcString(strTmp, (U8*)pMenu->m_pVarValue);
			}else if(pMenu->m_nVarType == VT_DIST_SUBCOMM)
			{
				memset(strTmp, 0, 50);
				memset(strTmp2, 0, 50);
				GetDscModeString(strTmp, *((U8*)pMenu->m_pVarValue));
				GetDscDistFreqString(strTmp2, *((U8*)pMenu->m_pVarValue), G_SysCfg.m_DroboseCompose.m_ucDistFreq);
				strcat(strTmp, " / ");
				strcat(strTmp, strTmp2);

			}else if(pMenu->m_nVarType == VT_DIST_DSC_FREQ)
			{
				int nIdx;
				memset(strTmp, 0, 50);
				nIdx = GetNormalDistFreqFlagToIndex(*((U8*)pMenu->m_pVarValue));
				
				strcpy(strTmp, G_StrDscFreqs[nIdx]);

			}else if(pMenu->m_nVarType == VT_DSC_TIMEOUT)
			{
				memset(strTmp, 0, 50);
				GetDscTimeoutString(strTmp, *((U32*) pMenu->m_pVarValue));
			}else if(pMenu->m_nVarType == VT_U8_NUMERIC)
			{
				
				memset(strTmp, 0, 50);
				sprintf(strTmp, "%d", *((U8*) pMenu->m_pVarValue));

			}
			else
			{
				
				nIdx = VarType2Idx(pMenu);
				//egl_font_set_color(G_pFnt_24Pt,nFClr);

				if(pMenu->m_nVarSize == 1)
				{
					strcpy(strTmp, (char *)pMenu->m_pVarMsg);
					//bmpfont_draw( G_pFnt_24Pt ,pMenu->m_nVarX ,nTxtYPos ,(char *)pMenu->m_pVarMsg);
				}
				else if(pMenu->m_nVarSize > 1)
				{
					strcpy(strTmp, (char *)pMenu->m_pVarMsg[nIdx]);
					//bmpfont_draw( G_pFnt_24Pt ,pMenu->m_nVarX ,nTxtYPos ,(char *)pMenu->m_pVarMsg[nIdx]);
				}					
			}

			sprintf(strTmp3 ,": %s", strTmp);

			/*if(bSelected == TRUE)
			{
				
				draw_rectfill(pMenu->m_nVarX-10, nTxtYPos, text_width(G_pFnt_24Pt,strTmp)+20, G_pFnt_24Pt->h, nSelBClr);
				egl_font_set_color(G_pFnt_24Pt,nSelFClr);
				bmpfont_draw( G_pFnt_24Pt ,pMenu->m_nVarX ,nTxtYPos ,strTmp);
			}
			else
			{
				egl_font_set_color(G_pFnt_24Pt,nFClr);
				bmpfont_draw( G_pFnt_24Pt ,pMenu->m_nVarX ,nTxtYPos ,strTmp);
			}*/
			if(bSelected == TRUE)
			{

				draw_rectfill(pMenu->m_nVarX-10, nTxtYPos, text_width(G_pFnt_24Pt,strTmp3)+20, G_pFnt_24Pt->h, nSelBClr);
				egl_font_set_color(G_pFnt_24Pt,nSelFClr);
				bmpfont_draw( G_pFnt_24Pt ,pMenu->m_nVarX ,nTxtYPos ,strTmp3);
			}
			else
			{
				egl_font_set_color(G_pFnt_24Pt,nFClr);
				bmpfont_draw( G_pFnt_24Pt ,pMenu->m_nVarX ,nTxtYPos ,strTmp3);
			}
		}
#endif
	}
}

void DrawAllMenuItem(MENU *pMenu,U8 nSize, U8 nSelIdx, U32 nBClr, U32 nFClr, U32 nSelBClr, U32 nSelFClr, bool bRedraw)
{
	int i = 0;
	bool bSelected = FALSE;

	if(pMenu != NULL)
	{
		for(i =0; i < nSize; i++)
		{
			if( i == nSelIdx)	bSelected = TRUE;
			else				bSelected = FALSE;
			DrawSingleMenuItem(&pMenu[i],bSelected, nBClr, nFClr, nSelBClr, nSelFClr, bRedraw);				
		}
	}
}

void DrawProc_Config()
{
#define MENU_GAP						28
#define MENU_Y_START					48
#define MENU_Y_POS(n)					(MENU_Y_START + (n) * MENU_GAP)
	MENU G_FnMainMenuSetup[MAIN_MENU_SETUP_SIZE] = {
	{SETTING_KEY_SOUND,				0, MENU_Y_POS(0),		480, MENU_GAP,		50, 300, 		0,	(void*)NULL,	0, (char**)NULL, NULL},
	{SETTING_INTERNAL_SPEAKER,		0, MENU_Y_POS(1),		480, MENU_GAP,		50, 300, 		0,	(void*)NULL,	0, (char**)NULL, NULL},
	{SETTING_EXTERNAL_SPEAKER,		0, MENU_Y_POS(2),		480, MENU_GAP,		50, 300, 		0,	(void*)NULL,	0, (char**)NULL, NULL},
	{SETTING_SCAN_MIN,				0, MENU_Y_POS(3),		480, MENU_GAP,		50, 300, 		0,	(void*)NULL,	0, (char**)NULL, NULL},
	{SETTING_SCAN_MAX,				0, MENU_Y_POS(4),		480, MENU_GAP,		50, 300, 		0,	(void*)NULL,	0, (char**)NULL, NULL},
	{SETTING_LANGUAGE,				0, MENU_Y_POS(5),		480, MENU_GAP,		50, 300, 		0,	(void*)NULL,	0, (char**)NULL, NULL},
	{SETTING_FACTORY_RESET,			0, MENU_Y_POS(6),		480, MENU_GAP,		50, 300, 		0,	(void*)NULL,	0, (char**)NULL, NULL},
	{SETTING_SYSTEM_INFO,			0, MENU_Y_POS(7),		480, MENU_GAP,		50, 300, 		0,	(void*)NULL,	0, (char**)NULL, NULL},
	};
	DrawAllMenuItem(G_FnMainMenuSetup, MAIN_MENU_SETUP_SIZE, nSelectedMenu, MAKE_COLORREF(0,0,0), MAKE_COLORREF(255,255,255), MAKE_COLORREF(255,255,255), MAKE_COLORREF(0,0,0), true);
}

bool EventProc_Config(u16 uEvent, u16 uParam)
{
	bool bRtnVal = false;
	int nTmp;
	switch(uEvent)
	{
	case EVENT_VKEY_SPOP:
		if(uParam == VKEY_ENTER)
		{
			if(nSelectedMenu == 5)
			{
			//	if(MsgBox("Factory Reset?(Y/N)") == false)
			//		break;
			//	FactoryReset();
			}
			bRtnVal = true;
		}
		else if(uParam == VKEY_CANCEL)
		{
			SetCurrentScreen(GetPreviousScreen());
			bRtnVal = true;
		}
		else if((uParam >= VKEY_NUM1) && (uParam <= VKEY_NUM9))
		{
			nTmp = uParam - VKEY_NUM1;
			if((nTmp < MAIN_MENU_SETUP_SIZE) && (nTmp >= 0))
			{
				nSelectedMenu = nTmp;
				PushEventA(EVENT_REDRAW, 0);
				bRtnVal = true;
			}
		}
		break;		
	case EVENT_ROTARY_SW:
		nTmp = nSelectedMenu + (signed char)uParam;
		if(nTmp >= MAIN_MENU_SETUP_SIZE)
			nSelectedMenu = MAIN_MENU_SETUP_SIZE - 1;
		else if(nTmp < 0)
			nSelectedMenu = 0;
		else			
			nSelectedMenu = nTmp;
		PushEventA(EVENT_REDRAW, 0);
		bRtnVal = true;
		break;
	};
	return bRtnVal;
}

void DrawRandom(RECT rcClient)
{
	draw_rectfill(120, 48, 360, 224, MAKE_COLORREF(0,0,0));
	srand((u16)GetSystemClock());
	
	int i;
	for(i = 0 ; i < 10 ; i++)
	{
		draw_circlefill(rand() % 480, rand() % 272, rand() % 100, MAKE_COLORREF(rand() % 255,rand() % 255,rand() % 255));
		draw_rect(rand() % 480, rand() % 272, rand() % 240, rand() % 136, MAKE_COLORREF(rand() % 255,rand() % 255,rand() % 255));
		draw_line(rand() % 480, rand() % 272, rand() % 480, rand() % 272, MAKE_COLORREF(rand() % 255,rand() % 255,rand() % 255));
	}
}

typedef struct
{
	int nItemCount;
	int nSelect;	
	int nPos;
	int nMaxVisible;
}LISTCTRL;

bool IsValidFrequency(ChannelManager* cm, int nFreq)
{
    if(nFreq < cm->nMinFreq)
        return false;
    if(nFreq > cm->nMaxFreq)
        return false;
	return true;
}

void CreateChannelManager(ChannelManager* cm, int nMaxCh, int nMinFreq, int nMaxFreq)
{
    (cm)->nMaxCh = nMaxCh;
    (cm)->nMinFreq = nMinFreq;
    (cm)->nMaxFreq = nMaxFreq;
    (cm)->arCh = (int*) malloc(sizeof(int) * nMaxCh);
    memset((cm)->arCh, 0xffffffff, sizeof(int) * nMaxCh);
}

void DestroyChannelManager(ChannelManager* cm, int nMaxCh, int nMinFreq, int nMaxFreq)
{
    free((cm)->arCh);
}

void DeleteChannel(ChannelManager* cm, int nCh)
{
    if(nCh <= cm->nMaxCh)  	// 최대값 검사해야 함
    {
        cm->arCh[nCh - 1] = INVALID_FREQUENCY;
    }
}

void DeleteAllChannels(ChannelManager* cm)
{
	int i;
	for(i = 0 ; i < cm->nMaxCh ; i++)
	{
		cm->arCh[i] = INVALID_FREQUENCY;
	}	
}

bool UpdateChannel(ChannelManager* cm, int nCh, int nFreq, int nOffset)
{
    if(nFreq < cm->nMinFreq)
        return false;
    if(nFreq > cm->nMaxFreq)
        return false;
    if(!(nCh <= cm->nMaxCh))		// 최대값 검사해야 함
		return false;
    if(nCh < 1)		
		return false;
	if(nFreq == INVALID_FREQUENCY)
		cm->arCh[nCh - 1] = INVALID_FREQUENCY;
	else
		cm->arCh[nCh - 1] = nFreq + nOffset;
		
    return true;
}

int GetChannel(ChannelManager* cm, int nCh, int nOffset)
{
    if(!(nCh <= cm->nMaxCh))		// 최대값 검사해야 함
		return INVALID_FREQUENCY;	
    if(nCh < 1)		
		return INVALID_FREQUENCY;
	int nFreq = cm->arCh[nCh - 1];
	
	if(nFreq == INVALID_FREQUENCY)
		return INVALID_FREQUENCY;
	else
		return nFreq + nOffset;
}

void DrawDesignSensitivity(bool bDay)
{
    //////////////////////////////////////////////////////////////////////////////////////////////////
    // 송/수신 감도
    int nTxRxSig, nRxgReveSig;

    // 배경 색칠하기
    RECT rcClient = {0, 0, 128, LCD_RESOLUTION_Y};
    u32 rgbBG = MAKE_COLORREF(235,235,235);
    u32 rgbScaleText = MAKE_COLORREF(100,100,100);
    u32 rgbTagBg = MAKE_COLORREF(0,0,0);
    u32 rgbTagText = MAKE_COLORREF(200,200,200);
    if(!bDay)
    {
        rgbBG = MAKE_COLORREF(35,35,35);
        rgbScaleText = MAKE_COLORREF(150,150,150);
        rgbTagBg = MAKE_COLORREF(61,61,61);
        rgbTagText = MAKE_COLORREF(200,200,200);
    }
    draw_rectfill(rcClient.x, rcClient.y, rcClient.w, rcClient.h, rgbBG);

    RECT rcSens;
    SURFACE* pSurfSensitivityFg = Resource_GetSurface(IDR_CTRL_SENSITIVITY_FG);
    // 좌측 막대 그래프 그리기
    rcSens.x = rcClient.x + 20;
    rcSens.y = rcClient.y + 15;
    rcSens.w = pSurfSensitivityFg->w;
    rcSens.h = pSurfSensitivityFg->h;
    // 배경
    drawsurface(Resource_GetSurface(IDR_CTRL_SENSITIVITY_BG), rcSens.x, rcSens.y);
    // 전경
    nTxRxSig = GetRxTxSig() * rcSens.h;
    //debugprintf("%d, %d, %d, %d, %d, %d, %d\r\n", nTxRxSig, rcSens.x, rcSens.y + rcSens.h - nTxRxSig, 0, rcSens.h - nTxRxSig, pSurfSensitivityFg->w, nTxRxSig);
    if(nTxRxSig != 0)		// draw_surface_rect()의 마지막 인수가 0일 경우 에러 발생함.
        draw_surface_rect(Resource_GetSurface(IDR_CTRL_SENSITIVITY_FG), rcSens.x, rcSens.y + rcSens.h - nTxRxSig, 0, rcSens.h - nTxRxSig, pSurfSensitivityFg->w, nTxRxSig);

    // 우측 막대 그래프 그리기
    rcSens.x = rcClient.x + 85;
    rcSens.y = rcClient.y + 15;
    rcSens.w = pSurfSensitivityFg->w;
    rcSens.h = pSurfSensitivityFg->h;
    // 배경
    drawsurface(Resource_GetSurface(IDR_CTRL_SENSITIVITY_BG), rcSens.x, rcSens.y);
    // 전경
    nRxgReveSig = GetRxgReveSig() * rcSens.h;
    if(nRxgReveSig != 0)		// draw_surface_rect()의 마지막 인수가 0일 경우 에러 발생함.
        draw_surface_rect(Resource_GetSurface(IDR_CTRL_SENSITIVITY_FG), rcSens.x, rcSens.y + rcSens.h - nRxgReveSig, 0, rcSens.h - nRxgReveSig, pSurfSensitivityFg->w, nRxgReveSig);

    rcSens.x = rcClient.x + (20 + 85 + pSurfSensitivityFg->w) / 2;
    rcSens.y = rcClient.y + 15;
    rcSens.w = pSurfSensitivityFg->w;
    rcSens.h = pSurfSensitivityFg->h;

    // 눈금 그리기
    egl_font_set_color(Resource_GetFont(IDF_DEFAULT), rgbScaleText);
    bitfont_draw(Resource_GetFont(IDF_DEFAULT), rcSens.x - 12, rcSens.y - 6,					"100");
    bitfont_draw(Resource_GetFont(IDF_DEFAULT), rcSens.x - 7, 	rcSens.y + rcSens.h / 2 - 7, 	"50");
    bitfont_draw(Resource_GetFont(IDF_DEFAULT), rcSens.x - 2,	rcSens.y + rcSens.h - 8, 		"0");

    // 막대 그래프 태그 그리기
    RECT rcBox = {0, 0, 58, 30};
    rcBox.x = rcClient.x + 4;
    rcBox.y = rcClient.y + rcClient.h - rcBox.h - 4;
    draw_roundrectfill(rcBox.x, rcBox.y, rcBox.w, rcBox.h, 3, rgbTagBg);
    rcBox.x += rcBox.w + 4;
    draw_roundrectfill(rcBox.x, rcBox.y, rcBox.w, rcBox.h, 3, rgbTagBg);

    egl_font_set_color(Resource_GetFont(IDF_24), rgbTagText);

    if(GetTRxMode())
    {
        rcBox.x -= rcBox.w + 4;
        DrawText(Resource_GetFont(IDF_24), rcBox, "송신", ALIGN_H_CENTER | ALIGN_V_CENTER);
        rcBox.x += rcBox.w + 4;
        DrawText(Resource_GetFont(IDF_24), rcBox, "반사", ALIGN_H_CENTER | ALIGN_V_CENTER);
    }
    else
    {
        rcBox.x -= rcBox.w + 4;
        DrawText(Resource_GetFont(IDF_24), rcBox, "수신", ALIGN_H_CENTER | ALIGN_V_CENTER);
        rcBox.x += rcBox.w + 4;
        DrawText(Resource_GetFont(IDF_24), rcBox, "감도", ALIGN_H_CENTER | ALIGN_V_CENTER);
    }
}

void DrawDesignStatusBar(bool bDay)
{
    //////////////////////////////////////////////////////////////////////////////////////////////////
    // 상태바
    RECT rcStatusBar = {128, 0, 352, 48};
    int nVoltage;
    char sVoltage[10] = "";

    char* sMode;
    char* arMode[] = {"SSB", "H3E", "AM", "FM", "VHF", "AMP"};
    int nMode = GetCurrentOpMode();

    //u32 rgbFG = MAKE_COLORREF(30,30,50);
    //u32 rgbSeperator = MAKE_COLORREF(5,5,10);
    u32 rgbBG = MAKE_COLORREF(255,255,255);
    u32 rgbText = MAKE_COLORREF(0,0,0);
    u32 rgbModeBG = MAKE_COLORREF(35,35,35);
    u32 rgbModeFG = MAKE_COLORREF(255,255,255);
    u32 rgbModeText = MAKE_COLORREF(0,0,255);

    if(!bDay)
    {
        rgbBG = MAKE_COLORREF(0,0,0);
        rgbText = MAKE_COLORREF(255,255,255);
        rgbModeBG = MAKE_COLORREF(0,0,127);
		rgbModeFG = MAKE_COLORREF(0,0,0);
        rgbModeText = MAKE_COLORREF(0,0,127);
    }

    sMode = arMode[nMode];
    //nVoltage = (int)(GetVoltage() * 10 + .5);
    //sprintf(sVoltage, "%d.%0dV  ", nVoltage / 10, nVoltage % 10);
	sprintf(sVoltage, "%.1fV", GetVoltage());
	

    draw_rectfill(rcStatusBar.x, rcStatusBar.y, rcStatusBar.w, rcStatusBar.h, rgbBG);
    //draw_hline(rcStatusBar.x, rcStatusBar.y + rcStatusBar.h, rcStatusBar.x + rcStatusBar.w, rgbSeperator);

    egl_font_set_color(Resource_GetFont(IDF_DEFAULT), rgbText);

    RECT rcMode = {150, 4, 70, 40};
    draw_roundrectfill(rcMode.x, rcMode.y, rcMode.w, rcMode.h, 5, rgbModeBG);
    rcMode.x += 2;
    rcMode.y += 2;
    rcMode.w -= 4;
    rcMode.h -= 4;
    draw_roundrectfill(rcMode.x, rcMode.y, rcMode.w, rcMode.h, 5, rgbModeFG);
	egl_font_set_color(Resource_GetFont(IDF_24), rgbModeText);
    DrawText(Resource_GetFont(IDF_24), rcMode, sMode, ALIGN_H_CENTER | ALIGN_V_CENTER);

    //egl_font_set_color(Resource_GetFont(IDF_24), rgbModeText);
    /*rcMode.x += 1;	DrawText(Resource_GetFont(IDF_24), rcMode, sMode, ALIGN_H_CENTER | ALIGN_V_CENTER);		rcMode.x -= 1;
    rcMode.x -= 1;	DrawText(Resource_GetFont(IDF_24), rcMode, sMode, ALIGN_H_CENTER | ALIGN_V_CENTER);		rcMode.x += 1;
    rcMode.y += 1;	DrawText(Resource_GetFont(IDF_24), rcMode, sMode, ALIGN_H_CENTER | ALIGN_V_CENTER);		rcMode.y -= 1;
    rcMode.y -= 1;	DrawText(Resource_GetFont(IDF_24), rcMode, sMode, ALIGN_H_CENTER | ALIGN_V_CENTER);		rcMode.y += 1;*/
    //egl_font_set_color(Resource_GetFont(IDF_24), rgbModeBG);
    //DrawText(Resource_GetFont(IDF_64), rcMode, sMode, ALIGN_H_CENTER | ALIGN_V_CENTER);


    int nIconSize = 24;
    int nIconOffset = (48 - nIconSize) / 2;

    if(IsRxMute())
    {
        drawsurface(Resource_GetSurface(IDR_ICO_RXMUTEON), rcStatusBar.x + rcStatusBar.w - 5 * 48 + nIconOffset, rcStatusBar.y + nIconOffset);
    }
    else
    {
        drawsurface(Resource_GetSurface(IDR_ICO_RXMUTEOFF), rcStatusBar.x + rcStatusBar.w - 5 * 48 + nIconOffset, rcStatusBar.y + nIconOffset);
    }
    // if(GetBKStatus())
    // {
    // drawsurface(Resource_GetSurface(IDR_ICO_BK), rcStatusBar.x + rcStatusBar.w - 5 * 48 + nIconOffset, rcStatusBar.y + nIconOffset);
    // }
    //if(IsBeepEnable())
    //{
    //drawsurface(Resource_GetSurface(IDR_ICO_BEEP), rcStatusBar.x + rcStatusBar.w - 3 * 48 + nIconOffset, rcStatusBar.y + nIconOffset);
    //}
    if(IsToneEnable())
    {
        drawsurface(Resource_GetSurface(IDR_ICO_TONEON), rcStatusBar.x + rcStatusBar.w - 4 * 48 + nIconOffset, rcStatusBar.y + nIconOffset);
    }
    egl_font_set_color(Resource_GetFont(IDF_24), rgbText);
    DrawText(Resource_GetFont(IDF_24), rcStatusBar, sVoltage, ALIGN_H_RIGHT);

    char sBright[5] = "";
    sprintf(sBright, "¤%d", abs(g_stOV.nBright));
    RECT rcText;
    rcText.x = rcStatusBar.x + rcStatusBar.w - 48 * 3;
    rcText.y = rcStatusBar.y;
    rcText.w = 48;
    rcText.h = 48;
    DrawText(Resource_GetFont(IDF_24), rcText, sBright, ALIGN_H_CENTER | ALIGN_V_CENTER);
}
