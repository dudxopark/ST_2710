
#include "adstar.h"
#include "ST_2710.h"
#include "ST_2710_GUI.h"
#include "ST_2710_GUI_Amp.h"
#include "ST_2710_Global.h"
#include "ResourceManager.h"
#include "Event.h"
#include "P10_3940.h"
#include "LCDCtrl.h"

void InitGUI_Amp()
{
}

void UpdateData_Amp(bool bApplyToVariable)
{
}	

bool EventProc_Amp(u16 uEvent, u16 uParam)
{
    bool bRtnVal = false;
/*    int nScreen = GetCurrentScreen();
    if((nScreen != SCREEN_AMP))
        return false;
    //if(EventProc_AmpChild(uEvent, uParam))
    //    return true;
    switch(uEvent)
    {
	case EVENT_UPDATEDATA:
		if(uParam)
			UpdateData_Amp(true);
		else
			UpdateData_Amp(false);
		bRtnVal = true;		
		break;
    }*/
    return bRtnVal;
}

void DrawProc_Amp()
{
    bool bDay = false;
    //DrawDesignSensitivity(bDay);
    //DrawDesignStatusBar(bDay);
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // ∏ﬁ¿Œ	
    RECT rcClient, rcGraph;
    u32 rgbBG = MAKE_COLORREF(255,255,255);
    U32	rgbFG = MAKE_COLORREF(204,204,204);
    U32	rgbText = MAKE_COLORREF(229,115,44);
	U32	rgbTextBG = MAKE_COLORREF(35,35,35);
    if(!bDay)
    {
        rgbBG = MAKE_COLORREF(0,0,0);
        rgbFG = MAKE_COLORREF(35,35,35);		
        rgbText = MAKE_COLORREF(229,115,44);
		rgbTextBG = MAKE_COLORREF(204,204,204);
    }
	rcClient.x = 0;
    rcClient.y = 0;
    rcClient.w = LCD_RESOLUTION_X;
    rcClient.h = LCD_RESOLUTION_Y;
	/*rcClient.x = 128;
    rcClient.y = 48;
    rcClient.w = 352;
    rcClient.h = 224;*/
    draw_rectfill(rcClient.x, rcClient.y, rcClient.w, rcClient.h, rgbBG);
	
    rcGraph.x =  rcClient.x + 30;
    rcGraph.y =  rcClient.y + 30;
    rcGraph.w =  rcClient.w - 60;
    rcGraph.h =  rcClient.h - 60;	

	//draw_roundrectfill(rcGraph.x, rcGraph.y, rcGraph.w, rcGraph.h, 8, rgbFG);
	egl_font_set_color(Resource_GetFont(IDF_80), rgbTextBG);
	rcGraph.x += 1;			DrawText(Resource_GetFont(IDF_80), rcGraph, "AMP", ALIGN_H_CENTER | ALIGN_V_CENTER);		rcGraph.x -= 1;
	rcGraph.y += 1;			DrawText(Resource_GetFont(IDF_80), rcGraph, "AMP", ALIGN_H_CENTER | ALIGN_V_CENTER);		rcGraph.y -= 1;
	rcGraph.x -= 1;			DrawText(Resource_GetFont(IDF_80), rcGraph, "AMP", ALIGN_H_CENTER | ALIGN_V_CENTER);		rcGraph.x += 1;
	rcGraph.y -= 1;			DrawText(Resource_GetFont(IDF_80), rcGraph, "AMP", ALIGN_H_CENTER | ALIGN_V_CENTER);		rcGraph.y += 1;	
	egl_font_set_color(Resource_GetFont(IDF_80), rgbText);
	DrawText(Resource_GetFont(IDF_80), rcGraph, "AMP", ALIGN_H_CENTER | ALIGN_V_CENTER);
}
