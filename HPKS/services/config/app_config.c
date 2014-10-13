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
 * File:   app_config.c
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 17. Dezember 2012, 14:25
 */

#include <stdio.h>
#include <stdlib.h>
#include "defines/used_includes.h"

#include "../../utils/minini/minIni.h"

typedef struct {
  char _deviceid[DS18B20_ID_LENGTH];
  int puffer;
  int pos;
} DS18B20_CONFIG_INI_struct;

typedef struct {
  double activ;
  double deactiv;
  int on_time;
  int off_time;
  int loop;
} RUETTLER_CONFIG_INI_struct;

typedef
struct {
  int simulate;
  int use_window;
  int use_lcd;
  int use_lambdacheck;
  int use_lambdasonde;
  int use_ksonde;
  int use_ds18b20;
  int use_abgas_temp;
  USED_ABGAS_SONDE_e use_abgas_sonde;
  int use_ruettler;
  int use_webserver;
  char www_path[FILENAME_MAX];
  unsigned int port;

  int puffer_count;
  int *puffer_volume;
  long max_volume;
  int ds18b20_count;
  DS18B20_CONFIG_INI_struct *ds18b20;
  RUETTLER_CONFIG_INI_struct shaker;
  int spez_wasser;
  int hb_time;
  int max_puffer_temp;
  int min_puffer_temp;
  double heizwert_holz;
  int abgas_end_temp;
  time_t endtime;
  double endleistung;
  
} CONFIG_DATA_struct;



CONFIG_DATA_struct config_data;
static const char ini_session_general[] = "general";
static const char ini_session_puffer[] = "puffer";
static const char ini_key_volume[] = "volume";

static const char ini_session_ds18b20[] = "ds18b20";
static const char ini_key_ds18b20_id[] = "id";
static const char ini_key_ds18b20_puffer[] = "puffer";
static const char ini_key_ds18b20_pos[] = "pos";

static const char ini_session_shaker[] = "ruettler";
static const char ini_session_regelung[] = "regelung";

static const char ini_key_value_pt100[] = "PT100";
static const char ini_key_value_k_sonde[] = "K-Sonde";


static char config_inifile[PATH_MAX];

/*!***************************************************************************
 * 
 * 
 *****************************************************************************/
int app_config_read_puffers()
{
  int k;
  long n, m;
  char key[40];
  char dummy[20];

  n = ini_getl(ini_session_puffer, "anzahl", 1, config_inifile);
  if (n > MAX_PUFFER_COUNT)
    n = MAX_PUFFER_COUNT;
  config_data.puffer_count = n;

  // Speicher reservieren
  config_data.puffer_volume = (int *)calloc(config_data.puffer_count, sizeof (int));
  if (config_data.puffer_volume == NULL)
  {
    dbg_printf("Kein freier Speicher vorhanden.");
    return EXIT_FAILURE;
  }
  for (k = 0; k < config_data.puffer_count; k++)
  {

    sprintf(dummy, "%s%u", ini_key_volume, k);
    n = ini_getl(ini_session_puffer, dummy, DEF_PUFFERVOLUMEN, config_inifile);
    if (n < MIN_PUFFERVOLUMEN && n > MAX_PUFFERVOLUMEN)
    {

      n = DEF_PUFFERVOLUMEN;
    }

    config_data.puffer_volume[k] = n;
    config_data.max_volume += n;
  }

  /*
    for (k = 0; ini_getkey(ini_session_puffer, k, key, sizeof key, config_inifile) > 0; k++) {
      printf("\t%s\n", key);
      if (!strncmp(key, ini_key_volume, strlen(ini_key_volume))) {
        m = atoi(key + strlen(ini_key_volume));
        n = ini_getl(ini_session_puffer, key, DEF_PUFFERVOLUMEN, config_inifile);
        if (n < MIN_PUFFERVOLUMEN && n > MAX_PUFFERVOLUMEN) {
          n = DEF_PUFFERVOLUMEN;
        }
        if (m <= config_data.puffer_count && m > 0) {
          config_data.puffer_volume[m - 1] = n;
          config_data.max_volume += n;
        }
      }
    }
   */
}

