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
 * File:   lcd_display.c
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 02. April 2013, 19:04
 */

#include "defines/used_includes.h"


#ifdef USE_LCD

#include "lcd_dispay.h"

uint8_t	View;
uint16_t ViewsAktualisierungsCount;

void InitialView(char *str, uint8_t pos)
{	
  uint8_t i;
  char  a[] ={0x7F, 0};
  
 
	LCD_ClearScreen();
	LCD_SetFont(1);	
  LCD_SetPenColor(1);
	LCD_PrintXY(0,0,"Initialisierung...");
  
  LCD_PrintXY(0, 20, str);
  
  for(i=0; i<pos; i++)
    LCD_PrintXY(0+10*i, 40, a);
  
  LCD_WriteFramebuffer();

}

void StatusView1(void)
{	
  char a[20];
  
  
/*
  (2000x4187x80-MW)/(3600x1000) = …KWh (Was noch in den Puffer geht);

Erklärung hierzu:

2000 = Puffervolumen;
4187 = Ws/(kg*K);  spezifische Wärmekapazität wasser in Wattsekunde/(kg*Kelvin)
80 = Maximaltemperatur des Puffers;
MW = Mittelwertberechnung der 8 Temperaturen;
Heizwert von 1 kg Holz = 3,5 KWh/kg
((2000x4187x80-MW)/(3600x1000))/3,5 = … kg (das ergibt die Holzmenge, die man nachlegen kann) – wird angezeigt am Display;

*/
  
  MEASUREMENT_VIEW_struct * CPUTempMem = cpu_temp_getViewMem();
  MEASUREMENT_VIEW_struct * O2Mem = lambda_getViewMem();
  MEASUREMENT_VIEW_struct * BrennerMem = k_sonde_getViewMem();
  MEASUREMENT_VIEW_struct * PufferMem = ds12b20_getViewMem();
  
  
  
  double KWh = (PUFFERVOLUMEN * SPEZ_WAERMEKAP_WASSER * MAX_PUFFER_TEMP - ds18b20_getAverage()) / (3600 * 1000);
  double kg = KWh / HEIZWERT_HOLZ;
 
	LCD_ClearScreen();
	LCD_SetFont(0);	
  LCD_SetPenColor(1);
	LCD_PrintXY(0,0,"PufferKontrollSystem");
  LCD_DrawLine(0,21,127,21);
  LCD_DrawLine(0,42,127,42);
  
  LCD_DrawLine(63,21,63,63);

  LCD_PrintXY(2,23,"Brenner");
  LCD_PrintXY(2,32, BrennerMem->cur_value);

  LCD_PrintXY(66,23,"Puffer");
  LCD_PrintXY(66,32, PufferMem->cur_value);

  LCD_PrintXY(2,44,"O2");
  LCD_PrintXY(2,53,O2Mem->cur_value);

  LCD_PrintXY(66,44,"nachlegen");
  LCD_PrintfXY(66,53,"%3.1f kg", kg);

}

void StatusView2(void)
{	
  char a[20];
  
  MEASUREMENT_VIEW_struct * AbgasMem = pt100_getViewMem();  
  
	LCD_ClearScreen();
	LCD_SetFont(0);	
  LCD_SetPenColor(1);
	LCD_PrintXY(0,0,"PufferKontrollSystem");
  LCD_DrawLine(0,21,127,21);
  LCD_DrawLine(0,42,127,42);
  
  LCD_DrawLine(63,21,63,63);

  LCD_PrintXY(2,23,"Abgas");
  LCD_PrintXY(2,32, AbgasMem->cur_value);

  LCD_PrintXY(66,23,"Verlust");
  double abgasverlust = get_abgasverlust();
  sprintf(a, "%4u",abgasverlust);
  LCD_PrintXY(66,32, a);


}

void ConfigView(void)
{	
  char a[20];
 
LCD_ClearScreen();
LCD_SetFont(0);	
  LCD_SetPenColor(1);
LCD_PrintXY(0,0,"Konfiguration");
  LCD_DrawLine(0,21,127,21);
  LCD_DrawLine(0,42,127,42);
  
  LCD_DrawLine(63,21,63,63);
  LCD_PrintXY(2,23,"Puffer :");
  LCD_PrintfXY(64,23, "%dl", app_config_get_puffer_volume());
  
  LCD_PrintXY(2,33,"Licht :");
  LCD_PrintfXY(64,33, "%ds", app_config_get_light_on_time());
  	//LCD_SetFont(0);	

}

