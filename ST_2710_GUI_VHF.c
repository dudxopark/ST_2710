
#include "adstar.h"
#include "ST_2710.h"
#include "ST_2710_GUI.h"
#include "ST_2710_GUI_VHF.h"
#include "ST_2710_Global.h"
#include "ResourceManager.h"
#include "P10_3940.h"
#include "Event.h"
#include "ScrollEdit.h"
#include "Key.h"
#include "util.h"
#include "ChannelScanner.h"

#define MIN_FREQ_VHF		150000					// 150.000 MHz
#define MAX_FREQ_VHF		165000					// 165.000 MHz
#define MAX_CH_VHF			64

typedef struct
{
	u16 nCh;
	int nFreq;
}VHF_CH;

// 16번 채널과 70번 채널은 DSC 표기 필요
const VHF_CH arChVHF[MAX_CH_VHF] = {
{1,		160650}, {2,	160700}, {3,	160750}, {4,	160800}, {5,	160850}, {2006,	160900}, {7,	160950}, {8,	161000}, {9,	161050}, {10,	161100}, 
{11,	161150}, {12,	161200}, {13,	161250}, {14,	161300}, {15,	161350}, {16,	161400}, {17,	161450}, {18,	161500}, {19,	161550}, {1019,	156950}, {2019,	161550}, {20,	161600}, {1020,	157000}, {2020,	161600}, 
{21,	161650}, {22,	161700}, {23,	161750}, {24,	161800}, {25,	161850}, {26,	161900}, {27,	161950}, {28,	162000},
{60,	160625}, {61,	160675}, {62,	160725}, {63,	160775}, {64,	160825}, {65,	160875}, {66,	160925}, {67,	156375}, {68,	156425}, {69,	156475},
{70,	156525}, {71,	156575}, {72,	156625}, {73,	156675}, {74,	156725}, {75,	156775}, {76,	156825}, 				 {78,	161525}, {1078,	156925}, {2078,	161525}, {79,	161575}, {1079,	156975}, {2079,	161575},
{80,	161625}, {81,	161675}, {82,	161725}, {83,	161775}, {84,	161825}, {85,	161875}, {86,	161925}, {87,	157375}, {88,	157425}, 
};  	   

static u16 uChVHF = 1;
static u32 uFreqVHF = 160650;	// 주파수 표시용 변수: 160.650 MHz
static ScrollEdit editChIndex = {IDR_EDIT_CH_INDEX, false};

int VHF_ChannelIterator(int nIterator);

T_CH_SCAN stChScanVHF = {VHF_ChannelIterator, SetVHFFreq};	

int VHF_ChannelIterator(int nIterator)
{
	if(nIterator < MAX_CH_VHF)
		return arChVHF[nIterator].nFreq;
	return -1;
}

int VHF_GetChannel(int nIndex)
{
	if(nIndex < MAX_CH_VHF)
		return arChVHF[nIndex].nCh;
	return -1;
}

int VHF_GetChannelFreq(int nChNum)
{
	int i;
	for(i = 0 ; i < MAX_CH_VHF ; i++)
	{
		if(arChVHF[i].nCh == nChNum) 
		{
			return arChVHF[i].nFreq;
		}
	}	
	return INVALID_FREQUENCY;
}

bool VHF_IsValidFrequency(int nFreq)
{
	if((MIN_FREQ_VHF <= nFreq) && (nFreq <= MIN_FREQ_VHF))
		return true;
	return false;
}

void UpdateData_VHF(bool bApplyToVariable)
{
   if(bApplyToVariable)  			// 화면 값을 변수에 저장
    {
		uChVHF = VHF_GetChannel(editChIndex.nValue);
		uFreqVHF = VHF_GetChannelFreq(uChVHF);
    }
    else  							// 변수 값을 화면으로...
    {
        //editChIndex.nValue = uChVHF;
    }
}

void InitGUI_VHF()
{
    editChIndex.nID = IDR_EDIT_CH_INDEX;
	editChIndex.nDotPos = 3;
    editChIndex.nMode = EDIT_MODE_DONE;
    editChIndex.nMinValue = 0;
    editChIndex.nMaxValue = MAX_CH_VHF - 1;

    UpdateData_VHF(false);
}

bool EventProc_VHFChild(u16 uEvent, u16 uParam)
{
    bool bRtnVal = false;
    bRtnVal = Edit_EventProc(&editChIndex, uEvent, uParam);
    return bRtnVal;
}

