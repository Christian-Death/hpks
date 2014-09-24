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
 * File:   windows.c
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 08. June 2012, 19:04
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <cdk/cdk.h>

#include "defines/used_includes.h"
#include "windows.h"

#define CDK_CONST /*nothing*/
#define CDK_CSTRING CDK_CONST char *
#define CDK_CSTRING2 CDK_CONST char * CDK_CONST *

/**
 * Different classes of colors.
 */
typedef enum
{
	OWN_COLOR_NORMAL = 1,
  OWN_COLOR_BACKGROUND,
	OWN_COLOR_HIGHLIGHT,		/* eg. when a button is selected */
	OWN_COLOR_DISABLED,		/* eg. when a button is disabled */
	OWN_COLOR_HIGHLIGHT_D,	/* eg. when a button is selected, but some other window is in focus */
	OWN_COLOR_TEXT_NORMAL,
	OWN_COLOR_TEXT_INACTIVE,	/* when the entry is out of focus */
	OWN_COLOR_MNEMONIC,
	OWN_COLOR_MNEMONIC_D,
	OWN_COLOR_SHADOW,
	OWN_COLOR_TITLE,
	OWN_COLOR_TITLE_D,
	OWN_COLOR_URGENT,       /* this is for the 'urgent' windows */
  OWN_COLOR_MENU,
  OWN_COLOR_MENU_BAR,
  OWN_COLOR_MENU_TITLE,
  OWN_COLOR_PANEL,
	OWN_COLORS
} OwnColorType;

enum
{
	OWN_COLOR_BLACK = 0,
	OWN_COLOR_RED,
	OWN_COLOR_GREEN,
	OWN_COLOR_BLUE,
	OWN_COLOR_WHITE,
	OWN_COLOR_GRAY,
	OWN_COLOR_DARK_GRAY,
	OWN_TOTAL_COLORS
};

CDKSCREEN *cdkScreen;
WINDOW *cursesWin;

WINDOW *my_wins[TOTAL_PANELS];
PANEL *my_panels[TOTAL_PANELS];



typedef struct _PANEL_DATA {
 int x, y, w, h;
 int hide; /* TRUE if panel is hidden */
 char *label; 
 int label_color;
 int color;
 
}PANEL_DATA;

#define NLINES 25
#define NCOLS 80

#define INIT_PANEL_HEIGHT 10
#define INIT_PANEL_WIDTH 60

char strInit[] = "Initialisierung";
char strDbg[] = "Debug Ausgaben";
PANEL_DATA panel_datas[TOTAL_PANELS] = {
  {(NCOLS/2)-(INIT_PANEL_WIDTH/2), (NLINES/2)-(INIT_PANEL_HEIGHT/2),
          INIT_PANEL_WIDTH, INIT_PANEL_HEIGHT,
          1, strInit, OWN_COLOR_PANEL, OWN_COLOR_PANEL},
  {0,0,40,20, 1, strInit, 1, 0},
          
  {0,20,80,4, 1, strDbg,  OWN_COLOR_PANEL, OWN_COLOR_PANEL},
};


bool hascolors;
static struct
{
	short r, g, b;
} colors[OWN_TOTAL_COLORS];

