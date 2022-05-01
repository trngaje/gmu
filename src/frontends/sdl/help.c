/* 
 * Gmu GP2X Music Player
 *
 * Copyright (c) 2006-2010 Johannes Heimansberg (wejp.k.vu)
 *
 * File: help.c  Created: 100404
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
#include "help.h"
#include "core.h"

#if 1
static const char *text_help = 
"Gmu 뮤직 플레이어를 사용해 주셔서 감사합니다!\n\n"
"이것은 Gmu의 가장 중요한 기능에 대한 짧은 설명서 입니다.\n\n"
"Gmu 는 장치의 버튼에 의해 작동합니다.\n"
"각각의 버튼은 두가지 기능을 갖을 수 있습니다.\n"
"첫 번째 기능은 버튼을 그냥 눌러서 동작하고,\n"
"두 번째 기능은 모드키와 같이 눌러서 동작합니다.\n"
"모드 키는 **%s ** 버튼 입니다.\n"
"이제 부터 그 키를 \"Mod\" 라고 다음 문장에서는 사용하겠습니다.\n"
"위 아래로 스크롤 하기 위해서는 **%s ** 와\n"
"**%s **버튼을 사용합니다.\n\n"
"Gmu 는 다양한 정보를 표시하기 위해 여러 화면을 가지고 있습니다.\n"
"주요 화면은 파일 탐색기와\n"
"플레이리스트 탐색기와 트랙 정보화면이 있습니다.\n"
"**%s** 키를 눌러 이 화면을 전환할 수 있습니다.\n\n"
"화면과 상관 없는 전체 기능이 있습니다.\n"
"그리고 특정 화면에서만 동작하는 기능이 있습니다.\n\n"
"**중요한 전체 기능은**\n\n"
"재생/앞으로 넘김.......: **%s **\n"
"뒤로 넘김...........: **%s **\n"
"앞으로 탐색..........: **%s **\n"
"뒤로 탐색...........: **%s **\n"
"일시정지............: **%s **\n"
"정지..............: **%s **\n"
"볼륨 업............: **%s **\n"
"볼륨 다운...........: **%s **\n"
"Gmu 종료..........: **%s **\n"
"프로그램 정보.........: **%s **\n"
"버튼 잠금+화면 끄기.....: **%s **\n"
"버튼 잠금해제+화면 켜기...: **%s **\n"
"\n"
"**파일 탐색 기능**\n\n"
"파일 추가/디렉토리 변경.........: **%s **\n"
"디렉토리 추가...............: **%s **\n"
"단일 파일 재생..............: **%s **\n"
"디렉토리로부터 새 플레이리스트 만들기..: **%s **\n"
"\n"
"**플레이 리스트 탐색기 기능**\n\n"
"선택한 파일 재생.......: **%s **\n"
"선택한 파일 큐표시......: **%s **\n"
"선택한 파일 삭제.......: **%s **\n"
"플레이 리스트 초기화.....: **%s **\n"
"재생 모드 변경........: **%s **\n"
"플레이 리스트 저장/불러오기.: **%s **\n"
"\n"
"가장 중요한 기능이 있습니다.\n"
"그다지 사용되지 않는 기능도 있습니다.\n"
"이러한 기능들은 Gmu 폴더 안에 있는 README.txt에서 설명하고 있습니다.\n"
"나중에 도움말 화면을 열고 싶을 때는\n"
"** %s** 버튼을 눌러주세요.\n\n"
"**시작하기**\n\n"
"가장 처음 원하는 것은 특정 트랙을 플레이 리스에 추가 하는 것이라고 생각됩니다.\n"
"이것은 매우 간단합니다. 처음 파일 탐색기 화면에서 **%s **을 사용합니다. \n"
"위에 설명한 버튼을 사용하여 시스템 트리 안에서 단일 파일을 추가하거나 전체 디렉토리를 \n"
"플레이리스트에 추가합니다.\n"
"적어도 한개의 파일이 플레이 리스트에 있으면\n"
"** %s** 버틀을 눌러 재생할 수 있습니다.\n\n"
"Gmu 뮤질 플레이어로 유익한 시간 보내시기 바랍니다.\n";
#else
static const char *text_help = 
"Welcome to the Gmu Music Player!\n\n"
"This is a short introduction to the\n"
"most important functions of Gmu.\n\n"
"Gmu is controlled by the device's buttons.\n"
"Each button can have two functions. The\n"
"primary function will be activated by\n"
"just pressing that button, while the secondary\n"
"function will be activated by pressing the\n"
"modifier key and the button at the same time.\n"
"The modifier button is the **%s **button.\n"
"When referring to it, it will be called \"Mod\"\n"
"in the following text.\n\n"
"To scroll up and down use the **%s **and\n"
"**%s **buttons.\n\n"
"Gmu has several screens displaying various\n"
"things. Its main screens are the file browser,\n"
"the playlist browser and the track info screen.\n"
"You can switch between those by pressing **%s**.\n\n"
"There are global functions which work all the\n"
"time no matter which screen you have selected\n"
"and there are screen dependent functions.\n\n"
"**Important global functions**\n\n"
"Play/Skip forward.......: **%s **\n"
"Skip backward...........: **%s **\n"
"Seek forward............: **%s **\n"
"Seek backward...........: **%s **\n"
"Pause...................: **%s **\n"
"Stop....................: **%s **\n"
"Volume up...............: **%s **\n"
"Volume down.............: **%s **\n"
"Exit Gmu................: **%s **\n"
"Program information.....: **%s **\n"
"Lock buttons+Screen off.: **%s **\n"
"Unlock buttons+Screen on: **%s **\n"
"\n"
"**File browser functions**\n\n"
"Add file/Change dir.....: **%s **\n"
"Add directory...........: **%s **\n"
"Play single file........: **%s **\n"
"New playlist from dir...: **%s **\n"
"\n"
"**Playlist browser functions**\n\n"
"Play selected item......: **%s **\n"
"Enqueue selected item...: **%s **\n"
"Remove selected item....: **%s **\n"
"Clear playlist..........: **%s **\n"
"Change play mode........: **%s **\n"
"Save/load playlist......: **%s **\n"
"\n"
"These are the most important functions. There\n"
"are some more which are typically used less\n"
"frequently. Those functions are explained in\n"
"the README.txt file which comes with Gmu.\n"
"If you wish to open this help screen at a later\n"
"time you can do so by pressing** %s**.\n\n"
"**Getting started**\n\n"
"The first thing you probably want to do is to\n"
"populate the playlist with some tracks.\n"
"This is very simple. First use **%s **to\n"
"navigate to the file browser screen. Once you\n"
"are there, use the buttons described above to\n"
"navigate within the file system tree and to add\n"
"single files or whole directories to the playlist.\n"
"Once there is at least one file in the playlist\n"
"you can start playback by pressing** %s**.\n\n"
"Have fun with the Gmu Music Player.\n"
"/wej"
;
#endif

void help_init(TextBrowser *tb_help, Skin *skin, KeyActionMapping *kam)
{
	static char txt[3072];

	snprintf(txt, 3071, text_help,
	                    key_action_mapping_get_button_name(kam, MODIFIER),
	                    key_action_mapping_get_full_button_name(kam, MOVE_CURSOR_UP),
	                    key_action_mapping_get_full_button_name(kam, MOVE_CURSOR_DOWN),
	                    key_action_mapping_get_full_button_name(kam, GLOBAL_TOGGLE_VIEW),
	                    key_action_mapping_get_full_button_name(kam, GLOBAL_NEXT),
	                    key_action_mapping_get_full_button_name(kam, GLOBAL_PREV),
	                    key_action_mapping_get_full_button_name(kam, GLOBAL_SEEK_FWD),
	                    key_action_mapping_get_full_button_name(kam, GLOBAL_SEEK_BWD),
	                    key_action_mapping_get_full_button_name(kam, GLOBAL_PAUSE),
	                    key_action_mapping_get_full_button_name(kam, GLOBAL_STOP),
	                    key_action_mapping_get_full_button_name(kam, GLOBAL_INC_VOLUME),
	                    key_action_mapping_get_full_button_name(kam, GLOBAL_DEC_VOLUME),
	                    key_action_mapping_get_full_button_name(kam, GLOBAL_EXIT),
	                    key_action_mapping_get_full_button_name(kam, GLOBAL_PROGRAM_INFO),
	                    key_action_mapping_get_full_button_name(kam, GLOBAL_LOCK),
	                    key_action_mapping_get_full_button_name(kam, GLOBAL_UNLOCK),
	                    key_action_mapping_get_full_button_name(kam, FB_ADD_FILE_TO_PL_OR_CHDIR),
	                    key_action_mapping_get_full_button_name(kam, FB_ADD_DIR_TO_PL),
	                    key_action_mapping_get_full_button_name(kam, FB_PLAY_FILE),
	                    key_action_mapping_get_full_button_name(kam, FB_NEW_PL_FROM_DIR),
	                    key_action_mapping_get_full_button_name(kam, PL_PLAY_ITEM),
	                    key_action_mapping_get_full_button_name(kam, PL_ENQUEUE),
	                    key_action_mapping_get_full_button_name(kam, PL_REMOVE_ITEM),
	                    key_action_mapping_get_full_button_name(kam, PL_CLEAR_PLAYLIST),
	                    key_action_mapping_get_full_button_name(kam, PL_TOGGLE_RANDOM),
	                    key_action_mapping_get_full_button_name(kam, PL_SAVE_PLAYLIST),
	                    key_action_mapping_get_full_button_name(kam, GLOBAL_HELP),
	                    key_action_mapping_get_full_button_name(kam, GLOBAL_TOGGLE_VIEW),
	                    key_action_mapping_get_full_button_name(kam, GLOBAL_NEXT));

	text_browser_init(tb_help, skin);
#if 1
	text_browser_set_text(tb_help, txt, "Gmu 도움말");
#else
	text_browser_set_text(tb_help, txt, "Gmu Help");
#endif
}

int help_process_action(TextBrowser *tb_help, View *view, View old_view, int user_key_action)
{
	int update = 0;
	switch (user_key_action) {
		case OKAY:
			*view = old_view;
			update = 1;
			break;
		case MOVE_CURSOR_DOWN:
			text_browser_scroll_down(tb_help);
			update = 1;
			break;
		case MOVE_CURSOR_UP:
			text_browser_scroll_up(tb_help);
			update = 1;
			break;
		case MOVE_CURSOR_LEFT:
			text_browser_scroll_left(tb_help);
			update = 1;
			break;
		case MOVE_CURSOR_RIGHT:
			text_browser_scroll_right(tb_help);
			update = 1;
			break;
	}
	return update;
}
