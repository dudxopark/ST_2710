#include "adstar.h"
#include "ST_2710.h"
#include "ST_2710_GUI.h"
#include "ST_2710_GUI_Comm.h"
#include "ST_2710_Global.h"
#include "ResourceManager.h"
#include "P10_3940.h"
#include "Event.h"
#include "ScrollEdit.h"
#include "Key.h"
#include "util.h"
#include "ChannelScanner.h"

// SSB와 H3E는 100 Hz 스텝으로 주파수 변경 가능
#define MIN_FREQ_SSB		5000					// 500 kHz
#define MAX_FREQ_SSB		299999					// 29,999.900 kHz

#define MIN_FREQ_H3E		4986		//5000					// 500 kHz
#define MAX_FREQ_H3E		299985		//299999				// 29,999.900 kHz

#define MIN_CH_SSB			1
#define MAX_CH_SSB			300

#define MIN_CH_H3E			1
#define MAX_CH_H3E			300

#define MIN_BFO				-150
#define MAX_BFO				150

static u16 uChSSB = 1;
static u16 uChH3E = 1;

static u32 uFreqSSB = 270000;		// 주파수 표시용 변수: 27,000.000 kHz			500 kHz ~ 29,999.900 kHz		Step: 100Hz
static u32 uFreqH3E = 269986;		// 주파수 표시용 변수: 26,998.600 kHz			500 kHz ~ 29,999.900 kHz		Step: 100Hz

static s32 iBFOUI = 0;

#define INPUT_ITEM_CH			0
#define INPUT_ITEM_FREQ			1
#define INPUT_ITEM_BFO			2

static int nInputItem = INPUT_ITEM_CH;
static ScrollEdit editCh = {IDR_EDIT_CH, true};
static ScrollEdit editFreq = {IDR_EDIT_FREQ, false};

ChannelManager cmComm;

int J3E_ChannelIterator(int nIterator);
int H3E_ChannelIterator(int nIterator);

T_CH_SCAN stChScanJ3E = {J3E_ChannelIterator, SetSSBFreq};	
T_CH_SCAN stChScanH3E = {H3E_ChannelIterator, SetH3EFreq};

int J3E_ChannelIterator(int nIterator)
{
	return GetChannel(&cmComm, nIterator, 0);
}

int H3E_ChannelIterator(int nIterator)
{
	return GetChannel(&cmComm, nIterator, -14);
}

void UpdateData_Comm(bool bApplyToVariable)
{
    int nScreen = GetCurrentScreen();
    if(bApplyToVariable)  			// 화면 값을 변수에 저장
    {
        if(nScreen == SCREEN_J3E)
        {
            uChSSB = editCh.nValue;
            uFreqSSB = editFreq.nValue;
        }
        else
        {
            uChH3E = editCh.nValue;
            uFreqH3E = editFreq.nValue;
        }
    }
    else  							// 변수 값을 화면으로...
    {
        if(nScreen == SCREEN_J3E)
        {
            editCh.nValue = uChSSB;
            editFreq.nValue = uFreqSSB;
        }
        else
        {
            editCh.nValue = uChH3E;
            editFreq.nValue = uFreqH3E ;
        }
    }
}

void InitGUI_Comm()
{
    editCh.nID = IDR_EDIT_CH;
	editCh.nDotPos = 0;
    editCh.nMode = EDIT_MODE_DONE;
    editCh.nMinValue = MIN_CH_SSB;
    editCh.nMaxValue = MAX_CH_SSB;

    editFreq.nID = IDR_EDIT_FREQ;
	editFreq.nDotPos = 1;
    editFreq.nMode = EDIT_MODE_DONE;
    editFreq.nMinValue = MIN_FREQ_SSB;
    editFreq.nMaxValue = MAX_FREQ_SSB;

    CreateChannelManager(&cmComm, MAX_CH_SSB, MIN_FREQ_SSB, MAX_FREQ_SSB);
    UpdateChannel(&cmComm, 1, 278224, 0);
    UpdateChannel(&cmComm, 2, 277904, 0);
    UpdateChannel(&cmComm, 3, 278064, 0);
    UpdateChannel(&cmComm, 4, 278384, 0);
    UpdateChannel(&cmComm, 5, 278574, 0);
    UpdateChannel(&cmComm, 6, 278704, 0);
    UpdateChannel(&cmComm, 7, 278864, 0);
    UpdateChannel(&cmComm, 8, 279024, 0);

    uChSSB = 1;
    uChH3E = 1;
    uFreqSSB = 278224;
    uFreqH3E = 278210;

    UpdateData_Comm(false);
}

