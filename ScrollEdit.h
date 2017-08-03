
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
	bool bFillZero;				// ���ڸ��� '0'���� ä�� �� ����
	int nDotPos;				// �Ҽ��� �ڸ�. ���ʺ��� ���° �ڸ��� .�� ǥ������ ����
	int nMode;					// ���۸��. �Է� �� or �Ϸ�
	int nCursorPos;				// ���� �Է� ��ġ
	int nPreviousValue;			// ���� ��
	int nValue;					// ���� ��
	int nMinValue;				// �ּҰ�
	int nMaxValue;				// �ִ밪
	u64 uTimeStamp;				// ������ �Է� �ð�
}ScrollEdit;

bool Edit_IsDataValid(ScrollEdit* edit);
void Edit_Start(ScrollEdit* edit, int nValue);
bool Edit_KeyInput(ScrollEdit* edit, u16 uKey);
bool Edit_ScrollInput(ScrollEdit* edit, int nDelta);
bool Edit_GetValue(ScrollEdit* edit);
u08 Edit_GetMode(ScrollEdit* edit);
bool Edit_EventProc(ScrollEdit* edit, u16 uEvent, u16 uParam);
void Edit_DrawProc(ScrollEdit* edit, EGL_FONT* pFnt, RECT rcClient, u32 rgbFG, u32 rgbBG);
