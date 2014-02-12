/* 
 * Gmu Music Player
 *
 * Copyright (c) 2006-2013 Johannes Heimansberg (wejp.k.vu)
 *
 * File: medialib.h  Created: 130627
 *
 * Description: Gmu media library
 */
#ifndef WEJ_MEDIALIB_H
#define WEJ_MEDIALIB_H
#include <sqlite3.h>
#include "trackinfo.h"

typedef struct GmuMedialib {
	sqlite3      *db;
	int           refresh_in_progress;
	sqlite3_stmt *pp_stmt_search, *pp_stmt_browse;
} GmuMedialib;

typedef enum {
	GMU_MLIB_ANY, GMU_MLIB_ARTIST, GMU_MLIB_TITLE, GMU_MLIB_ALBUM, 
} GmuMedialibDataType;

int  medialib_create_db_and_open(GmuMedialib *gm);
int  medialib_open(GmuMedialib *gm);
void medialib_close(GmuMedialib *gm);
int  medialib_start_refresh(GmuMedialib *gm, void (*finished_callback)(void));
int  medialib_is_refresh_in_progress(GmuMedialib *gm);
void medialib_refresh(GmuMedialib *gm);
int  medialib_add_file(GmuMedialib *gm, char *file);
void medialib_path_add(GmuMedialib *gm, char *path);
void medialib_path_remove(GmuMedialib *gm, char *path);
/* Search the medialib; Returns the number of results */
int  medialib_search_find(GmuMedialib *gm, GmuMedialibDataType type, char *str);
TrackInfo medialib_search_fetch_next_result(GmuMedialib *gm);
void medialib_search_finish(GmuMedialib *gm);
int  medialib_browse(GmuMedialib *gm, char *sel_column, ...);
char *medialib_browse_fetch_next_result(GmuMedialib *gm);
void medialib_browse_finish(GmuMedialib *gm);
TrackInfo medialib_get_data_for_id(GmuMedialib *gm, int id);
/* Increase/decrease/set rating for a given track */
int  medialib_rate_track_up(GmuMedialib *gm, int id);
int  medialib_rate_track_down(GmuMedialib *gm, int id);
int  medialib_rate_track(GmuMedialib *gm, int id, int rating);
#endif