void DS18B20View(void)
{	
  char a[20];
  int i, count;
  DS18B20_struct *ds18b20_dev;
  
 
LCD_ClearScreen();
LCD_SetFont(0);	
  LCD_SetPenColor(1);
LCD_PrintXY(0,0,"DS18B20");
  LCD_DrawLine(0,21,127,21);
  count =  ds18b20_getDeviceCount() ;


  for(i=0; i<count;i++) {
    ds18b20_dev = ds18b20_getDevice(i);
    if(ds18b20_dev->crc)
      LCD_PrintfXY(0,23+(8*i), "%s: %5.1f",ds18b20_dev->_deviceid, ds18b20_dev->tempGrad); 
    else
      LCD_PrintfXY(0,23+(8*i), "%s: Fehler",ds18b20_dev->_deviceid); 
  }
}

//////////////////////////////////////////////////////////////////////////


void CpuTemperatureView(void)
{
	uint16_t	i,y;
	MEASUREMENT_VIEW_struct * viewMem = cpu_temp_getViewMem();
	LCD_ClearScreen();
	LCD_SetPenColor(1);
	LCD_SetFont(1);
	LCD_PrintXY(40,0,"CPU:");
	LCD_PrintXY(80,0,viewMem->cur_value);
		
	LCD_SetFont(0);
	LCD_PrintXY(0,0, "60-");
	LCD_PrintXY(0,18,"50-");
	LCD_PrintXY(0,37,"40-");
	LCD_PrintXY(0,56,"30-");
	LCD_DrawLine(15,0,15,63);

	for(i=16;i<128;i++)
	{
		y = viewMem->Mem[127-i];
		
		if(y > 290) 
		{
			y = ((y - 290) / 5);
			y = 64 - y;
			LCD_PutPixel(i,y,1);
			LCD_PutPixel(i,y+1,1);
		}
	}
}


///////////////////////////////////////////////////////////////////////


void PufferTemperatureView(void)
{
	uint16_t	i,y;
	MEASUREMENT_VIEW_struct * viewMem = ds12b20_getViewMem();
	LCD_ClearScreen();
	LCD_SetPenColor(1);
	LCD_SetFont(0);
	LCD_PrintXY(30,0,"Puffer:");
	LCD_PrintXY(80,0, viewMem->cur_value);
		
	LCD_SetFont(0);
	
  int max = roundUp(viewMem->max);
  int min  = roundDown(viewMem->min);
  int diff = max-min;
  int span;
  if(!diff) {
    span = 10;
    if(min) min -= 10;
  }else {
    span = diff/3;
  }
  if(span < 10) span = 10;
  
  char dummy[10];
 
    sprintf(dummy,"%4u-",min+(span*3));
    LCD_PrintXY(0,0, dummy);
    
    sprintf(dummy,"%4u-",min+(span*2));
    LCD_PrintXY(0,18,dummy);
    
    sprintf(dummy,"%4u-",min+(span*1));
    LCD_PrintXY(0,37,dummy);
    
    sprintf(dummy,"%4u-",min);
    LCD_PrintXY(0,56,dummy);

  
	LCD_DrawLine(27,0,27,63);
  diff = (span*3);
  
	for(i=27;i<128;i++)
	{
		y = viewMem->Mem[127-i];
    y =  64 - ((double)(y-min)*(double)(64.0/diff));
		LCD_PutPixel(i,y,1);
		LCD_PutPixel(i,y+1,1);
		
	}
}

/////////////////////////////////////////////////////////////////////


void BrennerTemperatureView(void)
{
	uint16_t	i,y;
	MEASUREMENT_VIEW_struct * viewMem = k_sonde_getViewMem();
	LCD_ClearScreen();
	LCD_SetPenColor(1);
	LCD_SetFont(0);
	LCD_PrintXY(30,0,"BRENNER:");
	LCD_PrintXY(80,0,viewMem->cur_value);
		
	LCD_SetFont(0);
	
  int max = roundUp(viewMem->max);
  int min  = roundDown(viewMem->min);
  int diff = max-min;
  int span;
  if(!diff) {
    span = 10;
    if(min) min -= 10;
  }else {
    span = diff/3;
  }
  
  if(span < 10) span = 10;
  
  char dummy[10];
 
    sprintf(dummy,"%4u-",min+(span*3));
    LCD_PrintXY(0,0, dummy);
    
    sprintf(dummy,"%4u-",min+(span*2));
    LCD_PrintXY(0,18,dummy);
    
    sprintf(dummy,"%4u-",min+(span*1));
    LCD_PrintXY(0,37,dummy);
    
    sprintf(dummy,"%4u-",min);
    LCD_PrintXY(0,56,dummy);

  
	LCD_DrawLine(27,0,27,63);
  diff = (span*3);
  
	for(i=27;i<128;i++)
	{
		y = viewMem->Mem[127-i];
		y =  64 - ((double)(y-min)*(double)(64.0/diff));
		LCD_PutPixel(i,y,1);
		LCD_PutPixel(i,y+1,1);
		
	}
}