bool EventProc_VHF(u16 uEvent, u16 uParam)
{
    bool bRtnVal = false;
    int nScreen = GetCurrentScreen();
    if(nScreen != SCREEN_VHF)
        return false;
    if(EventProc_VHFChild(uEvent, uParam))
        return true;
    switch(uEvent)
    {
	case EVENT_UPDATE_SCREEN:
		bRtnVal = true;		
		break;			
	case EVENT_UPDATEDATA:
		if(uParam)
			UpdateData_VHF(true);
		else
			UpdateData_VHF(false);
		bRtnVal = true;		
		break;
    case EVENT_CH_SCAN:
	{
		int nResult;
		if((nResult = ChScan_DoScan(&stChScanVHF)) != 2)
		{
			editChIndex.nValue = ChScan_GetCurrentScan(&stChScanVHF);
			uChVHF = VHF_GetChannel(editChIndex.nValue);
			uFreqVHF = VHF_GetChannelFreq(uChVHF);
			
			if(nResult != 1)
 				RegisterTCB(stChScanVHF.nScanGap_ms, (void (*)(int))PushEvent, EVENT_CH_SCAN << 16);
           bRtnVal = true;
		}
        break;
	}
    case EVENT_EDIT_DONE:
 		if(uParam == editChIndex.nID)
		{
			uChVHF = VHF_GetChannel(editChIndex.nValue);
			uFreqVHF = VHF_GetChannelFreq(uChVHF);
			if(VHF_IsValidFrequency(uFreqVHF))
				SetVHFFreq(uFreqVHF);
			bRtnVal = true;		
		}
        break;
    case EVENT_VKEY_LPUSH:
		if(uParam == VKEY_TONE)
        {
            if(ChScan_IsOnScaning(&stChScanVHF))
			{
                ChScan_AbortScan(&stChScanVHF);
			}
            else
			{
				ChScan_StartScan(&stChScanVHF, editChIndex.nValue, 0, MAX_CH_VHF, 100);
 				RegisterTCB(1, (void (*)(int))PushEvent, EVENT_CH_SCAN << 16);
			}
            bRtnVal = true;
        }
        break;
    case EVENT_VKEY_SPOP:
        switch(uParam)
        {
        case VKEY_TONE:
            if(ChScan_IsOnScaning(&stChScanVHF))
            {
                ChScan_AbortScan(&stChScanVHF);
                bRtnVal = true;
            }
            break;
		}
        break;
    case EVENT_PTT:
        if(ChScan_IsOnScaning(&stChScanVHF))
        {
            ChScan_AbortScan(&stChScanVHF);
            bRtnVal = true;
        }
        break;
	case EVENT_ROTARY_SW:
        break;
    }
    return bRtnVal;
}

