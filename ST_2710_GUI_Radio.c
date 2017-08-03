
#include "adstar.h"
#include "ST_2710.h"
#include "ST_2710_GUI.h"
#include "ST_2710_GUI_Radio.h"
#include "ST_2710_Global.h"
#include "ResourceManager.h"
#include "P10_3940.h"
#include "Event.h"
#include "ScrollEdit.h"
#include "Key.h"
#include "util.h"
#include "FrequencyScanner.h"

#define MIN_FREQ_AM			522						// 522 kHz
#define MAX_FREQ_AM			1710					// 1,710 kHz

#define MIN_FREQ_FM			875						// 87.5 MHz
#define MAX_FREQ_FM			1080					// 108.0 MHz

#define MIN_CH_AM			1
#define MAX_CH_AM			30

#define MIN_CH_FM			1
#define MAX_CH_FM			30

static u16 uChAM = 1;
static u16 uChFM = 1;

static u32 uFreqAM = 522;		// 주파수 표시용 변수: 522 kHz
static u32 uFreqFM = 875;		// 주파수 표시용 변수: 87.5 MHz

#define INPUT_ITEM_CH			0
#define INPUT_ITEM_FREQ			1

static int nInputItem = INPUT_ITEM_CH;
static ScrollEdit editCh = {IDR_EDIT_CH, true};
static ScrollEdit editFreq = {IDR_EDIT_FREQ, false};

ChannelManager cmAM;
ChannelManager cmFM;

void UpdateData_Radio(bool bApplyToVariable)
{
    int nScreen = GetCurrentScreen();
    if(bApplyToVariable)  			// 화면 값을 변수에 저장
    {
        if(nScreen == SCREEN_AM)
        {
            uChAM = editCh.nValue;
            uFreqAM = editFreq.nValue;
        }
        else if(nScreen == SCREEN_FM)
        {
            uChFM = editCh.nValue;
            uFreqFM = editFreq.nValue;
        }
    }
    else  							// 변수 값을 화면으로...
    {
        if(nScreen == SCREEN_AM)
        {
            editCh.nValue = uChAM;
            editFreq.nValue = uFreqAM;
        }
        else if(nScreen == SCREEN_FM)
        {
            editCh.nValue = uChFM;
            editFreq.nValue = uFreqFM;
        }
    }
}

void InitGUI_Radio()
{
    editCh.nID = IDR_EDIT_CH;
	editCh.bFillZero = false;
	editCh.nDotPos = 0;
    editCh.nMode = EDIT_MODE_DONE;
    editCh.nMinValue = MIN_CH_AM;
    editCh.nMaxValue = MAX_CH_AM;

    editFreq.nID = IDR_EDIT_FREQ;
	editFreq.nDotPos = 1;
    editFreq.nMode = EDIT_MODE_DONE;
    editFreq.nMinValue = MIN_FREQ_AM;
    editFreq.nMaxValue = MAX_FREQ_AM;

    CreateChannelManager(&cmAM, MAX_CH_AM, MIN_FREQ_AM, MAX_FREQ_AM);
    UpdateChannel(&cmAM, 1, 603, 0);
    UpdateChannel(&cmAM, 2, 711, 0);
    UpdateChannel(&cmAM, 3, 792, 0);
    UpdateChannel(&cmAM, 4, 837, 0);
    UpdateChannel(&cmAM, 5, 900, 0);
    UpdateChannel(&cmAM, 6, 972, 0);
    UpdateChannel(&cmAM, 7, 1132, 0);
    UpdateChannel(&cmAM, 8, 1170, 0);
	UpdateChannel(&cmAM, 9, 1188, 0);

    CreateChannelManager(&cmFM, MAX_CH_FM, MIN_FREQ_FM, MAX_FREQ_FM);
    UpdateChannel(&cmFM, 1, 999, 0);
    UpdateChannel(&cmFM, 2, 875, 0);
    UpdateChannel(&cmFM, 3, 877, 0);
    UpdateChannel(&cmFM, 4, 937, 0);
    UpdateChannel(&cmFM, 5, 900, 0);
    UpdateChannel(&cmFM, 6, 972, 0);
    UpdateChannel(&cmFM, 7, 1032, 0);
    UpdateChannel(&cmFM, 8, 1070, 0);
	UpdateChannel(&cmFM, 9, 1080, 0);
	
    UpdateData_Radio(false);
}

bool EventProc_RadioChild(u16 uEvent, u16 uParam)
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