bool EventProc_CommChild(u16 uEvent, u16 uParam)
{
    bool bRtnVal = false;

    if(nInputItem == INPUT_ITEM_CH)
    {
        bRtnVal = Edit_EventProc(&editCh, uEvent, uParam);
    }
    else if(nInputItem == INPUT_ITEM_FREQ)
    {
        bRtnVal = Edit_EventProc(&editFreq, uEvent, uParam);
    }
    return bRtnVal;
}

bool EventProc_Comm(u16 uEvent, u16 uParam)
{
    bool bRtnVal = false;
    int nScreen = GetCurrentScreen();
    if((nScreen != SCREEN_J3E) && (nScreen != SCREEN_H3E))
        return false;
    if(EventProc_CommChild(uEvent, uParam))
        return true;
    switch(uEvent)
    {
	case EVENT_UPDATE_SCREEN:
		if(uParam == SCREEN_J3E)
		{
			editCh.nDotPos = 0;			
			editCh.nMinValue = MIN_CH_SSB;
			editCh.nMaxValue = MAX_CH_SSB;

			editFreq.nDotPos = 1;
			editFreq.nMinValue = MIN_FREQ_SSB;
			editFreq.nMaxValue = MAX_FREQ_SSB;
			bRtnVal = true;		
        }
        else if(uParam == SCREEN_H3E)
        {
			editCh.nDotPos = 0;			
			editCh.nMinValue = MIN_CH_H3E;
			editCh.nMaxValue = MAX_CH_H3E;

			editFreq.nDotPos = 1;
			editFreq.nMinValue = MIN_FREQ_H3E;
			editFreq.nMaxValue = MAX_FREQ_H3E;
			bRtnVal = true;		
        }
		break;				
	case EVENT_UPDATEDATA:
		if(uParam)
			UpdateData_Comm(true);
		else
			UpdateData_Comm(false);
		bRtnVal = true;		
		break;
    case EVENT_CH_SCAN:
		if(nScreen == SCREEN_J3E)
		{
			int nResult;		
			if((nResult = ChScan_DoScan(&stChScanJ3E)) != 2)
			{
				editCh.nValue = ChScan_GetCurrentScan(&stChScanJ3E);
				PushEventA(EVENT_EDIT_CHANGED, editCh.nID);
				if(nResult != 1)
					RegisterTCB(stChScanJ3E.nScanGap_ms, (void (*)(int))PushEvent, EVENT_CH_SCAN << 16);
				bRtnVal = true;			
			}
		}
		else if(nScreen == SCREEN_H3E)
		{
			int nResult;		
			if((nResult = ChScan_DoScan(&stChScanH3E)) != 2)
			{
				editCh.nValue = ChScan_GetCurrentScan(&stChScanH3E);
				PushEventA(EVENT_EDIT_CHANGED, editCh.nID);
				if(nResult != 1)
					RegisterTCB(stChScanH3E.nScanGap_ms, (void (*)(int))PushEvent, EVENT_CH_SCAN << 16);
				bRtnVal = true;
			}			
		}
        break;
    case EVENT_PTT:
        if(ChScan_IsOnScaning(&stChScanJ3E))
        {
            ChScan_AbortScan(&stChScanJ3E);
            bRtnVal = true;
        }
        if(ChScan_IsOnScaning(&stChScanH3E))
        {
            ChScan_AbortScan(&stChScanH3E);
            bRtnVal = true;
        }
        break;
    case EVENT_EDIT_CANCEL:
        UpdateData_Comm(false);
        bRtnVal = true;
        break;
    case EVENT_EDIT_DONE:
        UpdateData_Comm(true);		// UI -> 변수
        // Things to do here: 값 적용 루틴 들어갈 곳
        if(nScreen == SCREEN_J3E)
		{
			if(IsValidFrequency(&cmComm, uFreqSSB))
				SetSSBFreq(uFreqSSB);
			if(uParam == editFreq.nID)
				UpdateChannel(&cmComm, uChSSB, uFreqSSB, 0);
		}
        else if(nScreen == SCREEN_H3E)
		{
			if(IsValidFrequency(&cmComm, uFreqSSB))
				SetH3EFreq(uFreqH3E);
			if(uParam == editFreq.nID)
				UpdateChannel(&cmComm, uChH3E, uFreqH3E, 14);
		}
        bRtnVal = true;
        break;
    case EVENT_EDIT_CHANGED:
        if(nScreen == SCREEN_J3E)
		{
			if(uParam == editCh.nID)
			{
				editFreq.nValue = GetChannel(&cmComm, editCh.nValue, 0);
			}
		}
		else if(nScreen == SCREEN_H3E)
		{
			if(uParam == editCh.nID)
			{
				editFreq.nValue = GetChannel(&cmComm, editCh.nValue, -14);
			}
		}
        break;
    case EVENT_VKEY_LPUSH:
        if(uParam == VKEY_CLARITY)
        {
            iBFOUI = 0;
            SetBFOByLevel(iBFOUI);
            bRtnVal = true;
        }
        else if(uParam == VKEY_TONE)
        {
			if(nScreen == SCREEN_J3E)
			{        
				if(ChScan_IsOnScaning(&stChScanJ3E))
				{
					ChScan_AbortScan(&stChScanJ3E);
				}
				else
				{
					ChScan_StartScan(&stChScanJ3E, editCh.nValue, 1, 300, 100);
					RegisterTCB(1, (void (*)(int))PushEvent, EVENT_CH_SCAN << 16);
				}
				bRtnVal = true;
			}
			else if(nScreen == SCREEN_H3E)
			{        
				if(ChScan_IsOnScaning(&stChScanH3E))
				{
					ChScan_AbortScan(&stChScanH3E);
				}
				else
				{
					ChScan_StartScan(&stChScanH3E, editCh.nValue, 1, 300, 100);
					RegisterTCB(1, (void (*)(int))PushEvent, EVENT_CH_SCAN << 16);
				}
				bRtnVal = true;
			}
        }
        break;
    case EVENT_VKEY_SPOP:
        switch(uParam)
        {
        case VKEY_TONE:
			if(ChScan_IsOnScaning(&stChScanJ3E))
			{
				ChScan_AbortScan(&stChScanJ3E);
				bRtnVal = true;
				break;
			}
			if(ChScan_IsOnScaning(&stChScanH3E))
			{
				ChScan_AbortScan(&stChScanH3E);
				bRtnVal = true;
				break;
			}
            if(!IsToneEnable())
                SendToneSignal(true);
            else
                SendToneSignal(false);
            bRtnVal = true;
            break;
        case VKEY_CLARITY:
            if(nInputItem == INPUT_ITEM_BFO)
                nInputItem = INPUT_ITEM_CH;
            else
                nInputItem = INPUT_ITEM_BFO;
            bRtnVal = true;
            break;
        case VKEY_ENTER:
            if(nInputItem == INPUT_ITEM_CH)
                nInputItem = INPUT_ITEM_FREQ;
            else
                nInputItem = INPUT_ITEM_CH;
            bRtnVal = true;
            break;
        case VKEY_CANCEL:
        case VKEY_NUM0:
        case VKEY_NUM1:
        case VKEY_NUM2:
        case VKEY_NUM3:
        case VKEY_NUM4:
        case VKEY_NUM5:
        case VKEY_NUM6:
        case VKEY_NUM7:
        case VKEY_NUM8:
        case VKEY_NUM9:
            if(nInputItem == INPUT_ITEM_CH)
            {
                if(Edit_GetMode(&editCh) == EDIT_MODE_DONE)
                {
                    Edit_Start(&editCh, 0);
                    bRtnVal = Edit_KeyInput(&editCh, uParam);
                }
            }
            else if(nInputItem == INPUT_ITEM_FREQ)
            {
                if(Edit_GetMode(&editFreq) == EDIT_MODE_DONE)
                {
                    Edit_Start(&editFreq, 0);
                    bRtnVal = Edit_KeyInput(&editFreq, uParam);
                }
            }
            bRtnVal = true;
            break;
        }
        break;
    case EVENT_ROTARY_SW:
        if(nInputItem == INPUT_ITEM_BFO)
        {
            s32 iTmp;
            iTmp = iBFOUI + ((signed char)uParam * 10);
            if(iTmp > MAX_BFO)
                iBFOUI = MAX_BFO;
            else if(iTmp < MIN_BFO)
                iBFOUI = MIN_BFO;
            else
                iBFOUI = iTmp;
            SetBFOByLevel(iBFOUI);
            //PushEventA(EVENT_REDRAW, 0);
            bRtnVal = true;
        }

        break;
    }
    return bRtnVal;
}

