
#pragma once

#include "Common.h"

#define SCREEN_MAX			7
#define SCREEN_J3E			0
#define SCREEN_H3E			1
#define SCREEN_AM			2
#define SCREEN_FM			3
#define SCREEN_VHF			4
#define SCREEN_AMP			5
#define SCREEN_SETTING		6

extern u16 g_uLatestFPS, g_uFPS;			// Frame Per Second. �ʴ� ȭ�� ����� ������ ���� ����
extern u16 g_uLatestLPS, g_uLPS;			// Loop Per Second. �ʴ� ���� ȸ���� ����

void InitGUI();			// GUI �ʱ�ȭ

void ProcBootUp();								// ���� ����, ���÷ΰ�, �ʱ����� �׸� ����, ���� Ȯ�� ��

void SetCurrentScreen(int nScreen);
int GetCurrentScreen();
int GetPreviousScreen();
	
bool (*GetEventProc(int nSel))(u16 uEvent, u16 uParam);
void (*GetDrawProc(int nSel))();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �⺻ ȭ��
void DrawDesignStatusBar(bool bDay);
void DrawDesignSensitivity(bool bDay);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Text ����
void DrawText(EGL_FONT* font, RECT rcClient, const char* str, U8 uOption);
void IntToDoubleStrWithComma(int nValue, int nDotPos, char* pValue);
void IntToDoubleStrWithCommaAndFillZero(int nValue, int nMaxValue, int nDotPos, char* pValue);
void DrawRectFill(RECT rcClient, u32 uColor);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ä�� ������
typedef struct
{
    int* arCh;
    int nMaxCh;
    int nMinFreq;
    int nMaxFreq;
} ChannelManager;

void CreateChannelManager(ChannelManager* cm, int nMaxCh, int nMinFreq, int nMaxFreq);
void DestroyChannelManager(ChannelManager* cm, int nMaxCh, int nMinFreq, int nMaxFreq);
bool IsValidFrequency(ChannelManager* cm, int nFreq);
void DeleteChannel(ChannelManager* cm, int nCh);
void DeleteAllChannels(ChannelManager* cm);
bool UpdateChannel(ChannelManager* cm, int nCh, int nFreq, int nOffset);
int GetChannel(ChannelManager* cm, int nCh, int nOffset);
