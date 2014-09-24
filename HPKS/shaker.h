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
 * File:   shaker.h
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 28. August 2013, 21:19
 */

#ifndef SHAKER_H
#define	SHAKER_H

#ifdef	__cplusplus
extern "C" {
#endif
  
   
struct shake_info {
  time_t begin;
  time_t dauer;
  uint8_t count;
  struct shake_info *next;
};
  
void init_shaker();
void shaker_work();
void run_shaker(time_t, time_t);
void stop_shaker(time_t);
bool get_shaker_state();
int get_current_shaker_count();
int get_shaker_shake_count();
int get_shaker_run_count();
void reset_shaker();
struct shake_info* get_shake_info(struct shake_info* sinfo);

#ifdef	__cplusplus
}
#endif

#endif	/* SHAKER_H */

