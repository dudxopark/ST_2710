
#pragma once

#include "Common.h"

void InitGUIManager();
void DrawProc();
bool EventProc();			// 이벤트 처리

void ProcBootUp();
void DrawStatusBar();

void EventProc_VkeyPopS(u8 uVkey);
void EventProc_VkeyPopL(u8 uVkey);
void EventProc_VkeyPushS(u8 uVkey);
void EventProc_VkeyPushL(u8 uVkey);
bool PreFetchEvent(u16 uEventID, u16 uEventParam);
