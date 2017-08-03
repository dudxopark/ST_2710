
#pragma once

#include "Common.h"

#define MODEL_NO		"ST-2710"
#define SW_VERSION		"0.10"
#define HW_VERSION		"P10-3940-A0"

typedef struct _CONFIG
{
	u8 		nOpMode;			// Test Mode(��� �˻� ���), Normal Mode
	u8 		nTxMode;			// None, J3E, H3E, AMP
	u8 		nRxMode;			// None, J3E, H3E, AM, FM, VHF
	s8 		nBright;			// ��� 
	u8 		nClarify;			// ��ᵵ
	bool	bUseKeySound;		// Ű ���� ����
	bool 	bUseInSpk;			// ���� ����Ŀ ��� ����
	bool 	bUseOutSpk;			// �ܺ� ����Ŀ ��� ����
	u8		nLanguage;			// ���. 0: �ѱ���, 1: ����,...
}T_CONFIG;

extern T_CONFIG g_stOV;			// Operation Variable

// J3E ä�� 300��(1~9 ����. 10 ~ 300)
// AM ä�� 30��, FM ä�� 30��, VHF ä�� 63 + 50 ��

typedef  struct 
{	
	char   	*m_pName;				// �޴� �̸�
		
	U16		m_nMnuX;				// �޴� X��ġ
	U16		m_nMnuY;				// �޴� Y��ġ
	
	U16		m_nMnuW;				// �޴� ��ü ����
	U16		m_nMnuH;				// �޴� ��ü ����
	
	U16		m_nNameX;				// �޴� �̸�  X��ġ
	U16		m_nVarX;				// �޴� Variable X ��ġ

	U8		m_nVarType;				// Variable�� Type
	void	*m_pVarValue;			// Variable
	U8		m_nVarSize;				// VarName �� ���õ� �� �ִ� ����
	char	**m_pVarMsg;			// VarName �� ���� ���� ǥ�� �� Message
	U8  	(*Proc)(void *,U16);	// ���ý� ������ Action
} MENU;

#define INVALID_CH				0xffffffff
#define INVALID_FREQUENCY		0xffffffff
#define DEFAULT_BRIGHT			3

typedef struct{
	U16	uChNo;
	U32	uFreq;
}T_CH_INFO;

