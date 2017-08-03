
#pragma once

#include "Common.h"

#define MODEL_NO		"ST-2710"
#define SW_VERSION		"0.10"
#define HW_VERSION		"P10-3940-A0"

typedef struct _CONFIG
{
	u8 		nOpMode;			// Test Mode(장비 검사 모드), Normal Mode
	u8 		nTxMode;			// None, J3E, H3E, AMP
	u8 		nRxMode;			// None, J3E, H3E, AM, FM, VHF
	s8 		nBright;			// 밝기 
	u8 		nClarify;			// 명료도
	bool	bUseKeySound;		// 키 사운드 여부
	bool 	bUseInSpk;			// 내부 스피커 사용 여부
	bool 	bUseOutSpk;			// 외부 스피커 사용 여부
	u8		nLanguage;			// 언어. 0: 한국어, 1: 영어,...
}T_CONFIG;

extern T_CONFIG g_stOV;			// Operation Variable

// J3E 채널 300개(1~9 고정. 10 ~ 300)
// AM 채널 30개, FM 채널 30개, VHF 채널 63 + 50 개

typedef  struct 
{	
	char   	*m_pName;				// 메뉴 이름
		
	U16		m_nMnuX;				// 메뉴 X위치
	U16		m_nMnuY;				// 메뉴 Y위치
	
	U16		m_nMnuW;				// 메뉴 전체 넓이
	U16		m_nMnuH;				// 메뉴 전체 높이
	
	U16		m_nNameX;				// 메뉴 이름  X위치
	U16		m_nVarX;				// 메뉴 Variable X 위치

	U8		m_nVarType;				// Variable의 Type
	void	*m_pVarValue;			// Variable
	U8		m_nVarSize;				// VarName 이 선택될 수 있는 갯수
	char	**m_pVarMsg;			// VarName 의 값에 따라 표출 될 Message
	U8  	(*Proc)(void *,U16);	// 선택시 실행할 Action
} MENU;

#define INVALID_CH				0xffffffff
#define INVALID_FREQUENCY		0xffffffff
#define DEFAULT_BRIGHT			3

typedef struct{
	U16	uChNo;
	U32	uFreq;
}T_CH_INFO;