/////////////////////////////////////////////////////////////////////


void AbgasTemperatureView(void)
{
	uint16_t	i,y;
	MEASUREMENT_VIEW_struct * viewMem = pt100_getViewMem();
	LCD_ClearScreen();
	LCD_SetPenColor(1);
	LCD_SetFont(0);
	LCD_PrintXY(30,0,"ABGAS:");
	LCD_PrintXY(80,0,viewMem->cur_value);
		
	LCD_SetFont(0);
	
  int max = roundUp(viewMem->max);
  int min  = roundDown(viewMem->min);
  int diff = max-min;
  int span;
  if(!diff) {
    span = 10;
    if(min) min -= 10;
  }else {
    span = diff/3;
  }
  
  if(span < 10) span = 10;
  
  char dummy[10];
 
    sprintf(dummy,"%4u-",min+(span*3));
    LCD_PrintXY(0,0, dummy);
    
    sprintf(dummy,"%4u-",min+(span*2));
    LCD_PrintXY(0,18,dummy);
    
    sprintf(dummy,"%4u-",min+(span*1));
    LCD_PrintXY(0,37,dummy);
    
    sprintf(dummy,"%4u-",min);
    LCD_PrintXY(0,56,dummy);

  
	LCD_DrawLine(27,0,27,63);
  diff = (span*3);
  
	for(i=27;i<128;i++)
	{
		y = viewMem->Mem[127-i];
		y =  64 - ((double)(y-min)*(double)(64.0/diff));
		LCD_PutPixel(i,y,1);
		LCD_PutPixel(i,y+1,1);
		
	}
}

///////////////////////////////////////////////////////////////////
void O2_ErrorView(void)
{
	uint16_t	i,y;
  
	LCD_ClearScreen();
	LCD_SetPenColor(1);
	LCD_SetFont(0);
	LCD_PrintXY( 0,0, "Lambda Fehler!");
	
	LCD_SetFont(0);
  
	LCD_PrintXY(0, 0, "W | S | U | U | S | C");
	LCD_PrintXY(0, 9, "D | P | H | L | O | J");
  LCD_DrawLine(0,18,127,18);
  LCD_PrintXY(0,19, "  |   |   |   |   |  ");
  
  if(lambda_get_single_error(WATCHDOG)) LCD_PrintXY(0,19, "X");
  if(lambda_get_single_error(SPI_ERROR)) LCD_PrintXY(24,19, "X");
  if(lambda_get_single_error(UBAT_HIGH)) LCD_PrintXY(48,19, "X");
  if(lambda_get_single_error(UBAT_LOW)) LCD_PrintXY(72,19, "X");
  if(lambda_get_single_error(SONDE_OVERHEAT)) LCD_PrintXY(96,19, "X");
  if(lambda_get_single_error(CJ_ERROR)) LCD_PrintXY(120,19, "X");

      
      
  LCD_PrintXY(0,35, "DIAHG|IA/IP|UN|VM");
  LCD_PrintXY(0,44, "DIAHD|IA/IP|UN|VM");
  LCD_DrawLine(0,53,127,53);
  LCD_PrintXY(0,54, "     |     |  |  ");
   if(lambda_get_single_cjerror(DIAHG_DIAHD_ERROR)) LCD_PrintXY(12,54, "X");                 
   if(lambda_get_single_cjerror(IA_IP_ERROR)) LCD_PrintXY(54,54, "X"); 
   if(lambda_get_single_cjerror(UN_ERROR)) LCD_PrintXY(78,54, "X"); 
   if(lambda_get_single_cjerror(VM_ERROR)) LCD_PrintXY(96,54, "X"); 
  
  

}

