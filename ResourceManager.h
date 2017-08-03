#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 리소스 관련
#define IDR_MAX						12
#define IDR_TEST					0
#define IDR_BOOTUP					1

#define IDR_ICO_PTT					2
#define IDR_ICO_BK					3
#define IDR_ICO_TONEON				4
#define IDR_ICO_RXMUTEON			5
#define IDR_ICO_RXMUTEOFF			6
#define IDR_ICO_BEEP				7

#define IDR_CTRL_SENSITIVITY_BG		8
#define IDR_CTRL_SENSITIVITY_FG		9
#define IDR_CTRL_BFO_BG				10
#define IDR_CTRL_BFO_FG				11

enum {RESOURCE_TYPE_BMP, RESOURCE_TYPE_JPG};

void Resource_Initialize();
void Resource_Release();
void Resource_SetResource(int nIDR, int nType, const u8* pData, int nSize);
void* Resource_GetSurface(int nIDR);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 컨트롤 ID
#define IDR_EDIT_CH					0x1001
#define IDR_EDIT_FREQ				0x1002
#define IDR_EDIT_CH_INDEX			0x1003

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 폰트 관련
#define ALIGN_H_LEFT 	0x01
#define ALIGN_H_RIGHT 	0x02
#define ALIGN_H_CENTER 	0x04
#define ALIGN_V_UPPER	0x10
#define ALIGN_V_LOWER	0x20
#define ALIGN_V_CENTER	0x40

#define IDF_MAX						4
#define IDF_DEFAULT					0
#define IDF_24						1
#define IDF_64						2
#define IDF_80						3

void Resource_InitializeFont();
void Resource_ReleaseFont();
EGL_FONT* Resource_GetFont(int nIDR);