static void
backup_colors(void)
{
	short i;
	for (i = 0; i < OWN_TOTAL_COLORS; i++)
	{
		color_content(i, &colors[i].r,
				&colors[i].g, &colors[i].b);
	}
}
void init_colors() {
  int ret;
  start_color(); /* Start the color functionality */
  if (!(hascolors = has_colors()))
    return;
  if(can_change_color()){
    backup_colors();
    /* Do some init_color()s */
    ret = init_color(OWN_COLOR_BLACK, 0, 0, 0);
    ret = init_color(OWN_COLOR_RED, 1000, 0, 0);
    ret = init_color(OWN_COLOR_GREEN, 0, 1000, 0);
    //ret = init_color(OWN_COLOR_BLUE, 250, 250, 700);
    ret = init_color(OWN_COLOR_BLUE, 0, 0, 1000);
    ret = init_color(OWN_COLOR_WHITE, 0, 1000, 1000);
    ret = init_color(OWN_COLOR_GRAY, 699, 699, 699);
    ret = init_color(OWN_COLOR_DARK_GRAY, 256, 256, 256);

    /* Now some init_pair()s */
    ret = init_pair(OWN_COLOR_NORMAL, OWN_COLOR_BLACK, OWN_COLOR_WHITE);
    ret = init_pair(OWN_COLOR_BACKGROUND, OWN_COLOR_WHITE, OWN_COLOR_BLUE);
    ret = init_pair(OWN_COLOR_HIGHLIGHT, OWN_COLOR_WHITE, OWN_COLOR_BLUE);
    ret = init_pair(OWN_COLOR_SHADOW, OWN_COLOR_BLACK, OWN_COLOR_DARK_GRAY);

    ret = init_pair(OWN_COLOR_TITLE, OWN_COLOR_WHITE, OWN_COLOR_BLUE);
    ret = init_pair(OWN_COLOR_TITLE_D, OWN_COLOR_WHITE, OWN_COLOR_GRAY);

    ret = init_pair(OWN_COLOR_TEXT_NORMAL, OWN_COLOR_WHITE, OWN_COLOR_BLUE);
    ret = init_pair(OWN_COLOR_HIGHLIGHT_D, OWN_COLOR_BLACK, OWN_COLOR_GRAY);
    ret = init_pair(OWN_COLOR_DISABLED, OWN_COLOR_GRAY, OWN_COLOR_WHITE);
    ret = init_pair(OWN_COLOR_URGENT, OWN_COLOR_WHITE, OWN_COLOR_RED);

    ret = init_pair(OWN_COLOR_MENU,OWN_COLOR_WHITE,OWN_COLOR_BLUE);
    ret = init_pair(OWN_COLOR_MENU_BAR, OWN_COLOR_BLUE, OWN_COLOR_WHITE);
    ret = init_pair(OWN_COLOR_MENU_TITLE, OWN_COLOR_RED, OWN_COLOR_WHITE);
    
    ret = init_pair(OWN_COLOR_PANEL, OWN_COLOR_BLACK, OWN_COLOR_WHITE);
  }
  else
	{
		int bg;

		if (use_default_colors() == OK) {
			init_pair(OWN_COLOR_NORMAL, -1, -1);
			bg = -1;
		} else {
			init_pair(OWN_COLOR_NORMAL, COLOR_BLACK, COLOR_WHITE);
			bg = COLOR_WHITE;
		}
    init_pair(OWN_COLOR_BACKGROUND, COLOR_WHITE, COLOR_BLUE);
		init_pair(OWN_COLOR_DISABLED, COLOR_YELLOW, bg);
		init_pair(OWN_COLOR_URGENT, COLOR_GREEN, bg);

		init_pair(OWN_COLOR_HIGHLIGHT, COLOR_WHITE, COLOR_BLUE);
		init_pair(OWN_COLOR_SHADOW, COLOR_BLACK, COLOR_BLACK);
		init_pair(OWN_COLOR_TITLE, COLOR_WHITE, COLOR_BLUE);
		init_pair(OWN_COLOR_TITLE_D, COLOR_WHITE, COLOR_BLACK);
		init_pair(OWN_COLOR_TEXT_NORMAL, COLOR_WHITE, COLOR_BLUE);
		init_pair(OWN_COLOR_HIGHLIGHT_D, COLOR_CYAN, COLOR_BLACK);
    
    init_pair(OWN_COLOR_MENU, COLOR_WHITE, COLOR_BLUE);
    init_pair(OWN_COLOR_MENU_BAR, COLOR_BLUE, COLOR_WHITE);
    init_pair(OWN_COLOR_MENU_TITLE, COLOR_RED, COLOR_WHITE);
    
    init_pair(OWN_COLOR_PANEL, COLOR_BLACK, COLOR_WHITE);
	}
}

int init_ncurses() {
#if 0
  setlocale(LC_ALL, "");
  initscr(); /* Start curses mode */
  raw(); /* Line buffering disabled */
  cbreak(); /* Line buffering disabled, Pass on
 * everty thing to me */
  
  keypad(stdscr, TRUE); /* We get F1, F2 etc.. */
  noecho(); /* Don't echo() while we do getch */
  curs_set(0); /*Cursor aus*/
  


  init_colors();
#else
  /* Initialisation CDK */
  cursesWin = initscr();
  cdkScreen = initCDKScreen(cursesWin);
  /* Initialisation des couleurs CDK */
  initCDKColor();
  //init_colors();       
#endif
}