void DrawProc_Comm()
{
    bool bDay = false;
    DrawDesignSensitivity(bDay);
    DrawDesignStatusBar(bDay);
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 메인
    int nStaticX, nStaticY;
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
	

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 채널 값 그리기
    RECT rcCh, rcChStatic;
    rcCh.x = 345;
    rcCh.y = 50;
    rcCh.w = 125;
    rcCh.h = 74;
    rcChStatic.x = rcCh.x - nStaticX;
    rcChStatic.y = rcCh.y + 5;
    rcChStatic.w = nStaticX;
    rcChStatic.h = nStaticY;

    //egl_font_set_color(Resource_GetFont(IDF_64), rgbFG);
    //DrawText(Resource_GetFont(IDF_64), rcCh, sCh, ALIGN_H_RIGHT | ALIGN_V_LOWER);
    Edit_DrawProc(&editCh, Resource_GetFont(IDF_64), rcCh, rgbFG, rgbBG);

    if(GetLastKey() == VKEY_DISTRESS)				// 시험용
        draw_roundrect(rcCh.x, rcCh.y, rcCh.w, rcCh.h, 5, rgbFG);

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

    Edit_DrawProc(&editFreq, Resource_GetFont(IDF_80), rcFreq, rgbFreq, rgbBG);

    if(GetLastKey() == VKEY_DISTRESS)				// 시험용
        draw_roundrect(rcFreq.x, rcFreq.y, rcFreq.w, rcFreq.h, 5, rgbFreq);

    egl_font_set_color(Resource_GetFont(IDF_24), rgbFreq);
    DrawText(Resource_GetFont(IDF_24), rcFreqStatic, "kHz", ALIGN_H_CENTER | ALIGN_V_CENTER);
    draw_roundrect(rcFreqStatic.x, rcFreqStatic.y, rcFreqStatic.w, rcFreqStatic.h, 5, rgbFreq);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 송수신 상태
	char sTxStatus[5];
	RECT rcTxStatus;
	u32 rgbTRx;
    if(GetTRxMode())
    {
        strcpy(sTxStatus, "송신");
        rcTxStatus.x = rcFreqStatic.x + nStaticX + 10;
        rcTxStatus.y = rcChStatic.y;
        rcTxStatus.w = nStaticX;
        rcTxStatus.h = nStaticY;
		rgbTRx = MAKE_COLORREF(127,0,0);
    }
    else
    {
        strcpy(sTxStatus, "수신");
        rcTxStatus.x = rcFreqStatic.x + nStaticX + 10;
        rcTxStatus.y = rcFreqStatic.y;
        rcTxStatus.w = nStaticX;
        rcTxStatus.h = nStaticY;
		rgbTRx = MAKE_COLORREF(0,127,0);
    }
    egl_font_set_color(Resource_GetFont(IDF_24), rgbTRx);
    DrawText(Resource_GetFont(IDF_24), rcTxStatus, sTxStatus, ALIGN_H_CENTER | ALIGN_V_CENTER);
    draw_roundrect(rcTxStatus.x, rcTxStatus.y, rcTxStatus.w, rcTxStatus.h, 5, rgbTRx);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // BFO 값 그리기
    RECT rcBFO;
    SURFACE* surf;
    surf = Resource_GetSurface(IDR_CTRL_BFO_BG);
    rcBFO.x = rcClient.x + (rcClient.w - surf->w) / 2;
    rcBFO.y = 235;//rcClient.y + rcClient.h - surf->h;
    rcBFO.w = surf->w;
    rcBFO.h = surf->h;
    drawsurface(surf, rcBFO.x, rcBFO.y);
    if(GetLastKey() == VKEY_DISTRESS)				// 시험용
        draw_roundrect(rcBFO.x - 5, rcBFO.y - 5, rcBFO.w + 10, rcBFO.h + 10, 5, rgbFG);

    int nWidth = surf->w / 2;

    surf = Resource_GetSurface(IDR_CTRL_BFO_FG);
    rcBFO.x = rcBFO.x + (rcBFO.w - surf->w) / 2;
    rcBFO.y = rcBFO.y + (rcBFO.h - surf->h) / 2;
    rcBFO.w = surf->w;
    rcBFO.h = surf->h;
    rcBFO.x += nWidth * iBFOUI / MAX_BFO;
    drawsurface(surf, rcBFO.x, rcBFO.y);

    RECT rcBFOStatic;
    rcBFOStatic.x = 150;
    rcBFOStatic.y = rcBFO.y - nStaticY - 5;
    rcBFOStatic.w = nStaticX;
    rcBFOStatic.h = nStaticY;
	egl_font_set_color(Resource_GetFont(IDF_24), rgbFG);
    DrawText(Resource_GetFont(IDF_24), rcBFOStatic, "BFO", ALIGN_H_CENTER | ALIGN_V_CENTER);
    draw_roundrect(rcBFOStatic.x, rcBFOStatic.y, rcBFOStatic.w, rcBFOStatic.h, 5, rgbFG);

    char sBFO[10];
    sprintf(sBFO, "%d", iBFOUI);
    RECT rcBFOValue;
    rcBFOValue.x = rcClient.x + (rcClient.w / 2) - nStaticX / 2;
    rcBFOValue.y = rcBFOStatic.y;
    rcBFOValue.w = rcBFOStatic.w;
    rcBFOValue.h = rcBFOStatic.h;
    egl_font_set_color(Resource_GetFont(IDF_24), rgbFG);
    DrawText(Resource_GetFont(IDF_24), rcBFOValue, sBFO, ALIGN_H_CENTER | ALIGN_V_CENTER);
	
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 편집 중인 항목 표시
    char sEditing[10] = "";
    RECT rcEditing;
    switch(nInputItem)
    {
    case INPUT_ITEM_CH:
        strcpy(sEditing, "CH");
        rcEditing = rcChStatic;
        draw_roundrectfill(rcEditing.x, rcEditing.y, rcEditing.w, rcEditing.h, 5, rgbFG);
        break;
    case INPUT_ITEM_FREQ:
        strcpy(sEditing, "kHz");
        rcEditing = rcFreqStatic;
        draw_roundrectfill(rcEditing.x, rcEditing.y, rcEditing.w, rcEditing.h, 5, rgbFreq);
        break;
    case INPUT_ITEM_BFO:
        strcpy(sEditing, "BFO");
        rcEditing = rcBFOStatic;
        draw_roundrectfill(rcEditing.x, rcEditing.y, rcEditing.w, rcEditing.h, 5, rgbFG);
        break;
    default:
        break;
    }
    egl_font_set_color(Resource_GetFont(IDF_24), rgbBG);
    DrawText(Resource_GetFont(IDF_24), rcEditing, sEditing, ALIGN_H_CENTER | ALIGN_V_CENTER);

    if((ChScan_IsOnScaning(&stChScanJ3E) || ChScan_IsOnScaning(&stChScanH3E)) && CheckToggleValue(TOGGLE_FLAG_500))
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
