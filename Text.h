#pragma once

#define LANGUAGE_KOREAN		0
#define LANGUAGE_ENGLISH	1

#define LANGUAGE			LANGUAGE_KOREAN

#if LANGUAGE == LANGUAGE_KOREAN
#define START_GREETING				"ST-2710 동작 시작\r\n"
#define	SETTING_KEY_SOUND			"1. 키 동작음"
#define	SETTING_INTERNAL_SPEAKER	"2. 내부 스피커 사용"
#define	SETTING_EXTERNAL_SPEAKER	"3. 외부 스피커 사용"
#define SETTING_SCAN_MIN			"4. SSB/H3E 스캔 시작 채널"
#define SETTING_SCAN_MAX			"5. SSB/H3E 스캔 종료 채널"
#define	SETTING_LANGUAGE			"6. 언어"
#define	SETTING_FACTORY_RESET		"7. 초기화"
#define	SETTING_SYSTEM_INFO			"8. 시스템 정보"
#else //LANGUAGE == LANGUAGE_ENGLISH
#define START_GREETING				"ST-2710 Operation Started\r\n"
#define	SETTING_KEY_SOUND			"1. Key Sound"
#define	SETTING_INTERNAL_SPEAKER	"2. Internal Speaker"
#define	SETTING_EXTERNAL_SPEAKER	"3. External Speaker"
#define SETTING_SCAN_MIN			"4. SSB/H3E 스캔 시작 채널"
#define SETTING_SCAN_MAX			"5. SSB/H3E 스캔 종료 채널"
#define	SETTING_LANGUAGE			"6. Language"
#define	SETTING_FACTORY_RESET		"7. Factory Reset"
#define	SETTING_SYSTEM_INFO			"8. System Info"
#endif