void create_wnd() {
 
  CreateDebugWindow();

  

  /* On affiche */
  refreshCDKScreen (cdkScreen);

}

//* StatusWINDOW
CDKLABEL *label1, *label2, *label3, *label4, *label5, *label6, *instruct;
 
const char *mesg1[3];
const char *mesg2[3];
const char *mesg3[3];
const char *mesg4[3];
const char *mesg5[3];
const char *mesg6[3];

void deinit_window(){
/*
  del_panel(panel_init);
  //del_panel(pan2);
  delwin(wnd_init);
  //delwin(win2);
*/
    /* Clean up */
   destroyCDKLabel (label1);
   destroyCDKLabel (label2);
   destroyCDKLabel (label3);
   destroyCDKLabel (label4);
   destroyCDKLabel (label5);
   destroyCDKLabel (label6);
   destroyCDKScreen (cdkScreen);
   endCDK ();
}

void CreateStatusWindows()
{


   mesg1[0] = " Brennertemperatur           ";
   mesg1[1] = "                             ";
   mesg1[2] = "                             ";

   label1 = newCDKLabel (cdkScreen, 10, 2, (CDK_CSTRING2) mesg1, 3, TRUE, FALSE);
  // setCDKLabelULChar (label1, '1' | A_BOLD);

   mesg2[0] = " Puffertemperatur            ";
   mesg2[1] = "                             ";
   mesg2[2] = "                             ";

   label2 = newCDKLabel (cdkScreen, 41, 2, (CDK_CSTRING2) mesg2, 3, TRUE, FALSE);
  // setCDKLabelULChar (label2, '2' | A_BOLD);

   mesg3[0] = " Sauerstoffgehalt            ";
   mesg3[1] = "                             ";
   mesg3[2] = "                             ";

   label3 = newCDKLabel (cdkScreen, 10, 7, (CDK_CSTRING2) mesg3, 3, TRUE, FALSE);
   //setCDKLabelULChar (label3, '3' | A_BOLD);

   mesg4[0] = " Holzmenge welche nachgelegt ";
   mesg4[1] = " werden muss                 ";
   mesg4[2] = "                             ";

   label4 = newCDKLabel (cdkScreen, 41, 7, (CDK_CSTRING2) mesg4, 3, TRUE, FALSE);
   //setCDKLabelULChar (label4, '4' | A_BOLD);   
  
   mesg5[0] = " Abgas                       ";
   mesg5[1] = "                             ";
   mesg5[2] = "                             ";

   label5 = newCDKLabel (cdkScreen, 10, 12, (CDK_CSTRING2) mesg5, 3, TRUE, FALSE);
   //setCDKLabelULChar (label3, '3' | A_BOLD);
   
   mesg6[0] = " Verlust                     ";
   mesg6[1] = "                             ";
   mesg6[2] = "                             ";

   label6 = newCDKLabel (cdkScreen, 41, 12, (CDK_CSTRING2) mesg6, 3, TRUE, FALSE);
   //setCDKLabelULChar (label3, '3' | A_BOLD);
   
   drawCDKLabel(label1, ObjOf (label1)->box);
   drawCDKLabel(label2, ObjOf (label2)->box);
   drawCDKLabel(label3, ObjOf (label3)->box);
   drawCDKLabel(label4, ObjOf (label4)->box);
   drawCDKLabel(label5, ObjOf (label5)->box);
   drawCDKLabel(label6, ObjOf (label6)->box);
}

void DeleteStatusWindows()
{
  //destroyCDKUSlider (USliderInitwidget);
}