int app_config_write_puffers()
{
  int k;
  int n;
  char dummy[20];

  n = ini_putl(ini_session_puffer, "anzahl", config_data.puffer_count, config_inifile);

  // Speicher reservieren
  for (k = 0; k < config_data.puffer_count; k++)
  {
    sprintf(dummy, "%s%u", ini_key_volume, k);
    n = ini_putl(ini_session_puffer, dummy, config_data.puffer_volume[k], config_inifile);

  }
}

/*!***************************************************************************
 * 
 * 
 *****************************************************************************/
int app_config_read_ds18b20()
{
  int k;
  long n, m;
  char key[40];
  char sId[20];
  char dummy[20];
  int nPuffer, nPos;

  if (config_data.ds18b20 != NULL)
  {
    free(config_data.ds18b20);
  }

  config_data.ds18b20_count = ds18b20_getDeviceCount();
  // Speicher reservieren
  config_data.ds18b20 = (DS18B20_CONFIG_INI_struct *)calloc(config_data.ds18b20_count, sizeof (DS18B20_CONFIG_INI_struct));
  if (config_data.ds18b20 == NULL)
  {
    dbg_printf("Kein freier Speicher vorhanden.");
    return EXIT_FAILURE;
  }

  DS18B20_struct *ds18b20_dev;
  for (k = 0; k < config_data.ds18b20_count; k++)
  {
    ds18b20_dev = ds18b20_getDevice(k);
    sprintf(config_data.ds18b20[k]._deviceid, "%s", ds18b20_dev->_deviceid);
  }


  for (k = 0; ini_getkey(ini_session_ds18b20, k, key, sizeof key, config_inifile) > 0; k++)
  {
    printf("\t%s\n", key);
    if (!strncmp(key, ini_key_ds18b20_id, strlen(ini_key_ds18b20_id)))
    {
      m = atoi(key + strlen(ini_key_ds18b20_id));
      n = ini_gets(ini_session_ds18b20, key, "", sId, sizearray(sId), config_inifile);

      sprintf(dummy, "%s%u", ini_key_ds18b20_puffer, m);
      nPuffer = ini_getl(ini_session_ds18b20, dummy, 0, config_inifile);
      sprintf(dummy, "%s%u", ini_key_ds18b20_pos, m);
      nPos = ini_getl(ini_session_ds18b20, dummy, 0, config_inifile);

      app_config_set_pufferpos(sId, nPuffer, nPos);
    }
  }

  for (k = 0; k < config_data.ds18b20_count; k++)
  {
    ds18b20_dev = ds18b20_getDevicewithID(config_data.ds18b20[k]._deviceid);
    app_config_get_pufferpos(ds18b20_dev);
  }

}

int app_config_write_ds18b20()
{
  int k;
  int n;
  char dummy[20];

  // Speicher reservieren
  for (k = 0; k < config_data.ds18b20_count; k++)
  {
    sprintf(dummy, "%s%u", ini_key_ds18b20_id, k);
    n = ini_puts(ini_session_ds18b20, dummy, config_data.ds18b20[k]._deviceid, config_inifile);
    sprintf(dummy, "%s%u", ini_key_ds18b20_puffer, k);
    n = ini_putl(ini_session_ds18b20, dummy, config_data.ds18b20[k].puffer, config_inifile);
    sprintf(dummy, "%s%u", ini_key_ds18b20_pos, k);
    n = ini_putl(ini_session_ds18b20, dummy, config_data.ds18b20[k].pos, config_inifile);
  }
}

/*!***************************************************************************
 * 
 * 
 *****************************************************************************/
int app_config_read_shaker()
{
  long n;
  INI_REAL f;

  f = ini_getf(ini_session_shaker, "aktiv", DEF_SHAKER_AKTIVE, config_inifile);
  config_data.shaker.activ = f;

  f = ini_getf(ini_session_shaker, "deaktiv", DEF_SHAKING, config_inifile);
  config_data.shaker.deactiv = f;
  n = ini_getl(ini_session_shaker, "zeit_an", DEF_SHAKER_RUN, config_inifile);
  config_data.shaker.on_time = n;
  n = ini_getl(ini_session_shaker, "zeit_aus", DEF_SHAKER_PAUSE, config_inifile);
  config_data.shaker.off_time = n;
  n = ini_getl(ini_session_shaker, "anzahl", DEF_SHAKING_COUNT, config_inifile);
  config_data.shaker.loop = n;

}