T_FREQ_SCAN stFreqScanAM = {SetAMFreq, NULL};	
T_FREQ_SCAN stFreqScanFM = {SetFMFreq, NULL};
	
bool EventProc_Radio(u16 uEvent, u16 uParam)
{
    bool bRtnVal = false;
    int nScreen = GetCurrentScreen();
    if((nScreen != SCREEN_AM) && (nScreen != SCREEN_FM))
        return false;
    if(EventProc_RadioChild(uEvent, uParam))
        return true;
    switch(uEvent)
    {
	case EVENT_UPDATE_SCREEN:
		if(uParam == SCREEN_AM)
		{
			editCh.nDotPos = 0;			
			editCh.nMinValue = MIN_CH_AM;
			editCh.nMaxValue = MAX_CH_AM;

			editFreq.nDotPos = 0;
			editFreq.nMinValue = MIN_FREQ_AM;
			editFreq.nMaxValue = MAX_FREQ_AM;
			bRtnVal = true;		
        }
        else if(uParam == SCREEN_FM)
        {
			editCh.nDotPos = 0;			
			editCh.nMinValue = MIN_CH_FM;
			editCh.nMaxValue = MAX_CH_FM;

			editFreq.nDotPos = 1;
			editFreq.nMinValue = MIN_FREQ_FM;
			editFreq.nMaxValue = MAX_FREQ_FM;
			bRtnVal = true;		
        }
		break;			
	case EVENT_UPDATEDATA:
		if(uParam)
			UpdateData_Radio(true);
		else
			UpdateData_Radio(false);
		bRtnVal = true;		
		break;
    case EVENT_FREQ_SCAN:
		if(nScreen == SCREEN_AM)
		{
			int nResult;
			if((nResult = FreqScan_DoScan(&stFreqScanAM)) != 2)
			{
				editFreq.nValue = FreqScan_GetCurrentScan(&stFreqScanAM);
				PushEventA(EVENT_EDIT_CHANGED, editFreq.nID);
				if(nResult == 1)		// 끝남
				{
					int i, nFreq;
					FreqScan_SortScanResult(&stFreqScanAM);
					for(i = 0 ; i < MAX_CH_AM ; i++)
					{
						nFreq = stFreqScanFM.arStrength[i].nFreq;
						debugprintf("Ch: %d, Freq: %d, Strength: %d\r\n", i + 1, nFreq, stFreqScanFM.arStrength[i].nStrength);
						if(IsValidFrequency(&cmAM, nFreq))
						{
							UpdateChannel(&cmAM, i + 1, nFreq, 0);
						}						
					}
					FreqScan_ClearScanResult(&stFreqScanAM);					
				}
				else if(nResult != 1)
					RegisterTCB(stFreqScanAM.nScanGap_ms, (void (*)(int))PushEvent, EVENT_FREQ_SCAN << 16);
			   bRtnVal = true;
			}
		}
		else if(nScreen == SCREEN_FM)
		{
			int nResult;
			if((nResult = FreqScan_DoScan(&stFreqScanFM)) != 2)
			{
				editFreq.nValue = FreqScan_GetCurrentScan(&stFreqScanFM);
				PushEventA(EVENT_EDIT_CHANGED, editFreq.nID);
				if(nResult == 1)		// 끝남
				{
					int i, nFreq;
					FreqScan_SortScanResult(&stFreqScanFM);
					for(i = 0 ; i < MAX_CH_FM ; i++)
					{
						nFreq = stFreqScanFM.arStrength[i].nFreq;
						debugprintf("Ch: %d, Freq: %d, Strength: %d\r\n", i + 1, nFreq, stFreqScanFM.arStrength[i].nStrength);
						if(IsValidFrequency(&cmFM, nFreq))
						{
							UpdateChannel(&cmFM, i + 1, nFreq, 0);
						}						
					}
					FreqScan_ClearScanResult(&stFreqScanFM);
				}
				else if(nResult != 1)
					RegisterTCB(stFreqScanFM.nScanGap_ms, (void (*)(int))PushEvent, EVENT_FREQ_SCAN << 16);
			   bRtnVal = true;
			}
		}
		break;			
    case EVENT_EDIT_CANCEL:
        UpdateData_Radio(false);
        bRtnVal = true;
        break;
    case EVENT_EDIT_DONE:
        UpdateData_Radio(true);		// UI -> 변수
        // Things to do here: 값 적용 루틴 들어갈 곳
        if(nScreen == SCREEN_AM)
		{
			if(IsValidFrequency(&cmAM, uFreqAM))
				SetAMFreq(uFreqAM);
			if(uParam == editFreq.nID)
				UpdateChannel(&cmAM, uChAM, uFreqAM, 0);
		}
        else if(nScreen == SCREEN_FM)
		{
			if(IsValidFrequency(&cmFM, uFreqFM))
				SetFMFreq(uFreqFM);
			if(uParam == editFreq.nID)
				UpdateChannel(&cmFM, uChFM, uFreqFM, 0);
		}
        bRtnVal = true;
        break;
    case EVENT_EDIT_CHANGED:
        if(nScreen == SCREEN_AM)
		{
			if(uParam == editCh.nID)
			{
				editFreq.nValue = GetChannel(&cmAM, editCh.nValue, 0);
			}
		}
		else if(nScreen == SCREEN_FM)
		{
			if(uParam == editCh.nID)
			{
				editFreq.nValue = GetChannel(&cmFM, editCh.nValue, 0);
			}
		}
        break;
    case EVENT_VKEY_LPUSH:
		if(uParam == VKEY_TONE)
        {
			if(nScreen == SCREEN_AM)
			{
				if(FreqScan_IsOnScaning(&stFreqScanAM))
				{
					FreqScan_AbortScan(&stFreqScanAM);
				}
				else
				{
					editCh.nValue = INVALID_CH;
					DeleteAllChannels(&cmAM);
					FreqScan_StartScan(&stFreqScanAM, editFreq.nValue, cmAM.nMinFreq, cmAM.nMaxFreq, 5, MAX_CH_AM);
					RegisterTCB(1, (void (*)(int))PushEvent, EVENT_FREQ_SCAN << 16);
				}
				bRtnVal = true;
			}
			else if(nScreen == SCREEN_FM)
			{
				if(FreqScan_IsOnScaning(&stFreqScanFM))
				{
					FreqScan_AbortScan(&stFreqScanFM);
				}
				else
				{
					editCh.nValue = INVALID_CH;
					DeleteAllChannels(&cmFM);
					FreqScan_StartScan(&stFreqScanFM, editFreq.nValue, cmFM.nMinFreq, cmFM.nMaxFreq, 5, MAX_CH_FM);
					RegisterTCB(1, (void (*)(int))PushEvent, EVENT_FREQ_SCAN << 16);
				}
				bRtnVal = true;
			}
        }
        break;
    case EVENT_VKEY_SPOP:
        switch(uParam)
        {
        case VKEY_TONE:
            if(FreqScan_IsOnScaning(&stFreqScanAM))
            {
                FreqScan_AbortScan(&stFreqScanAM);
                bRtnVal = true;
            }
            if(FreqScan_IsOnScaning(&stFreqScanFM))
            {
                FreqScan_AbortScan(&stFreqScanFM);
                bRtnVal = true;
            }
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
    case EVENT_PTT:
        if(FreqScan_IsOnScaning(&stFreqScanAM))
        {
            FreqScan_AbortScan(&stFreqScanAM);
            bRtnVal = true;
        }
		if(FreqScan_IsOnScaning(&stFreqScanFM))
        {
            FreqScan_AbortScan(&stFreqScanFM);
            bRtnVal = true;
        }
        break;
	case EVENT_ROTARY_SW:
        break;
    }
    return bRtnVal;
}

void DrawProc_Radio()
{
    bool bDay = false;
    DrawDesignSensitivity(bDay);
    DrawDesignStatusBar(bDay);
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 메인
    int nStaticX, nStaticY;
	char sFreqUnit[5] = "kHz";
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

	if(GetCurrentScreen() == SCREEN_FM)
		sprintf(sFreqUnit, "MHz");

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
    DrawText(Resource_GetFont(IDF_24), rcFreqStatic, sFreqUnit, ALIGN_H_CENTER | ALIGN_V_CENTER);
    draw_roundrect(rcFreqStatic.x, rcFreqStatic.y, rcFreqStatic.w, rcFreqStatic.h, 5, rgbFreq);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 송수신 상태
    //char sTxStatus[5];
    // RECT rcText;
    // if(GetTRxMode())
    // {
        // strcpy(sTxStatus, "송신");
        // rcText.x = rcFreqStatic.x + nStaticX + 10;
        // rcText.y = rcChStatic.y;
        // rcText.w = nStaticX;
        // rcText.h = nStaticY;
    // }
    // else
    // {
        // strcpy(sTxStatus, "수신");
        // rcText.x = rcFreqStatic.x + nStaticX + 10;
        // rcText.y = rcFreqStatic.y;
        // rcText.w = nStaticX;
        // rcText.h = nStaticY;
    // }
    // egl_font_set_color(Resource_GetFont(IDF_24), rgbFG);
    // DrawText(Resource_GetFont(IDF_24), rcText, sTxStatus, ALIGN_H_CENTER | ALIGN_V_CENTER);
    // draw_roundrect(rcText.x, rcText.y, rcText.w, rcText.h, 5, rgbFG);

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
        strcpy(sEditing, sFreqUnit);
        rcEditing = rcFreqStatic;
        draw_roundrectfill(rcEditing.x, rcEditing.y, rcEditing.w, rcEditing.h, 5, rgbFreq);
        break;
    default:
        break;
    }
    egl_font_set_color(Resource_GetFont(IDF_24), rgbBG);
    DrawText(Resource_GetFont(IDF_24), rcEditing, sEditing, ALIGN_H_CENTER | ALIGN_V_CENTER);
    if((FreqScan_IsOnScaning(&stFreqScanAM) || FreqScan_IsOnScaning(&stFreqScanFM)) && CheckToggleValue(TOGGLE_FLAG_500))
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


/*
int nSelectedChannel = -1;

void DrawProc_Recv_ChannelList(T_CH_INFO* arChInfo, int nCount, int nSel)
{
#define MAX_DIVIDE_CH_LIST		8
	RECT rcClient = {0, 48, 128, 224};
	RECT rcNum, rcFreq;
	rcNum = rcClient;		rcNum.w = 42;
	rcFreq = rcClient;		rcFreq.x = 42; 		rcFreq.w = 128 - 42;
	U32	rgbBG = MAKE_COLORREF(10,10,20);
	//U32	rgbFG = MAKE_COLORREF(30,30,50);
	U32	rgbNumBG = MAKE_COLORREF(8,8,15);
	//U32	rgbNumFG = MAKE_COLORREF(25,25,40);
	U32 rgbSeperator = MAKE_COLORREF(5,5,10);
	
	draw_rectfill(rcNum.x, rcNum.y, rcNum.w, rcNum.h, rgbNumBG);
	draw_rectfill(rcFreq.x, rcFreq.y, rcFreq.w, rcFreq.h, rgbBG);
	
	draw_vline(rcFreq.x, rcClient.y, rcClient.y + rcClient.h, rgbSeperator);
	
	int i;
	int nPosY = rcClient.y;
	int nHeight = round((double)rcClient.h / MAX_DIVIDE_CH_LIST);
	for(i = 0 ; i < MAX_DIVIDE_CH_LIST + 1; i++)
	{
		draw_hline(rcClient.x, nPosY, rcClient.x + rcClient.w, rgbSeperator);		
		nPosY += nHeight;
	}	
	
	rcNum.h = nHeight;
	rcFreq.h = nHeight;
}

void DrawProc_Recv()
{
	//DrawProc_Recv_ChannelList(g_AM_CH, AM_CH_INFO_SIZE, nSelectedChannel);		// 128
	
	char sFreq[10];
	char sName[20] = "공영방송 KBS 1";
	RECT rcClient = {128, 48, 352, 224};
	U32	rgbBG = MAKE_COLORREF(0,0,0);
	U32	rgbFreq = MAKE_COLORREF(229,115,44);
	draw_rectfill(rcClient.x, rcClient.y, rcClient.w, rcClient.h, rgbBG);

	IntToDoubleStrWithComma(uFreqFM, sFreq);
	
	RECT rcFreq, rcName;
	rcFreq.x = rcClient.x;	rcFreq.y = rcClient.y;	rcFreq.w = rcClient.w;	rcFreq.h = rcClient.h / 2;
	rcName.x = rcClient.x;	rcName.y = rcClient.y + rcClient.h / 10 * 6;	rcName.w = rcClient.w;	rcName.h = rcClient.h / 10 + 4;
	
	egl_font_set_color(Resource_GetFont(IDF_64), rgbFreq);
	DrawText(Resource_GetFont(IDF_64), rcFreq, sFreq, ALIGN_H_CENTER | ALIGN_V_LOWER);
	DrawText(Resource_GetFont(IDF_DEFAULT), rcFreq, "MHz ", ALIGN_H_RIGHT | ALIGN_V_LOWER);	
	DrawText(Resource_GetFont(IDF_DEFAULT), rcName, sName, ALIGN_H_CENTER | ALIGN_V_UPPER);
}
*/
