
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
 * File:   app_config.h
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 17. Dezember 2012, 14:24
 */

#ifndef APP_CONFIG_H
#define	APP_CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif


typedef enum USED_ABGAS_SONDE_enum{
    K_SONDE,
            PT100,
}USED_ABGAS_SONDE_e;    
    
int app_config_read();
int app_config_save();
void app_config_save_endstate(time_t endtime, double endleistung) ;
int app_config_read_ds18b20();
int app_config_write_ds18b20();

double app_config_get_endleistung();
bool app_config_is_simulate_mode();
bool app_config_get_use_server();
char* app_config_get_www_path();
uint app_config_get_server_port();
bool app_config_get_use_abgas_temp();
USED_ABGAS_SONDE_e app_config_get_used_abgas_sonde();
bool app_config_get_use_lambdasonde();
bool app_config_get_use_lambdacheck();

bool app_config_get_use_ds18b20();
bool app_config_get_use_k_sonde();
bool app_config_get_use_ruettler();
bool app_config_get_use_window();

bool app_config_get_pufferpos( DS18B20_struct *ds18b20);
bool app_config_set_pufferpos(const char* deviceid, int puf_id, int puf_pos);
void app_config_refresh_pufferpos();

long app_config_get_max_volume();
double app_config_get_heizwert_holz();
int app_config_set_heizwert_holz(double value);
int app_config_get_spez_wasser();
double app_config_getd_spez_wasser();
int app_config_get_max_puffer_temp();
double app_config_getd_max_puffer_temp();
int app_config_set_max_puffer_temp(int value);
int app_config_get_min_puffer_temp();
double app_config_getd_min_puffer_temp();
int app_config_set_min_puffer_temp(int value);
int app_config_get_puffer_count();
int app_config_set_puffer_count(int value);
int app_config_get_puffer_volume(int puffer);
int app_config_set_puffer_volume(int puffer, int volume);
int app_config_get_light_on_time();
int app_config_set_light_on_time(int time); 

int app_config_get_abgas_end_temp();
int app_config_set_abgas_end_temp(int value);

double app_config_get_ruettler_activ();
int app_config_set_ruettler_activ(double value);
double app_config_get_ruettler_deactiv();
int app_config_set_ruettler_deactiv(double value);
int app_config_get_ruettler_on_time();
int app_config_set_ruettler_on_time(int value);
int app_config_get_ruettler_off_time();
int app_config_set_ruettler_off_time(int value) ;
int app_config_get_ruettler_loop();
int app_config_set_ruettler_loop(int value);

#ifdef	__cplusplus
}
#endif

#endif	/* APP_CONFIG_H */