int app_config_write_shaker()
{
  long n;

  n = ini_putf(ini_session_shaker, "aktiv", config_data.shaker.activ, config_inifile);

  n = ini_putf(ini_session_shaker, "deaktiv", config_data.shaker.deactiv, config_inifile);

  n = ini_putl(ini_session_shaker, "zeit_an", config_data.shaker.on_time, config_inifile);

  n = ini_putl(ini_session_shaker, "zeit_aus", config_data.shaker.off_time, config_inifile);

  n = ini_putl(ini_session_shaker, "anzahl", config_data.shaker.loop, config_inifile);


}

/*!***************************************************************************
 * 
 * 
 *****************************************************************************/
int app_config_read()
{
  char str[100];
  long n;
  INI_REAL f;
  struct stat buf;



  snprintf(config_inifile, PATH_MAX, "%s/%s", app_fullpath, CONFIG_FILE_NAME);
  if ((0 == stat(config_inifile, &buf)) && (S_ISREG(buf.st_mode)))
  {
    dbg_printf("Konfigfile %s gefunden\n", config_inifile);
  }
  else
  {
    dbg_printf("Konfigfile %s nicht gefunden\n", config_inifile);
  }

  //n = ini_gets("general", "string", "dummy", str, sizearray(str), config_inifile);

  app_config_read_puffers();
  //  app_config_read_ds18b20();
  app_config_read_shaker();

  n = ini_getbool(ini_session_general, "simulate", 0, config_inifile);
  config_data.simulate = n;
  if (config_data.simulate)
    simulate_init();

  n = ini_getl(ini_session_regelung, "abgas_end_temp", DEF_ABGAS_END_TEMP, config_inifile);
  config_data.abgas_end_temp = n;

  n = ini_getl(ini_session_general, "spez_wasser", DEF_SPEZ_WAERMEKAP_WASSER, config_inifile);
  config_data.spez_wasser = n;
  n = ini_getl(ini_session_general, "max_puffer_temp", DEF_MAX_PUFFER_TEMP, config_inifile);
  config_data.max_puffer_temp = n;
  n = ini_getl(ini_session_general, "min_puffer_temp", DEF_MIN_PUFFER_TEMP, config_inifile);
  config_data.min_puffer_temp = n;
  f = ini_getf(ini_session_general, "heizwert_holz", DEF_HEIZWERT_HOLZ, config_inifile);
  config_data.heizwert_holz = f;
  n = ini_getbool(ini_session_general, "USE_LCD", 0, config_inifile);
  config_data.use_lcd = n;

  n = ini_getbool(ini_session_general, "", 0, config_inifile);

  n = ini_getbool(ini_session_general, "USE_ABGAS_TEMP", 1, config_inifile);
  config_data.use_abgas_temp = n;

  config_data.use_abgas_sonde = K_SONDE;
  n = ini_gets(ini_session_general, "USE_ABGAS_SONDE", "", str, sizearray(str), config_inifile);
  if (!strncmp(str, ini_key_value_pt100, strlen(ini_key_value_pt100)))
  {
    config_data.use_abgas_sonde = PT100;
  }

  n = ini_getbool(ini_session_general, "USE_LAMBDA_CHECK", 1, config_inifile);
  config_data.use_lambdacheck = n;
  n = ini_getbool(ini_session_general, "USE_LAMBDA_SONDE", 0, config_inifile);
  config_data.use_lambdasonde = n;
  n = ini_getbool(ini_session_general, "USE_K-SONDE", 1, config_inifile);
  config_data.use_ksonde = n;
  n = ini_getbool(ini_session_general, "USE_DS18B20", 1, config_inifile);
  config_data.use_ds18b20 = n;
  n = ini_getbool(ini_session_general, "USE_RUETTLER", 0, config_inifile);
  config_data.use_ruettler = n;
  n = ini_getbool(ini_session_general, "USE_WINDOW", 0, config_inifile);
  config_data.use_window = n;
  n = ini_getbool(ini_session_general, "USE_WEBSERVER", 1, config_inifile);
  config_data.use_webserver = n;

  n = ini_gets(ini_session_general, "WWW_PATH", DEF_WWW_PATH, config_data.www_path, sizearray(config_data.www_path), config_inifile);
 
  n = ini_getl(ini_session_general, "SERVER_PORT", DEF_SERVER_PORT, config_inifile);
  config_data.port = n;

  
  
  config_data.hb_time = 2;

  return (EXIT_SUCCESS);
}

