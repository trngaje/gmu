/* 
 * Gmu Music Player
 *
 * Copyright (c) 2006-2015 Johannes Heimansberg (wej.k.vu)
 *
 * File: about.c  Created: 061223
 *
 * Description: Program info
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2 of
 * the License. See the file COPYING in the Gmu's main directory
 * for details.
 */
#include "kam.h"
#include "textbrowser.h"
#include "about.h"
#include "core.h"

#if 1
static const char *text_about_gmu = 
	"이 프로그램에서 사용하는 라이브러리는:\n\n"
	"- SDL, SDL_Image, SDL_gfx (옵션)\n\n"
	"디코더 플러그인이 디코딩을 위한 추가 라이브러리를 사용합니다.\n"
	"Johannes Heimansberg (**wej.k.vu**)이 프로그램 했습니다.\n\n"
	"태릉|아재 (trngaje)에 의해 한글화 되었습니다.\n\n"
	"보다 자세한 내용과 설정과 관련된 힌트를 얻으려면 README.txt를\n"
	"보기 바랍니다. 내장된 도움말 화면도 정보를 얻을 수 있습니다.\n"
	"프로젝트 웹사이트:\n"
	"**http://wej.k.vu/projects/gmu/**\n\n"
	"Gmu 는 프리소프트웨어입니다: GPL2. 라이센스에 따라\n"
	"재배포 수정을 할 수 있습니다.\n";
#else
static const char *text_about_gmu = 
	"Libraries used by this program:\n\n"
	"- SDL, SDL_Image, SDL_gfx (optional)\n\n"
	"The decoder plugins use additional\n"
	"libraries for decoding.\n\n"
	"Program written by\n"
	"Johannes Heimansberg (**wej.k.vu**)\n\n"
	"Please take a look at the README.txt\n"
	"file for more details and\n"
	"configuration hints. You also might\n"
	"want to check the in-program help\n"
	"screen.\n\n"
	"Project website:\n"
	"**http://wej.k.vu/projects/gmu/**\n\n"
	"Gmu is free software: you can\n"
	"redistribute it and/or modify it under\n"
	"the terms of the GNU General Public\n"
	"License version 2.\n";
#endif

int about_process_action(TextBrowser *tb_about, View *view, View old_view, int user_key_action)
{
	int update = 0;
	switch (user_key_action) {
		case OKAY:
			*view = old_view;
			update = 1;
			break;
		case RUN_SETUP:
			*view = SETUP;
			update = 1;
			break;
		case MOVE_CURSOR_DOWN:
			text_browser_scroll_down(tb_about);
			update = 1;
			break;
		case MOVE_CURSOR_UP:
			text_browser_scroll_up(tb_about);
			update = 1;
			break;
		case MOVE_CURSOR_LEFT:
			text_browser_scroll_left(tb_about);
			update = 1;
			break;
		case MOVE_CURSOR_RIGHT:
			text_browser_scroll_right(tb_about);
			update = 1;
			break;
	}
	return update;
}

void about_init(TextBrowser *tb_about, Skin *skin, char *decoders)
{
	static char txt[1500];

#if 1
	snprintf(txt, sizeof(txt)-1, "이 것은 Gmu 뮤직플레이어입니다.\n\n"
	                    "버젼.........: **"VERSION_NUMBER"**\n"
	                    "빌드된 날짜.....: "__DATE__" "__TIME__"\n"
	                    "인식된 장치.....: %s\n"
	                    "설정파일 경로....: %s\n\n"
	                    "Gmu 는 디코더 플러그인에 따라 다양한 파일 포맷을 지원합니다.\n"
	                    "%s 디코더:\n\n%s\n"
	                    "%s",
	                    gmu_core_get_device_model_name(),
	                    gmu_core_get_config_dir(),
	                    STATIC ? "내장됨" : "로드됨",
	                    decoders,
	                    text_about_gmu);
#else
	snprintf(txt, sizeof(txt)-1, "This is the Gmu music player.\n\n"
	                    "Version.........: **"VERSION_NUMBER"**\n"
	                    "Built on........: "__DATE__" "__TIME__"\n"
	                    "Detected device.: %s\n"
	                    "Config directory: %s\n\n"
	                    "Gmu supports various file formats\n"
	                    "through decoder plugins.\n\n"
	                    "%s decoders:\n\n%s\n"
	                    "%s",
	                    gmu_core_get_device_model_name(),
	                    gmu_core_get_config_dir(),
	                    STATIC ? "Static build with built-in" : "Loaded",
	                    decoders,
	                    text_about_gmu);
#endif

	text_browser_init(tb_about, skin);
	
#if 1
	text_browser_set_text(tb_about, txt, "Gmu 에 대한 설명");
#else
	text_browser_set_text(tb_about, txt, "About Gmu");
#endif
}
