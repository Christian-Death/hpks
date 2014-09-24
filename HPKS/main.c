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
 * File:   main.c
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 27. November 2012, 19:04
 */


#include <stdio.h>
#include <stdlib.h>

#include "defines/used_includes.h"

/*#### DEFINES ###################*/
#define BUFFER_LEN 20

/*#### TYPEDEFS ###################*/
typedef void (*IntFunctionWithNoParameter) (void);

typedef struct TIMING_FUNCT_struct {
  IntFunctionWithNoParameter func;
  uint16_t any_ms;
  uint16_t still_ms;
} TIMING_FUNCT_t;

/*#### VARIABLE ###################*/
char app_fullpath[FILENAME_MAX];

static TIMING_FUNCT_t func[] = {
  {.func = temp_brenner_getData, .any_ms = 1000}, // 1s
  {.func = temp_abgas_getData, .any_ms = 1000}, // 1s
  {.func = ds18b20_getData, .any_ms = 500}, // 100ms
  {.func = cpu_temp_getData, .any_ms = 5000}, // 5s
  {.func = lambda_getData, .any_ms = 500}, // 500ms
  {.func = lambda_check_getData, .any_ms = 500}, // 500ms
  {.func = pt100_getData, .any_ms = 2000}, // 2s
  {.func = scan_inputs, .any_ms = 10}, // 1ms
  {.func = event_work, .any_ms = 1}, // 1ms
  {.func = work_regelung, .any_ms = 1000}, // 100ms
  {.func = shaker_work, .any_ms = 100}, // 100ms
};
static int func_count = sizeof (func) / sizeof (func[0]);
static uint8_t cur_func = 0;

//--------------------------------------------------------------------------------------------------
// Name:      RaspiHwInit
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

int RaspiHwInit(void)
{
  if (!bcm2835_init()) return 0;

  spi_init();


  // K-Sonde U4
#ifdef PIN_MAX31855_U4_CS 
  bcm2835_gpio_fsel(PIN_MAX31855_U4_CS, BCM2835_GPIO_FSEL_OUTP); // GPIO25 (PIN 22) Output: CS3
  bcm2835_gpio_set(PIN_MAX31855_U4_CS); // Musst set High for disable
#endif

  // K-Sonde U5
#ifdef PIN_MAX31855_U5_CS   
  bcm2835_gpio_fsel(PIN_MAX31855_U5_CS, BCM2835_GPIO_FSEL_OUTP); // GPIO7 (PIN 26) Output: CS2
  bcm2835_gpio_set(PIN_MAX31855_U5_CS); // Musst set High for disable
#endif

  // A/D Wandler
#ifdef PIN_MCP300X_CS  
  bcm2835_gpio_fsel(PIN_MCP300X_CS, BCM2835_GPIO_FSEL_OUTP); // GPIO24 (PIN 24) Output: CE1
  bcm2835_gpio_set(PIN_MCP300X_CS); // Musst set High for disable  
#endif

#ifdef SHAKER1 
  bcm2835_gpio_fsel(SHAKER1, BCM2835_GPIO_FSEL_OUTP); // GPIO19 (PIN 35) Output: REL1
  bcm2835_gpio_clr(SHAKER1); // Musst set LOW for disable  
#endif
  
#ifdef SHAKER2
  bcm2835_gpio_fsel(SHAKER2, BCM2835_GPIO_FSEL_OUTP); /// GPIO26 (PIN 37) Output: REL2
  bcm2835_gpio_clr(SHAKER2); // Musst set LOW for disable  
#endif

#ifdef INP1
  bcm2835_gpio_fsel(INP1, BCM2835_GPIO_FSEL_INPT); // GPIO5 (PIN 29) Input: INP1
#endif
#ifdef INP2
  bcm2835_gpio_fsel(INP2, BCM2835_GPIO_FSEL_INPT); // GPIO6 (PIN 31) Input: INP2
#endif
#ifdef INP3
  bcm2835_gpio_fsel(INP3, BCM2835_GPIO_FSEL_INPT); // GPIO12 (PIN 32) Input: INP3
#endif
#ifdef INP4
  bcm2835_gpio_fsel(INP4, BCM2835_GPIO_FSEL_INPT); // GPIO13 (PIN 33) Input: INP4
#endif


#ifdef USE_PORTEXPANDER

  bcm2835_gpio_fsel(PIN_MCP23S17_CS, BCM2835_GPIO_FSEL_OUTP); // GPIO25 Output: RST
  bcm2835_gpio_set(PIN_MCP23S17_CS); // Musst set High for disable

  // PORT Expander
  mcp23s17_init();

  //
  // PORTA PIN0 und PIN1 Output
  //
  mcp23s17_direction(PORT_A, 0b11111100);
  mcp23s17_configurePullUps(PORT_A, 0b00000100);

  // LCD_BL,LCD_RS, LCD_RST, BUT_0, BUT_1, BUT_2, BUT_3, BUT_4 
  mcp23s17_direction(PORT_B, 0b00011111);
  mcp23s17_configurePullUps(PORT_B, 0b00011111);

#endif

#ifdef USE_LCD
  // LCD Display
  //bcm2835_gpio_fsel(PIN_LCD_MOSI,     BCM2835_GPIO_FSEL_OUTP);	// GPIO10 Output: MOSI
  //bcm2835_gpio_fsel(PIN_LCD_SCLK,     BCM2835_GPIO_FSEL_OUTP);	// GPIO11 Output: SCLK

  bcm2835_gpio_fsel(PIN_LCD_CS, BCM2835_GPIO_FSEL_OUTP); // GPIO8  Output: CS

#  ifndef USE_PORTEXPANDER
  bcm2835_gpio_fsel(PIN_LCD_RST, BCM2835_GPIO_FSEL_OUTP); // GPIO25 Output: RST
  bcm2835_gpio_fsel(PIN_LCD_RS, BCM2835_GPIO_FSEL_OUTP); // GPIO7  Output: RS
  bcm2835_gpio_fsel(PIN_LCD_BACKLIGHT, BCM2835_GPIO_FSEL_OUTP); // GPIO18 Output: Backlight
#  endif
#endif


  return 1;

}

