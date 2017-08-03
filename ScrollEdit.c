
#include "ScrollEdit.h"
#include "ResourceManager.h"
#include "ST_2710_GUI.h"
#include "Event.h"

#define EDIT_TIMEOUT			10000

void Edit_Start(ScrollEdit* edit, int nValue)
{
	debugprintf("Enter Editing Mode.\r\n");

	(edit)->nMode = EDIT_MODE_DOING;
	(edit)->nCursorPos = -1;
	(edit)->nValue = nValue;
	(edit)->uTimeStamp = GetSystemClock();
}

void Edit_CheckTimeOut(ScrollEdit* edit)
{
	u64 uTimeNow = GetSystemClock();
	if((uTimeNow - (edit)->uTimeStamp) > EDIT_TIMEOUT)
	{
		(edit)->nMode = EDIT_MODE_DONE;
		PushEventA(EVENT_EDIT_CANCEL, (edit)->nID);
	}	
}

bool Edit_IsDataValid(ScrollEdit* edit)
{
	if((edit)->nValue < (edit)->nMinValue)
		return false;
	else if((edit)->nValue > (edit)->nMaxValue)
		return false;
	return true;
}

bool Edit_KeyInput(ScrollEdit* edit, u16 uKey)
{
	if(/*(uKey >= VKEY_NUM0) && */(uKey <= VKEY_NUM9))
	{
		(edit)->uTimeStamp = GetSystemClock();
		if((edit)->nValue * 10 + (uKey - VKEY_NUM0) > (edit)->nMaxValue)
			return false;
		(edit)->nCursorPos++;
		(edit)->nValue *= 10;
		(edit)->nValue += (uKey - VKEY_NUM0);
		PushEventA(EVENT_EDIT_CHANGED, (edit)->nID);
		return true;
	}
	else if(uKey == VKEY_ENTER)
	{
		(edit)->nMode = EDIT_MODE_DONE;
		if(Edit_IsDataValid(edit))
			PushEventA(EVENT_EDIT_DONE, (edit)->nID);
		else
			PushEventA(EVENT_EDIT_CANCEL, (edit)->nID);			
		return true;
	}
	else if(uKey == VKEY_CANCEL)
	{
		(edit)->nMode = EDIT_MODE_DONE;
		PushEventA(EVENT_EDIT_CANCEL, (edit)->nID);
		return true;
	}
	return false;
}

bool Edit_ScrollInput(ScrollEdit* edit, int nDelta)
{
	(edit)->uTimeStamp = GetSystemClock();
	if((edit)->nValue + nDelta > (edit)->nMaxValue)
		(edit)->nValue = (edit)->nMaxValue;
		//return false;
	else if((edit)->nValue + nDelta < (edit)->nMinValue)
		(edit)->nValue = (edit)->nMinValue;
		//return false;
	else
		(edit)->nValue += nDelta;
	PushEventA(EVENT_EDIT_CHANGED, (edit)->nID);
	return true;
}

bool Edit_GetValue(ScrollEdit* edit)
{
	return (edit)->nValue;
}
	
u08 Edit_GetMode(ScrollEdit* edit)
{
	return (edit)->nMode;
}

bool Edit_EventProc(ScrollEdit* edit, u16 uEvent, u16 uParam)
{	
	bool bRtnVal = false;	
	
	switch(uEvent)
	{
	case EVENT_VKEY_SPOP:
		switch(uParam)
		{
		case VKEY_ENTER:
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
			if(Edit_GetMode(edit) != EDIT_MODE_DOING)
				return false;
			bRtnVal = Edit_KeyInput(edit, uParam);
			break;
		}
		break;			
	case EVENT_ROTARY_SW:
		bRtnVal = Edit_ScrollInput(edit, (signed char)uParam);
		if(Edit_GetMode(edit) == EDIT_MODE_DONE)
			PushEventA(EVENT_EDIT_DONE, (edit)->nID);
		break;
	case EVENT_TIMERTICK_100MS:
		if(Edit_GetMode(edit) == EDIT_MODE_DOING)
			Edit_CheckTimeOut(edit);
		bRtnVal = false;
		break;		
	};
	return bRtnVal;		
}

