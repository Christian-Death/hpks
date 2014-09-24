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
 * File:   buttons.c
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 2. Dezember 2012, 11:52
 */


#include "../../defines/used_includes.h"

#ifdef USE_BUTTONS

#include "buttons.h"

//=== Global variables =============================================================================

uint8_t	Button,ButtonPressed;

//=== Local constants  =============================================================================

//=== Local variables ==============================================================================

uint8_t	PinButton[5];
uint8_t	ButtonMem;


//--------------------------------------------------------------------------------------------------
// Name:      UpdateButtons
// Function:  Read button states and save them to the variable "Button" and "ButtonPressed"
//            
// Parameter: 
// Return:    
//--------------------------------------------------------------------------------------------------
static unsigned int last_ms;
void UpdateButtons(void)
{
	ButtonMem = Button;		// Save last State
	
  unsigned int cur_ms =  millis();

  
	Button = 0;
  uint8_t btn = mcp23s17_read(PORT_B);
  
	if(!(btn & (0x01<<0))) Button |= (1<<0);
	if(!(btn & (0x01<<1))) Button |= (1<<1);
	if(!(btn & (0x01<<2))) Button |= (1<<2);
	if(!(btn & (0x01<<3))) Button |= (1<<3);
	if(!(btn & (0x01<<4))) Button |= (1<<4);
	
  printf("Btn: %u - %u %u %u\r\n", cur_ms - last_ms, btn, ButtonMem, Button);	
  last_ms = cur_ms;
  
	ButtonPressed = (Button ^ ButtonMem) & Button;			// Set by Pressing a Button
}

#endif