/*!***************************************************************************
 * 
 * 
 *****************************************************************************/
int app_config_save()
{
  int n;
  n = ini_putl(ini_session_general, "spez_wasser", config_data.spez_wasser, config_inifile);

  n = ini_putl(ini_session_general, "max_puffer_temp", config_data.max_puffer_temp, config_inifile);
  n = ini_putl(ini_session_general, "min_puffer_temp", config_data.min_puffer_temp, config_inifile);
  n = ini_putf(ini_session_general, "heizwert_holz", config_data.heizwert_holz, config_inifile);

  n = ini_putl(ini_session_general, "USE_LCD", config_data.use_lcd, config_inifile);

  n = ini_putl(ini_session_general, "USE_ABGAS_TEMP", config_data.use_abgas_temp, config_inifile);
  
  n = ini_puts(ini_session_general, "USE_ABGAS_SONDE", (config_data.use_abgas_sonde==PT100)?ini_key_value_pt100:ini_key_value_k_sonde , config_inifile);
    
  
  n = ini_putl(ini_session_general, "USE_K-SONDE", config_data.use_ksonde, config_inifile);

  n = ini_putl(ini_session_general, "USE_LAMBDA_CHECK", config_data.use_lambdacheck, config_inifile);

  n = ini_putl(ini_session_general, "USE_LAMBDA_SONDE", config_data.use_lambdasonde, config_inifile);


  n = ini_putl(ini_session_general, "USE_K-SONDE", config_data.use_ksonde, config_inifile);

  n = ini_putl(ini_session_general, "USE_DS18B20", config_data.use_ds18b20, config_inifile);

  n = ini_putl(ini_session_general, "USE_RUETTLER", config_data.use_ruettler, config_inifile);

  n = ini_putl(ini_session_general, "USE_WINDOW", config_data.use_window, config_inifile);

  n = ini_putl(ini_session_general, "USE_WEBSERVER", config_data.use_webserver, config_inifile);

  n = ini_putl(ini_session_general, "SERVER_PORT", config_data.port, config_inifile);

  app_config_write_puffers();
  app_config_write_shaker();
}

char* app_config_get_www_path()
{
  return config_data.www_path;
}

void app_config_save_endstate(time_t endtime, double endleistung)
{
  config_data.endtime = endtime;
  config_data.endleistung = endleistung;
}

double app_config_get_endleistung()
{
  return config_data.endleistung;
}

bool app_config_set_pufferpos(const char* deviceid, int puf_id, int puf_pos)
{
  for (int i = 0; i < config_data.ds18b20_count; i++)
  {
    if (!strcmp(deviceid, config_data.ds18b20[i]._deviceid))
    {
      config_data.ds18b20[i].puffer = puf_id;
      config_data.ds18b20[i].pos = puf_pos;
      return true;
    }
  }
  return true;
}

void app_config_refresh_pufferpos()
{
  DS18B20_struct *ds18b20_dev;
  for (int k = 0; k < config_data.ds18b20_count; k++)
  {
    ds18b20_dev = ds18b20_getDevicewithID(config_data.ds18b20[k]._deviceid);
    app_config_get_pufferpos(ds18b20_dev);
  }
}

bool app_config_get_pufferpos(DS18B20_struct *ds18b20)
{
  for (int i = 0; i < config_data.ds18b20_count; i++)
  {
    if (!strcmp(ds18b20->_deviceid, config_data.ds18b20[i]._deviceid))
    {
      ds18b20->puf_id = config_data.ds18b20[i].puffer;
      ds18b20->puf_pos = config_data.ds18b20[i].pos;
      return true;
    }
  }

  return true;
}

bool app_config_is_simulate_mode()
{
  return config_data.simulate;
}

/*!***************************************************************************
 * 
 * 
 *****************************************************************************/
bool app_config_get_use_server()
{
  return config_data.use_webserver;
}

uint app_config_get_server_port()
{
  return config_data.port;
}

