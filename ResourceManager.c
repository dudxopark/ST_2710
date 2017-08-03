
#include "adstar.h"
#include "Resource.h"
#include "ResourceManager.h"
#include "font.h"

SURFACE* arResource[IDR_MAX] = {NULL};
EGL_FONT* arFont[IDF_MAX] = {NULL};

void Resource_Initialize()
{
	Resource_SetResource(IDR_TEST					, RESOURCE_TYPE_BMP, g_bmpDefault, 			sizeof(g_bmpDefault));
#ifndef UNLOAD_RESOURCE	
	Resource_SetResource(IDR_BOOTUP					, RESOURCE_TYPE_JPG, g_jpgBootUp, 			sizeof(g_jpgBootUp));
	Resource_SetResource(IDR_ICO_PTT				, RESOURCE_TYPE_BMP, g_icoPTT, 				sizeof(g_icoPTT));
	Resource_SetResource(IDR_ICO_BK					, RESOURCE_TYPE_BMP, g_icoBK, 				sizeof(g_icoBK));
	Resource_SetResource(IDR_ICO_TONEON				, RESOURCE_TYPE_JPG, g_icoToneOn, 			sizeof(g_icoToneOn));
	Resource_SetResource(IDR_ICO_RXMUTEON			, RESOURCE_TYPE_JPG, g_icoRxMuteOn, 		sizeof(g_icoRxMuteOn));
	Resource_SetResource(IDR_ICO_RXMUTEOFF			, RESOURCE_TYPE_JPG, g_icoRxMuteOff, 		sizeof(g_icoRxMuteOff));
	Resource_SetResource(IDR_ICO_BEEP				, RESOURCE_TYPE_BMP, g_icoBeep, 			sizeof(g_icoBeep));
	Resource_SetResource(IDR_CTRL_SENSITIVITY_BG	, RESOURCE_TYPE_JPG, g_ctlSensitivityBg, 	sizeof(g_ctlSensitivityBg));
	Resource_SetResource(IDR_CTRL_SENSITIVITY_FG	, RESOURCE_TYPE_JPG, g_ctlSensitivityFg, 	sizeof(g_ctlSensitivityFg));
	Resource_SetResource(IDR_CTRL_BFO_BG			, RESOURCE_TYPE_JPG, g_ctlBfoBg, 			sizeof(g_ctlBfoBg));
	Resource_SetResource(IDR_CTRL_BFO_FG			, RESOURCE_TYPE_JPG, g_ctlBfoFg, 			sizeof(g_ctlBfoFg));
#endif
}

void Resource_SetResource(int nIDR, int nType, const u8* pData, int nSize)
{
	if(nIDR >= IDR_MAX)
		return;
	if(arResource[nIDR] != NULL)
		release_surface(arResource[nIDR]);				
	if(nType == RESOURCE_TYPE_BMP)				// bmp
	{
		arResource[nIDR] = loadbmpp((u8*)pData);
		//debugprintf("0x%08x\r\n", arResource[nIDR]);
	}
#ifndef UNLOAD_RESOURCE
	else if(nType == RESOURCE_TYPE_JPG)			// jpg
	{
		arResource[nIDR] = loadjpgp((u8*)pData, nSize);
		//debugprintf("0x%08x\r\n", arResource[nIDR]);
	}	
#endif
}

void Resource_Release()
{
	int i;
	for(i = 0 ; i < IDR_MAX ; i++)
	{
		if(arResource[i] == NULL)
			release_surface(arResource[i]);
	}
}

void* Resource_GetSurface(int nIDR)
{
#ifndef UNLOAD_RESOURCE	
	if(nIDR < IDR_MAX)
		return arResource[nIDR];
	return NULL;
#else
	return arResource[0];
#endif
}

void Resource_InitializeFont()
{
	arFont[IDF_DEFAULT] = create_bitfont();
#ifndef UNLOAD_FONT
	int i;
	MyFont	arMyFnt[IDF_MAX];
	arMyFnt[IDF_24].pFntInfo	= g_fntInfo24;
	arMyFnt[IDF_24].nInfoSize	= FNT24_INFO_SIZE;
	arMyFnt[IDF_24].pFntData 	= g_fntData24;
	arMyFnt[IDF_24].nDataSize	= FNT24_DATA_SIZE;
	
	arMyFnt[IDF_64].pFntInfo	= g_fntInfo64;
	arMyFnt[IDF_64].nInfoSize	= FNT64_INFO_SIZE;
	arMyFnt[IDF_64].pFntData 	= g_fntData64;
	arMyFnt[IDF_64].nDataSize	= FNT64_DATA_SIZE;		
	
	arMyFnt[IDF_80].pFntInfo	= g_fntInfo80;
	arMyFnt[IDF_80].nInfoSize	= FNT80_INFO_SIZE;
	arMyFnt[IDF_80].pFntData 	= g_fntData80;
	arMyFnt[IDF_80].nDataSize	= FNT80_DATA_SIZE;

	for(i = 1 ; i < IDF_MAX ; i++)
	{
		arFont[i] = Create_MyBmpFnt(&arMyFnt[i]);
		if(arFont[i])	
		{
			bmpfont_setautokerning(arFont[i], true);
			bmpfont_settextencoding(arFont[i], NONE);
		}
	}
#endif
}

void Resource_ReleaseFont()
{
	int i;
	release_bitfont(arFont[IDF_DEFAULT]);
	for(i = 1 ; i < IDF_MAX ; i++)
	{
		if(arFont[i] != NULL)
			release_bmpfont(arFont[i]);
	}
}

EGL_FONT* Resource_GetFont(int nIDR)
{
#ifndef UNLOAD_FONT
	if(nIDR < IDF_MAX)
		return arFont[nIDR];
	return NULL;
#else
	return arFont[0];
#endif
}
