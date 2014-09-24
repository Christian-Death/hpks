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
 * File:   simulate.h
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 28. November 2013, 20:01
 */

#ifndef SIMULATE_H
#define	SIMULATE_H

#ifdef	__cplusplus
extern "C" {
#endif

int simulate_init() ;
int simulate_get_lambda();
int simulate_get_pt100();
int simulate_get_k_sonde_abgas();
int simulate_get_k_sonde_brenner();

#ifdef	__cplusplus
}
#endif

#endif	/* SIMULATE_H */

