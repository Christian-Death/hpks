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
 * File:   utils.h
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 27. November 2012, 20:11
 */

#ifndef UTILS_H
#define	UTILS_H

#ifdef	__cplusplus
extern "C" {
#endif


//=== Global function prototypes ===================================================================
int getkey();
int GetRaspberryHwRevision(void);
const char *byte_to_binary(int x);
void SleepMs(uint32_t ms);
int roundUp(double numToRound);
int roundDown(double numToRound);
int countchr(const char *str, char ch);
void millis_init();
unsigned int millis (void);
int get_ipaddr();
void dbg_printf(char *str,...);

void removeAll(char *p, char c);
#ifdef	__cplusplus
}
#endif

#endif	/* UTILS_H */

