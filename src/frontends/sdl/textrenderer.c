/* 
 * Gmu Music Player
 *
 * Copyright (c) 2006-2012 Johannes Heimansberg (wejp.k.vu)
 *
 * File: textrenderer.c  Created: 060929
 *
 * Description: Bitmap font renderer
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2 of
 * the License. See the file COPYING in the Gmu's main directory
 * for details.
 */
#include <string.h>
#include "textrenderer.h"
#include "SDL.h"
#include "SDL_image.h"
#include "charset.h"

#include "skin.h"
#if 1

#include "bitmap.h"
#include "bitmapfont_11x11.h"

extern Skin                skin;

unsigned short utf8_to_unicode(unsigned char c1, unsigned char c2, unsigned char c3)
{
	unsigned short c=0;
	
	if ((c1 & 0xf0) == 0xe0)
		if ((c2 & 0xc0) == 0x80)
			if ((c3 & 0xc0) == 0x80)
			{
				c = ((c1 & 0xf) << 12) | ((c2 & 0x3f) << 6) | (c3 & 0x3f);
				return c;
			}
			
	return 0;
}

void draw_char_retroarch_eng(SDL_Surface* surface, unsigned char symbol, int x, int y, unsigned long color) 
{
	unsigned long *dst;

	int l, u;
	
	for (l = 0; l < FONT_HEIGHT; l++)
	{
		dst=(unsigned long*)surface->pixels + (surface->pitch / 4)*(y+l) + x;
		for (u = 0; u < FONT_WIDTH; u++)
		{
			unsigned char rem = 1 << (((u) + (l) * FONT_WIDTH) & 7);
			unsigned char offset  = ((u) + (l) * FONT_WIDTH) >> 3;
			if ((bitmap_bin[FONT_OFFSET(symbol) + offset] & rem) > 0)	{								
				*dst = color;
			}
			dst++;

		}
	}
}

void draw_char_kor(SDL_Surface* surface, unsigned short symbol, int x, int y, unsigned long color) 
{
	unsigned long *dst;

	int l, u;
	
	for (l = 0; l < FONT_KOR_HEIGHT; l++)
	{
		dst=(unsigned long*)surface->pixels + (surface->pitch / 4)*(y+l) + x;
		for (u = 0; u < FONT_KOR_WIDTH; u++)
		{
			unsigned char rem = 1 << (((u) + (l) * FONT_KOR_WIDTH) & 7);
			unsigned char offset  = ((u) + (l) * FONT_KOR_WIDTH) >> 3;
			if ((bitmap_kor_bin[FONT_KOR_OFFSET(symbol-0xac00) + offset] & rem) > 0)	{								
				*dst = color;
			}
			dst++;

		}
	}
}
#endif


int textrenderer_init(TextRenderer *tr, char *chars_file, int chwidth, int chheight)
{
	int          result = 0;
	SDL_Surface *tmp = IMG_Load(chars_file);

	tr->chars = NULL;
	if (tmp) {
		if ((tr->chars = SDL_DisplayFormatAlpha(tmp))) {
			tr->chwidth  = chwidth;
			tr->chheight = chheight;
			result = 1;
		}
		SDL_FreeSurface(tmp);
	}
	return result;
}

void textrenderer_free(TextRenderer *tr)
{
	if (tr->chars != NULL) {
		SDL_FreeSurface(tr->chars);
		tr->chars = NULL;
	}
}

void textrenderer_draw_char(const TextRenderer *tr, UCodePoint ch, SDL_Surface *target, int target_x, int target_y)
{
	const int n = (ch - '!') * tr->chwidth;
	SDL_Rect  srect, drect;

	if (ch >= 0xac00 && ch <= 0xd7a3) {
		if (tr == &skin.font2)
			draw_char_kor(target, ch, target_x, target_y, 0x3e76ff);
		else
			draw_char_kor(target, ch, target_x, target_y, 0xffffff);
		
	}
	else {
		if (n >= 0) {
			srect.x = 1 + n;
			srect.y = 1;
			srect.w = tr->chwidth;
			srect.h = tr->chheight;

			drect.x = target_x;
			drect.y = target_y;
			drect.w = 1;
			drect.h = 1;

			SDL_BlitSurface(tr->chars, &srect, target, &drect);
		}
	}
	
}

