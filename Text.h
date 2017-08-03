#pragma once

#define LANGUAGE_KOREAN		0
#define LANGUAGE_ENGLISH	1

#define LANGUAGE			LANGUAGE_KOREAN

#if LANGUAGE == LANGUAGE_KOREAN
#define START_GREETING				"ST-2710 ���� ����\r\n"
#define	SETTING_KEY_SOUND			"1. Ű ������"
#define	SETTING_INTERNAL_SPEAKER	"2. ���� ����Ŀ ���"
#define	SETTING_EXTERNAL_SPEAKER	"3. �ܺ� ����Ŀ ���"
#define SETTING_SCAN_MIN			"4. SSB/H3E ��ĵ ���� ä��"
#define SETTING_SCAN_MAX			"5. SSB/H3E ��ĵ ���� ä��"
#define	SETTING_LANGUAGE			"6. ���"
#define	SETTING_FACTORY_RESET		"7. �ʱ�ȭ"
#define	SETTING_SYSTEM_INFO			"8. �ý��� ����"
#else //LANGUAGE == LANGUAGE_ENGLISH
#define START_GREETING				"ST-2710 Operation Started\r\n"
#define	SETTING_KEY_SOUND			"1. Key Sound"
#define	SETTING_INTERNAL_SPEAKER	"2. Internal Speaker"
#define	SETTING_EXTERNAL_SPEAKER	"3. External Speaker"
#define SETTING_SCAN_MIN			"4. SSB/H3E ��ĵ ���� ä��"
#define SETTING_SCAN_MAX			"5. SSB/H3E ��ĵ ���� ä��"
#define	SETTING_LANGUAGE			"6. Language"
#define	SETTING_FACTORY_RESET		"7. Factory Reset"
#define	SETTING_SYSTEM_INFO			"8. System Info"
#endif