void DrawProc_VHF()
{
    bool bDay = false;
    DrawDesignSensitivity(bDay);
    DrawDesignStatusBar(bDay);
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 메인
    int nStaticX, nStaticY;
	char sFreqUnit[5] = "MHz";
    RECT rcClient;
    u32 rgbBG = MAKE_COLORREF(255,255,255);
    U32	rgbFG = MAKE_COLORREF(0,0,0);
    U32	rgbFreq = MAKE_COLORREF(229,115,44);
    if(!bDay)
    {
        rgbBG = MAKE_COLORREF(0,0,0);
        rgbFG = MAKE_COLORREF(255,255,255);
        rgbFreq = MAKE_COLORREF(229,115,44);
    }
    nStaticX = 58;
    nStaticY = 25;
    rcClient.x = 128;
    rcClient.y = 48;
    rcClient.w = 352;
    rcClient.h = 224;
    draw_rectfill(rcClient.x, rcClient.y, rcClient.w, rcClient.h, rgbBG);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 그려질 값 정리
	char sChPre[4] = "";
	char sCh[4] = "";
	char sFreq[12] = "";
	if(uChVHF > 100)
	{
		sprintf(sChPre, "%d", uChVHF / 100);
		sprintf(sCh, "%02d", uChVHF % 100);
	}
	else
	{
		sprintf(sCh, "%02d", uChVHF);
	}
	IntToDoubleStrWithComma(uFreqVHF, 0, sFreq);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 채널 값 그리기
    RECT rcCh, rcChPre, rcChStatic;
    rcCh.x = 345;
    rcCh.y = 50;
    rcCh.w = 125;
    rcCh.h = 74;
	rcChPre.x = 340;
    rcChPre.y = 80;
    rcChPre.w = 50;
    rcChPre.h = 35;
    rcChStatic.x = rcCh.x - nStaticX;
    rcChStatic.y = rcCh.y + 5;
    rcChStatic.w = nStaticX;
    rcChStatic.h = nStaticY;

	egl_font_set_color(Resource_GetFont(IDF_24), rgbFG);
    DrawText(Resource_GetFont(IDF_24), rcChPre, sChPre, ALIGN_H_RIGHT | ALIGN_V_LOWER);
	egl_font_set_color(Resource_GetFont(IDF_64), rgbFG);
    DrawText(Resource_GetFont(IDF_64), rcCh, sCh, ALIGN_H_RIGHT | ALIGN_V_LOWER);

    if(GetLastKey() == VKEY_DISTRESS)				// 시험용
	{
        draw_roundrect(rcCh.x, rcCh.y, rcCh.w, rcCh.h, 5, rgbFG);
        draw_roundrect(rcChPre.x, rcChPre.y, rcChPre.w, rcChPre.h, 5, rgbFG);
	}

    egl_font_set_color(Resource_GetFont(IDF_24), rgbFG);
    DrawText(Resource_GetFont(IDF_24), rcChStatic, "CH", ALIGN_H_CENTER | ALIGN_V_CENTER);
    draw_roundrect(rcChStatic.x, rcChStatic.y, rcChStatic.w, rcChStatic.h, 5, rgbFG);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 주파수 값 그리기
    RECT rcFreq, rcFreqStatic;
    rcFreq.x = 150;
    rcFreq.y = 115;
    rcFreq.w = 320;
    rcFreq.h = 90;
    rcFreqStatic.x = rcFreq.x;
    rcFreqStatic.y = rcFreq.y - nStaticY;
    rcFreqStatic.w = nStaticX;
    rcFreqStatic.h = nStaticY;

    egl_font_set_color(Resource_GetFont(IDF_80), rgbFreq);
    DrawText(Resource_GetFont(IDF_80), rcFreq, sFreq, ALIGN_H_RIGHT | ALIGN_V_LOWER);

    if(GetLastKey() == VKEY_DISTRESS)				// 시험용
        draw_roundrect(rcFreq.x, rcFreq.y, rcFreq.w, rcFreq.h, 5, rgbFreq);

    egl_font_set_color(Resource_GetFont(IDF_24), rgbFreq);
    DrawText(Resource_GetFont(IDF_24), rcFreqStatic, sFreqUnit, ALIGN_H_CENTER | ALIGN_V_CENTER);
    draw_roundrect(rcFreqStatic.x, rcFreqStatic.y, rcFreqStatic.w, rcFreqStatic.h, 5, rgbFreq);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 부가정보
	char sText[32] = "";
	if(uChVHF == 16)
		sprintf(sText, "DISTRESS, SAFETY & CALLING");
	else if(uChVHF == 70)
		sprintf(sText, "DSC for distress/safety/calling");
	RECT rcText;
	rcText.x = rcClient.x;
	rcText.y = rcFreq.y + rcFreq.h;
	rcText.w = rcClient.w;
	rcText.h = rcClient.y + rcClient.h - rcText.y;
    egl_font_set_color(Resource_GetFont(IDF_24), rgbFG);
    DrawText(Resource_GetFont(IDF_24), rcText, sText, ALIGN_H_CENTER | ALIGN_V_CENTER);	
	
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 송수신 상태
    //char sTxStatus[5];
    // RECT rcTxStatus;
    // if(GetTRxMode())
    // {
        // strcpy(sTxStatus, "송신");
        // rcTxStatus.x = rcFreqStatic.x + nStaticX + 10;
        // rcTxStatus.y = rcChStatic.y;
        // rcTxStatus.w = nStaticX;
        // rcTxStatus.h = nStaticY;
    // }
    // else
    // {
        // strcpy(sTxStatus, "수신");
        // rcTxStatus.x = rcFreqStatic.x + nStaticX + 10;
        // rcTxStatus.y = rcFreqStatic.y;
        // rcTxStatus.w = nStaticX;
        // rcTxStatus.h = nStaticY;
    // }
    // egl_font_set_color(Resource_GetFont(IDF_24), rgbFG);
    // DrawText(Resource_GetFont(IDF_24), rcTxStatus, sTxStatus, ALIGN_H_CENTER | ALIGN_V_CENTER);
    // draw_roundrect(rcTxStatus.x, rcTxStatus.y, rcTxStatus.w, rcTxStatus.h, 5, rgbFG);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 편집 중인 항목 표시
    char sEditing[10] = "";
    RECT rcEditing;
	strcpy(sEditing, "CH");
	rcEditing = rcChStatic;
	draw_roundrectfill(rcEditing.x, rcEditing.y, rcEditing.w, rcEditing.h, 5, rgbFG);
	
    egl_font_set_color(Resource_GetFont(IDF_24), rgbBG);
    DrawText(Resource_GetFont(IDF_24), rcEditing, sEditing, ALIGN_H_CENTER | ALIGN_V_CENTER);
    if(ChScan_IsOnScaning(&stChScanVHF) && CheckToggleValue(TOGGLE_FLAG_500))
    {
        RECT rcScan;
        rcScan.x = rcChStatic.x;
        rcScan.y = rcFreqStatic.y;
        rcScan.w = nStaticX;
        rcScan.h = nStaticY;
        draw_roundrect(rcScan.x, rcScan.y, rcScan.w, rcScan.h, 5, rgbFG);
        egl_font_set_color(Resource_GetFont(IDF_24), rgbFG);
        DrawText(Resource_GetFont(IDF_24), rcScan, "Scan", ALIGN_H_CENTER | ALIGN_V_CENTER);
    }
}


