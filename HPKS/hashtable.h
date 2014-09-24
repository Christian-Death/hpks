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
 * File:   hashtable.h
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 20. August 2013, 19:31
 */

#ifndef HASHTABLE_H
#define	HASHTABLE_H

#ifdef	__cplusplus
extern "C" {
#endif

void init_hashtab();
char* get_hashtab(char* name);
int add_hashtab(char* name,char* desc);
void displaytable();
void cleanup_hashtab();

#ifdef	__cplusplus
}
#endif

#endif	/* HASHTABLE_H */

