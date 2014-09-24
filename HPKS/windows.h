/*
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 * 
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* 
 * File:   windows.h
 * Author: Christian Rost  <rost at entronix.de>  <rost at entronix.de>
 *
 * Created on 12. April 2013, 21:11
 */

#ifndef WINDOWS_H
#define	WINDOWS_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <panel.h>
#include <ncurses.h>
  
typedef struct _win_border_struct {
  chtype ls, rs, ts, bs, 
  tl, tr, bl, br;
}WIN_BORDER;
typedef struct _WIN_struct {
  int startx, starty;
  int height, width;
  WIN_BORDER border;
}WIN;  
 
typedef enum
{
  PANEL_INIT,
  PANEL_STATE,
  PANEL_DBG,
  TOTAL_PANELS
}OWN_PANEL;

int init_ncurses();
WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);


void init_win_params(WIN *p_win);
void print_win_params(WIN *p_win);
void create_box(WINDOW *win, bool flag);

void create_wnd();


void wnd_printf(OWN_PANEL panel, int y, int x, char *str,...);

void CreateStatusWindows();

void CreateInitSlider();
void DeleteInitSlider();
void wnd_initUSlider( char *str, ...);

void CreateDebugWindow();
void dbg_wnd_printf(char *str,...);
#ifdef	__cplusplus
}
#endif

#endif	/* WINDOWS_H */

