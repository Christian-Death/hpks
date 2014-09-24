
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
 * File:   buttons.h
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 4. Dezember 2012, 20:46
 */

#ifndef BUTTONS_H
#define	BUTTONS_H

#ifdef	__cplusplus
extern "C" {
#endif

//=== Preprocessing directives (#define) ===========================================================

#define		BUTTON_UP				(Button & 0x01)
#define		BUTTON_LEFT				(Button & 0x02)
#define		BUTTON_CENTER			(Button & 0x04)
#define		BUTTON_RIGHT			(Button & 0x08)
#define		BUTTON_DOWN				(Button & 0x10)

#define		BUTTON_PRESSED_UP		(ButtonPressed & 0x01)
#define		BUTTON_PRESSED_LEFT		(ButtonPressed & 0x08)
#define		BUTTON_PRESSED_CENTER	(ButtonPressed & 0x04)
#define		BUTTON_PRESSED_RIGHT	(ButtonPressed & 0x02)
#define		BUTTON_PRESSED_DOWN		(ButtonPressed & 0x10)

//=== Type definitions (typedef) ===================================================================

//=== Global constants (extern) ====================================================================

//=== Global variables (extern) ====================================================================

extern uint8_t	Button;
extern uint8_t	ButtonPressed; 
  
void UpdateButtons(void);


#ifdef	__cplusplus
}
#endif

#endif	/* BUTTONS_H */