//--------------------------------------------------------------------------------------------------
// Name:      RaspiHwDeInit
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

int RaspiHwDeInit(void)
{
#ifdef USE_LCD
  SetBacklight(0); // Turn Backlight on 
#endif   

  spi_deinit();
  bcm2835_close();

  return 1;

}


//--------------------------------------------------------------------------------------------------
// Name:      init_komponenten
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

int init_komponenten(void)
{

  uint8_t i;
  uint8_t init_count = 1;
  char buffer[BUFFER_LEN + 1];

#ifdef USE_LCD
  InitialView("Konfig lesen", init_count++);
#endif
  if (app_config_get_use_window())
    wnd_initUSlider("Konfig lesen");

  SleepMs(500);

#ifdef USE_LCD
  InitialView("CPU-Temperatur", init_count++);
#endif
  if (app_config_get_use_window())
    wnd_initUSlider("CPU-Temperatur");
  cpu_temp_init();
  cpu_temp_getData();
  SleepMs(500);

  if (app_config_get_use_k_sonde())
  {
#ifdef USE_LCD
    InitialView("K-Sonde", init_count++);
#endif
    if (app_config_get_use_window())
      wnd_initUSlider("K-Sonde Brenner");
    temp_brenner_init();
    temp_brenner_getData();
    SleepMs(500);
  }

  if (app_config_get_use_ds18b20())
  {
#ifdef USE_LCD
    InitialView("Puffertemp", init_count++);
#endif
    if (app_config_get_use_window())
      wnd_initUSlider("Puffertemp");

    i = ds18b20_getDeviceList();
    while (i--)
    {
      ds18b20_readData();
      SleepMs(500);
    };
    app_config_read_ds18b20();
  }

  if (app_config_get_use_lambdasonde())
  {
#ifdef USE_LCD
    InitialView("Lambda-Sonde", init_count++);
#endif
    if (app_config_get_use_window())
      wnd_initUSlider("Lambda-Sonde");
    lambda_init();
    lambda_getData();
    SleepMs(500);
  }

  if (app_config_get_use_lambdacheck())
  {
#ifdef USE_LCD
    InitialView("Lambda-Sonde", init_count++);
#endif
    if (app_config_get_use_window())
      wnd_initUSlider("Lambda-Check");
    lambda_check_init();
    lambda_check_getData();
    SleepMs(500);
  }


  if (app_config_get_use_abgas_temp() == PT100)
  {
    if (app_config_get_used_abgas_sonde() == PT100)
    {
#ifdef USE_LCD
      InitialView("PT100", init_count++);
#endif
      if (app_config_get_use_window())
        wnd_initUSlider("PT100");
      pt100_init();
      pt100_getData();
    }
    else
    {
#ifdef USE_LCD
      InitialView("K-Sonde", init_count++);
#endif
      if (app_config_get_use_window())
        wnd_initUSlider("K-Sonde Abgas");
      temp_abgas_init();
      temp_abgas_getData();
    }
    SleepMs(500);
  }

#ifdef USE_WEBSERVER
  if (app_config_get_use_server())
  {

#  ifdef USE_LCD
    InitialView("Webserver starten", init_count++);
#  endif
    if (app_config_get_use_window())
      wnd_initUSlider("Webserver starten");
    webserver_start(app_config_get_server_port());
    init_json();
    SleepMs(500);

#  ifdef USE_LCD
    if (get_ipaddr(buffer, BUFFER_LEN) == EXIT_SUCCESS)
    {
      //InitialView(buffer, init_count++);
      /*
            dbg_printf(buffer);
       */
    };
    SleepMs(500);
#  endif
  }
#endif

#ifdef USE_LCD
  InitialView("Fertig", init_count++);
  wnd_initUSlider("Fertig");
#endif  
  SleepMs(2500);
}

