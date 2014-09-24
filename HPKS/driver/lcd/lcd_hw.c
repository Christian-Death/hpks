#include <defines/used_includes.h>

#ifdef USE_LCD
#include "lcd_hw.h"

uint8_t lcd_tx[1];


static  SPI_CONFIG_struct lcd_spi_config = {
  SPI_MODE_3|SPI_NO_CS,
  0,
  1000000,
  0,
  lcd_tx,
  NULL,
  1};
//--------------------------------------------------------------------------------------------------
// Name:    	SpiPutc
// Function:  	Emulate SPI on GPIO (Bitbanging)
//            
// Parameter: 	Databyte to send
// Return:      -
//--------------------------------------------------------------------------------------------------
/*void SpiPutc1(unsigned char d)
{
	int i,n;
	
	for(i=0;i<8;i++)
	{
		if(d & 0x80)	bcm2835_gpio_set(PIN_LCD_MOSI);		// MOSI = 1
			else		bcm2835_gpio_clr(PIN_LCD_MOSI);		// MOSI = 0
		d <<= 1;
		
		for(n=0;n<4;n++) bcm2835_gpio_clr(PIN_LCD_SCLK); 	// CLK = 0
		for(n=0;n<4;n++) bcm2835_gpio_set(PIN_LCD_SCLK);	// CLK = 1
	}
}*/

void SpiPutc(unsigned char d)
{
  lcd_tx[0] = d;
  LCD_CS_CLR;
	spi_transfer(&lcd_spi_config, d);
	LCD_CS_SET;

}
//--------------------------------------------------------------------------------------------------
// Name:    	SetBacklight
// Function:  	Hintergrundbeleuchtung
//            
// Parameter: 	0=Off 1=On
// Return:      -
//--------------------------------------------------------------------------------------------------
void SetBacklight(uint8_t light)
{
	if(light)	LCD_BACKLIGHT_ON;
		else	LCD_BACKLIGHT_OFF;
}

#endif