void textrenderer_draw_string_codepoints(const TextRenderer *tr, const UCodePoint *str, int str_len, SDL_Surface *target, int target_x, int target_y)
{
	int i;
	for (i = 0; i < str_len && str[i]; i++)
		textrenderer_draw_char(tr, str[i], target, target_x + i * (tr->chwidth + 1), target_y);
}

void textrenderer_draw_string(const TextRenderer *tr, const char *str, SDL_Surface *target, int target_x, int target_y)
{
	int utf8_chars = charset_utf8_len(str)+1;
	UCodePoint *ustr = utf8_chars > 0 ? malloc(sizeof(UCodePoint) * (utf8_chars+1)) : NULL;

	if (ustr && charset_utf8_to_codepoints(ustr, str, utf8_chars)) {
		textrenderer_draw_string_codepoints(tr, ustr, utf8_chars, target, target_x, target_y);
	}
	if (ustr) free(ustr);
}

int textrenderer_get_string_length(const char *str)
{
	int i, len = (int)strlen(str);
	int len_const = len;
	int utf8_chars = charset_utf8_len(str);

	for (i = 0; i < len_const-1; i++)
		if (str[i] == '*' && str[i+1] == '*') utf8_chars--;
	return utf8_chars;
}

void textrenderer_draw_string_with_highlight(const TextRenderer *tr1, const TextRenderer *tr2,
                                             const char *str, int str_offset,
                                             SDL_Surface *target, int target_x, int target_y,
                                             int max_length, Render_Mode rm)
{
	int highlight = 0;
	int i, j;
	int l = (int)strlen(str);
	int utf8_chars = charset_utf8_len(str)+1;
	UCodePoint *ustr = utf8_chars > 0 ? malloc(sizeof(UCodePoint) * (utf8_chars+1)) : NULL;

	if (rm == RENDER_ARROW) {
		if (str_offset > 0)
			textrenderer_draw_char(tr2, '<', target, target_x, target_y);
		if (textrenderer_get_string_length(str) - str_offset > max_length) {
			textrenderer_draw_char(tr2, '>', target, target_x + (max_length - 1) * (tr2->chwidth + 1), target_y);
			max_length--;
		}
	}

	if (rm == RENDER_CROP) {
		if (utf8_chars > max_length) {
			int current_max = 0;

			for (i = 0, j = 0; j < max_length && str[i] != '\0'; i++, j++) {
				if (str[i] == '*' && i+1 < l && str[i+1] == '*') j-=2;
				if (str[i] == ' ') current_max = j;
			}
			max_length = current_max;
		}
	}

	if (ustr && charset_utf8_to_codepoints(ustr, str, utf8_chars)) {
		for (i = 0, j = 0; i < utf8_chars && j - str_offset < max_length; i++, j++) {
#if 1
			if (ustr[i] == '*' && i+1 < utf8_chars && ustr[i+1] == '*') {
				highlight = !highlight;
				i+=2;
			}
#else
			if (str[i] == '*' && i+1 < utf8_chars && str[i+1] == '*') {
				highlight = !highlight;
				i+=2;
			}
#endif
			if (j >= str_offset && (j != str_offset || str_offset == 0)) {
				if (!highlight)
					textrenderer_draw_char(tr1, ustr[i], target, 
										   target_x + (j-str_offset) * (tr1->chwidth + 1), target_y);
				else
					textrenderer_draw_char(tr2, ustr[i], target, 
										   target_x + (j-str_offset) * (tr2->chwidth + 1), target_y);
			}
		}
	}
	if (ustr) free(ustr);
}