//--------------------------------------------------------------------------------------------------
// Name:      get_path
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

void get_path()
{
  int length;

  /*
   * /proc/self is a symbolic link to the process-ID subdir of /proc, e.g.
   * /proc/4323 when the pid of the process of this program is 4323.
   * Inside /proc/<pid> there is a symbolic link to the executable that is
   * running as this <pid>.  This symbolic link is called "exe". So if we
   * read the path where the symlink /proc/self/exe points to we have the
   * full path of the executable. 
   */

  length = readlink("/proc/self/exe", app_fullpath, sizeof (app_fullpath));

  /*
   * Catch some errors: 
   */
  if (length < 0)
  {
    perror("resolving symlink /proc/self/exe.");
    exit(1);
  }
  if (length >= sizeof (app_fullpath))
  {
    fprintf(stderr, "Path too long.\n");
    exit(1);
  }

  char *s = strrchr(app_fullpath, '/');
  *s++ = 0;

  //app_fullpath[length] = '\0';
  dbg_printf("Full path is: %s\n", app_fullpath);
}


//--------------------------------------------------------------------------------------------------
// Name:      sig_handler
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

void sig_handler(int signo)
{
  if (signo == SIGTERM || signo == SIGINT || signo == SIGKILL)
  {
#ifdef USE_WEBSERVER   
    webserver_stop();
#endif  

    RaspiHwDeInit();
    // getch(); /* Wait for user input */

    if (app_config_get_use_window())
      endwin(); /* End curses mode */

    exit(0);
  }
}

//--------------------------------------------------------------------------------------------------
// Name:      init_sig_handler
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

void init_sig_handler()
{
  struct sigaction sigIntHandler;

  sigIntHandler.sa_handler = sig_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  sigaction(SIGINT, &sigIntHandler, NULL);
  sigaction(SIGKILL, &sigIntHandler, NULL);
  sigaction(SIGTERM, &sigIntHandler, NULL);
}

//--------------------------------------------------------------------------------------------------
// Name:      End
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

void End(void)
{
  printf("The End\n");
#ifdef USE_WEBSERVER   
  webserver_stop();
#endif  

  RaspiHwDeInit();

  deinit_window(); /* End curses mode */

  exit(0);
}

//--------------------------------------------------------------------------------------------------
// Name:      time_func_tasks
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

void time_func_tasks()
{
  if (func[cur_func].still_ms == 0)
  {
    func[cur_func].func();
    func[cur_func].still_ms = func[cur_func].any_ms;
  }
  func[cur_func].still_ms--;

  cur_func++;
  if (cur_func >= func_count)
  {
    cur_func = 0;
  }

}

//--------------------------------------------------------------------------------------------------
// Name:      main
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

int main(int argc, char** argv)
{

  int key = ERR;
  uint8_t i;
  WINDOW *my_win;

  // Funktion welche beim beenden aufgerufen wird
  atexit(End);

  get_path();
  init_sig_handler();

  // Kofig lesen
  app_config_read();

  if (app_config_get_use_window())
  {
    init_ncurses();
    create_wnd();
  }

  dbg_printf("Puffer Kontroll System v0.1 [" __DATE__ " " __TIME__"]\n");
  dbg_printf("RaspberryHwRevision=%i\n", GetRaspberryHwRevision());

  if (geteuid() != 0)
  {
    fprintf(stderr, "****You need to be root to properly run this test program");
    return 1;
  }
  if (!RaspiHwInit())
  {
    dbg_printf("RaspiHwInit() failed!");
    return 1;
  }

  millis_init();

#ifdef USE_LCD
  lcd_display_init();
#endif   

  if (app_config_get_use_window())
  {
    CreateInitSlider();
  }

  init_komponenten();
  init_regelung();
  init_shaker();

  if (app_config_get_use_window())
  {
    DeleteInitSlider();
    CreateStatusWindows();
  }



  //timeout(50);

  do
  {
    if (app_config_get_use_window())
    {
      wnd_StatusWindows();
    }
    //button_worker();
#ifdef USE_BUTTONS
    // UpdateButtons();
#endif
#ifdef USE_LCD
    //lcd_display_worker();
#endif
    time_func_tasks();

    //napms(500);
    //key = getch();
    //usleep(100);
    bcm2835_delay(1);

#ifdef NDEBUG
    key = getkey();
    /* terminate loop on ESC (0x1B) on STDIN */
    if (key == 0x1B)
    {
      break;
    }
#endif      

  } while (key == ERR);


  return (EXIT_SUCCESS);
}