bool app_config_get_use_abgas_temp()
{
  return config_data.use_abgas_temp;
}

USED_ABGAS_SONDE_e app_config_get_used_abgas_sonde()
{
  return config_data.use_abgas_sonde;
}

bool app_config_get_use_lambdasonde()
{
  return config_data.use_lambdasonde;
}

bool app_config_get_use_lambdacheck()
{
  return config_data.use_lambdacheck;
}

bool app_config_get_use_ds18b20()
{
  return config_data.use_ds18b20;
}

bool app_config_get_use_k_sonde()
{
  return config_data.use_ksonde;
}

bool app_config_get_use_ruettler()
{
  return config_data.use_ruettler;
}

bool app_config_get_use_window()
{
  return config_data.use_window;
}

double app_config_get_ruettler_activ()
{
  return config_data.shaker.activ;
}

int app_config_set_ruettler_activ(double value)
{
  config_data.shaker.activ = value;
}

double app_config_get_ruettler_deactiv()
{
  return config_data.shaker.deactiv;
}

int app_config_set_ruettler_deactiv(double value)
{
  config_data.shaker.deactiv = value;
}

/*!
 * time in sek.
 * */
int app_config_get_ruettler_on_time()
{
  return config_data.shaker.on_time;
}

int app_config_set_ruettler_on_time(int value)
{
  config_data.shaker.on_time = value;
}

/*!
 * time in sek.
 * */
int app_config_get_ruettler_off_time()
{
  return config_data.shaker.off_time;
}

int app_config_set_ruettler_off_time(int value)
{
  config_data.shaker.off_time = value;
}

int app_config_get_ruettler_loop()
{
  return config_data.shaker.loop;
}

int app_config_set_ruettler_loop(int value)
{
  config_data.shaker.loop = value;
}

int puffer_volume;

long app_config_get_max_volume()
{
  return config_data.max_volume;
}

double app_config_get_heizwert_holz()
{
  return config_data.heizwert_holz;
}

int app_config_set_heizwert_holz(double value)
{
  config_data.heizwert_holz = value;
}

int app_config_get_spez_wasser()
{
  return config_data.spez_wasser;
}

double app_config_getd_spez_wasser()
{
  return (double)config_data.spez_wasser / 1000.0;
}

int app_config_get_abgas_end_temp()
{
  return config_data.abgas_end_temp;
}

int app_config_set_abgas_end_temp(int value)
{
  config_data.abgas_end_temp = value;
}

int app_config_get_max_puffer_temp()
{
  return config_data.max_puffer_temp;
}

double app_config_getd_max_puffer_temp()
{
  return (double)config_data.max_puffer_temp;
}

int app_config_set_max_puffer_temp(int value)
{
  config_data.max_puffer_temp = value;
}

int app_config_get_min_puffer_temp()
{
  return config_data.min_puffer_temp;
}

double app_config_getd_min_puffer_temp()
{
  return (double)config_data.min_puffer_temp;
}

int app_config_set_min_puffer_temp(int value)
{
  config_data.min_puffer_temp = value;
}

int app_config_get_puffer_count()
{
  return config_data.puffer_count;
}

int app_config_set_puffer_count(int value)
{
  config_data.puffer_count = value;
  if (!config_data.puffer_volume)
    free(config_data.puffer_volume);

  config_data.puffer_volume = (int *)calloc(config_data.puffer_count, sizeof (int));
  if (!config_data.puffer_volume)
  {
    dbg_printf("Kein freier Speicher vorhanden.");
    return EXIT_FAILURE;
  }
}

int app_config_get_puffer_volume(int puffer)
{
  return config_data.puffer_volume[puffer];
}

int app_config_set_puffer_volume(int puffer, int volume)
{

  if (puffer < 0 && puffer >= config_data.puffer_count)
    return -1;
  if (volume < 1 && volume > 40000)
    return -1;

  config_data.puffer_volume[puffer] = volume;
  return config_data.puffer_volume[puffer];
}

int app_config_get_light_on_time()
{
  return config_data.hb_time;
}

int app_config_set_light_on_time(int time)
{
  if (time < 0 && time > 10000)
    return -1;

  config_data.hb_time = time;
  return config_data.hb_time;
}