void wnd_StatusWindows()
{
  char buf[10];
  MEASUREMENT_VIEW_struct * CPUTempMem = cpu_temp_getViewMem();
  MEASUREMENT_VIEW_struct * O2Mem = lambda_getViewMem();
  MEASUREMENT_VIEW_struct * BrennerMem = temp_brenner_getViewMem();
  MEASUREMENT_VIEW_struct * PufferMem = ds12b20_getViewMem();
  MEASUREMENT_VIEW_struct * AbgasMem = pt100_getViewMem(); 

  double KWh = (app_config_get_max_volume() * app_config_get_spez_wasser() * app_config_get_max_puffer_temp() - ds18b20_getAverage()) / (3600 * 1000);
  double kg = KWh / app_config_get_heizwert_holz();
    
  sprintf(buf, " %s", BrennerMem->cur_value);
  mesg1[2] = copyChar(buf);
  sprintf(buf, " %s", PufferMem->cur_value);
  mesg2[2] = copyChar(buf);
  sprintf(buf, " %s", O2Mem->cur_value);
  mesg3[2] = copyChar(buf);
  sprintf(buf, " %3.1f kg", kg);
  mesg4[2] = copyChar(buf);
  
  sprintf(buf, " %s", AbgasMem->cur_value);
  mesg5[2] = copyChar(buf);
  double abgasverlust = get_abgasverlust();
  sprintf(buf, " %4u", abgasverlust);
  mesg6[2] = copyChar(buf);  
  
 
  setCDKLabelMessage(label1,(CDK_CSTRING2) mesg1, 3);
  setCDKLabelMessage(label2,(CDK_CSTRING2) mesg2, 3);
  setCDKLabelMessage(label3,(CDK_CSTRING2) mesg3, 3);
  setCDKLabelMessage(label4,(CDK_CSTRING2) mesg4, 3);
  setCDKLabelMessage(label5,(CDK_CSTRING2) mesg5, 3);
  setCDKLabelMessage(label6,(CDK_CSTRING2) mesg6, 3);
}

//* Sliderwindow
CDKUSLIDER* USliderInitwidget;
char *labelInitSlider		= "<C></5>                               ";
void CreateInitSlider()
{
  char *titleInitSlider		= "<C></5>Initialisierung";
  
  /* Create the widget. */
  USliderInitwidget = newCDKUSlider (cdkScreen, // CDKSCREEN *cdkscreen,
    			  CENTER, // int xpos,
    			  CENTER, // int ypos,
    			 titleInitSlider, labelInitSlider, // const char *title, const char *label,
    			 A_REVERSE | COLOR_PAIR (29) | ' ', // chtype fillerCharacter,
    			 20, // int fieldWidth,
    			 1, 1, 20, // int currentValue,unsigned lowValue,unsigned highValue,
    			 1, (1 * 2), // unsigned increment,unsigned fastIncrement,
    			 TRUE, // boolean box,
    			 TRUE); // boolean shadow
  

   //setCDKUSliderBackgroundColor(USliderInitwidget, "</47>");      
}

void DeleteInitSlider()
{
  destroyCDKUSlider (USliderInitwidget);
}

void wnd_initUSlider( char *str, ...)
{
  va_list arglist; 
  va_start(arglist, str); 
  char plop[80];
  char buf[80];
  vsnprintf(plop, 20, str , arglist);
  sprintf(buf, "<C></5>%s" , plop);
  int junk;
  
  USliderInitwidget->label = char2Chtype (labelInitSlider, &USliderInitwidget->labelLen, &junk);
  refreshCDKScreen (cdkScreen);
  USliderInitwidget->label = char2Chtype (buf, &USliderInitwidget->labelLen, &junk);
  setCDKUSliderValue(USliderInitwidget, USliderInitwidget->current+USliderInitwidget->inc);
  refreshCDKScreen (cdkScreen);
}


void wnd_printf(OWN_PANEL panel, int y, int x, char *str,...)
{
  WINDOW *win = my_wins[panel];

  mvwprintw(win, 2, 2, "Test");
 
}


//* DEBUGWINDOW
CDKSWINDOW *sDebugWindow; // 
void CreateDebugWindow()
{
  char *title2		= "<C></5>Informations";
  sDebugWindow = newCDKSwindow(cdkScreen,0,20,5,80,title2,100,true,false);
  drawCDKSwindow (sDebugWindow, ObjOf (sDebugWindow)->box);
}

void DeleteDebugWindow()
{
  destroyCDKSwindow (sDebugWindow);
}

void dbg_wnd_printf(char *str, ...){
  va_list arglist; 
  va_start(arglist, str); 
  
  char plop[80];
  vsnprintf(plop, 79, str, arglist);
  addCDKSwindow(sDebugWindow, plop, TOP);

}