void O2_StatusView(void)
{
	uint16_t	i,y;
  
	LCD_ClearScreen();
	LCD_SetPenColor(1);
	LCD_SetFont(0);
	LCD_PrintXY( 0,0, "Lambda Fehler!");
	
	LCD_SetFont(0);
  
	LCD_PrintXY(0, 0, "W | S | U | U | S | C");
	LCD_PrintXY(0, 9, "D | P | H | L | O | J");
  LCD_DrawLine(0,18,127,18);
  LCD_PrintXY(0,19, "  |   |   |   |   |  ");
  
  if(lambda_get_single_error(WATCHDOG)) LCD_PrintXY(0,19, "X");
  if(lambda_get_single_error(SPI_ERROR)) LCD_PrintXY(24,19, "X");
  if(lambda_get_single_error(UBAT_HIGH)) LCD_PrintXY(48,19, "X");
  if(lambda_get_single_error(UBAT_LOW)) LCD_PrintXY(72,19, "X");
  if(lambda_get_single_error(SONDE_OVERHEAT)) LCD_PrintXY(96,19, "X");
  if(lambda_get_single_error(CJ_ERROR)) LCD_PrintXY(120,19, "X");

      
      
  LCD_PrintXY(0,35, "DIAHG|IA/IP|UN|VM");
  LCD_PrintXY(0,44, "DIAHD|IA/IP|UN|VM");
  LCD_DrawLine(0,53,127,53);
  LCD_PrintXY(0,54, "     |     |  |  ");
   if(lambda_get_single_cjerror(DIAHG_DIAHD_ERROR)) LCD_PrintXY(12,54, "X");                 
   if(lambda_get_single_cjerror(IA_IP_ERROR)) LCD_PrintXY(54,54, "X"); 
   if(lambda_get_single_cjerror(UN_ERROR)) LCD_PrintXY(78,54, "X"); 
   if(lambda_get_single_cjerror(VM_ERROR)) LCD_PrintXY(96,54, "X"); 
  
  
}

void O2View(void)
{
	uint16_t	i,y;
  
  if(lambda_fehler()) {
    O2_ErrorView();
    return;
  } 
  else if(!lambda_run()) {
    O2_StatusView();
    return;
  }
  
	MEASUREMENT_VIEW_struct * viewMem = lambda_getViewMem();
	LCD_ClearScreen();
	LCD_SetPenColor(1);
	LCD_SetFont(0);
	LCD_PrintXY(40,0,"O2:");
	LCD_PrintXY(80,0,viewMem->cur_value);
		
	LCD_SetFont(0);
	LCD_PrintXY(0,0, "25-");
	LCD_PrintXY(0,18,"15-");
	LCD_PrintXY(0,37," 5-");
	LCD_PrintXY(0,56," 0-");
	LCD_DrawLine(15,0,15,63);

  
  for(i=16;i<128;i++)
	{
    y = viewMem->Mem[127-i];
    if(y < 25) {
		
      y =  64 - ((double)(y)*(double)(64.0/25));
      LCD_PutPixel(i,y,1);
      LCD_PutPixel(i,y+1,1);
    }
	}

}



static unsigned int last_view_update;

void lcd_display_init(void)
{
  
  LCD_Init();			// Init Display
  SetBacklight(1);	// Turn Backlight on 
  View = 0;


}




void lcd_display_worker(void) {
  
	unsigned int cur_ms =  millis();
  if((cur_ms - last_view_update)<100 && !ButtonPressed) return;
	
  last_view_update = cur_ms;
    

    /* if(BUTTON_PRESSED_UP || BUTTON_PRESSED_DOWN)
     {
       if(BUTTON_PRESSED_UP   && (Contrast < 20))  Contrast++; 
       if(BUTTON_PRESSED_DOWN && (Contrast > 0))   Contrast--; 
       LCD_SetContrast(Contrast); 
     }
     if(BUTTON_PRESSED_CENTER)
     {
       Backlight = (Backlight) ? 0 : 1;	// Toggle Backlight
       SetBacklight(Backlight);			// Write to Hardware
     }	*/	
    
    
     
     if(BUTTON_PRESSED_LEFT  && View) View--;		
     if(BUTTON_PRESSED_RIGHT && (View < 6)) {
       View++;		
     }
     if(     View == 0)	StatusView1();
     else if(View == 1)	StatusView2();
     else if(View == 2)   CpuTemperatureView(); 
     
     else if(View == 3) {PufferTemperatureView();}
     else if(View == 4)  { BrennerTemperatureView(); }
     else if(View == 5)  { AbgasTemperatureView(); }
     
     else if(View == 6)   {O2View();}  
     else if(View == 7)   {DS18B20View();}          
     else if(View == 8)   {ConfigView();} 
 

   

     LCD_WriteFramebuffer();
  
}

#endif