void Edit_MakeRenderingValue(ScrollEdit* edit, char* sValue)
{
	if((edit)->bFillZero)
	{
		IntToDoubleStrWithCommaAndFillZero((edit)->nValue, (edit)->nMaxValue, (edit)->nDotPos, sValue);
/*		int nValue = (edit)->nValue;
		int nMaxValue = (edit)->nMaxValue;
		int nDotPos = (edit)->nDotPos;
		char* pValue = sValue;
		
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
			//TRACE("%s\r\n", sRemainder);
		}			
		////////////////////////////////////////////////////////////////////////////////////////////////
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
			//sprintf(sOne, "%d", nOne);
		}
		else
		{
			char sFommatter[15];
			sprintf(sFommatter, "%%0%dd,%%03d", nMaxDotPos - 3);			// To make %01d/%02d/%03d/....
			sprintf(sOne, sFommatter, nThousand, nOne);
			//sprintf(sOne, "%d,%03d", nThousand, nOne);
		}
		////////////////////////////////////////////////////////////////////////////////////////////////
		sprintf(pValue, "%s%s", sOne, sRemainder);*/
	}
	else
	{
		IntToDoubleStrWithComma((edit)->nValue, (edit)->nDotPos, sValue);
	}
}

void Edit_DrawProc(ScrollEdit* edit, EGL_FONT* pFnt, RECT rcClient, u32 rgbFG, u32 rgbBG)
{
#if	1				// 커서 ' ' 사용
	char sEdit[32];
	//Edit_MakeRenderingValue(edit, sEdit);
	IntToDoubleStrWithComma((edit)->nMaxValue, (edit)->nDotPos, sEdit);
	
	int hgap = 5;
	int wgap = 3;
	int right, bottom;
	RECT rcEdit;
	right = rcClient.x + rcClient.w;
	bottom = rcClient.y + rcClient.h;	
	rcEdit.w = text_width(pFnt, sEdit);
	rcEdit.h = pFnt->h;
	rcEdit.x = right - rcEdit.w - wgap;
	rcEdit.y = bottom - rcEdit.h - hgap;

	Edit_MakeRenderingValue(edit, sEdit);
	//IntToDoubleStrWithComma((edit)->nValue, (edit)->nDotPos, sEdit);

	if(Edit_GetMode(edit) == EDIT_MODE_DONE)
	{
		//DrawRectFill(rcEdit, rgbBG);
		egl_font_set_color(pFnt, rgbFG);
		DrawText(pFnt, rcClient, sEdit, ALIGN_H_RIGHT | ALIGN_V_LOWER);
	}
	else if(Edit_GetMode(edit) == EDIT_MODE_DOING)
	{
		DrawRectFill(rcEdit, rgbFG);
		egl_font_set_color(pFnt, rgbBG);
		DrawText(pFnt, rcClient, sEdit, ALIGN_H_RIGHT | ALIGN_V_LOWER);
		
		if(CheckToggleValue(TOGGLE_FLAG_500))
		{
			sprintf(sEdit, "%01d", (edit)->nValue % 10);
			
			rcEdit.w = text_width(pFnt, sEdit);
			rcEdit.h = pFnt->h;
			rcEdit.x = right - rcEdit.w - wgap - 1;
			rcEdit.y = bottom - rcEdit.h - hgap;
			DrawRectFill(rcEdit, rgbBG);
			egl_font_set_color(pFnt, rgbFG);
			DrawText(pFnt, rcClient, sEdit, ALIGN_H_RIGHT | ALIGN_V_LOWER);
		}
	}		
#else				// 커서 '|' 사용
	char sEdit[32], sTmp[2] = "";
	IntToDoubleStrWithComma((edit)->nMaxValue, (edit)->nDotPos, sEdit);
	int hgap = 5;
	int wgap = 15;
	int right, bottom;
	RECT rcEdit;
	right = rcClient.x + rcClient.w;
	bottom = rcClient.y + rcClient.h;	
	rcEdit.w = text_width(pFnt, sEdit) + wgap;
	rcEdit.h = pFnt->h;
	rcEdit.x = right - rcEdit.w + wgap / 2;
	rcEdit.y = bottom - rcEdit.h - hgap;
	
	DrawRectFill(rcEdit, rgbBG);
	//draw_rectfill(rcClient.x, rcClient.y, rcClient.w, rcClient.h, rgbBG);
		
	egl_font_set_color(pFnt, rgbFG);
	IntToDoubleStrWithComma((edit)->nValue, (edit)->nDotPos, sEdit);
	DrawText(pFnt, rcClient, sEdit, ALIGN_H_RIGHT | ALIGN_V_LOWER);
		
	if(CheckToggleValue(TOGGLE_FLAG_500))
	{
		rcClient.x += wgap;
		sTmp[0] = '|';
		egl_font_set_color(pFnt, rgbFG);
		DrawText(pFnt, rcClient, sTmp, ALIGN_H_RIGHT | ALIGN_V_LOWER);
	}
#endif
}
