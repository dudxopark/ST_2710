
#pragma once
#include "Common.h"
#include "adstar.h"

// Mode
#define EDIT_MODE_DONE			0
#define EDIT_MODE_DOING			1

// Event
#define EVENT_EDIT_CANCEL		(EVENT_APP_BASE + 1)
#define EVENT_EDIT_DONE			(EVENT_APP_BASE + 2)
#define EVENT_EDIT_CHANGED		(EVENT_APP_BASE + 3)

typedef struct tagScrollEdit
{
	int nID;					// Edit ID
	bool bFillZero;				// 앞자리를 '0'으로 채울 지 설정
	int nDotPos;				// 소수점 자리. 뒷쪽부터 몇번째 자리에 .을 표시할지 결정
	int nMode;					// 동작모드. 입력 중 or 완료
	int nCursorPos;				// 현재 입력 위치
	int nPreviousValue;			// 이전 값
	int nValue;					// 현재 값
	int nMinValue;				// 최소값
	int nMaxValue;				// 최대값
	u64 uTimeStamp;				// 마지막 입력 시간
}ScrollEdit;

bool Edit_IsDataValid(ScrollEdit* edit);
void Edit_Start(ScrollEdit* edit, int nValue);
bool Edit_KeyInput(ScrollEdit* edit, u16 uKey);
bool Edit_ScrollInput(ScrollEdit* edit, int nDelta);
bool Edit_GetValue(ScrollEdit* edit);
u08 Edit_GetMode(ScrollEdit* edit);
bool Edit_EventProc(ScrollEdit* edit, u16 uEvent, u16 uParam);
void Edit_DrawProc(ScrollEdit* edit, EGL_FONT* pFnt, RECT rcClient, u32 rgbFG, u32 rgbBG);
