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
 * File:   cpu_temp.h
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 11. Dezember 2012, 21:51
 */

#ifndef CPU_TEMP_H
#define	CPU_TEMP_H

#ifdef	__cplusplus
extern "C" {
#endif

void cpu_temp_init(void);
void cpu_temp_getData();
MEASUREMENT_VIEW_struct * cpu_temp_getViewMem();


#ifdef	__cplusplus
}
#endif

#endif	/* CPU_TEMP_H */

