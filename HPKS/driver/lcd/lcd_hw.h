/* 
 * File:   lcd_hw.h
 * Author: The Death
 *
 * Created on 27. November 2012, 20:36
 */

#ifndef LCD_HW_H
#define	LCD_HW_H

#ifdef	__cplusplus
extern "C" {
#endif

  //=== Preprocessing directives (#define) ===========================================================


#ifdef USE_PORTEXPANDER
#define	LCD_RST_CLR 		mcp23s17_clr_pin(PORT_B, PIN_LCD_RST) //bcm2835_gpio_clr(PIN_LCD_RST) 
#define	LCD_RST_SET			mcp23s17_set_pin(PORT_B, PIN_LCD_RST) 

#define	LCD_CS_CLR 			bcm2835_gpio_clr(PIN_LCD_CS)
#define	LCD_CS_SET			bcm2835_gpio_set(PIN_LCD_CS)

#define	LCD_RS_CLR 			mcp23s17_clr_pin(PORT_B, PIN_LCD_RS)
#define	LCD_RS_SET			mcp23s17_set_pin(PORT_B, PIN_LCD_RS)
  
#define	LCD_BACKLIGHT_ON mcp23s17_set_pin(PORT_B, PIN_LCD_BACKLIGHT)
#define	LCD_BACKLIGHT_OFF mcp23s17_clr_pin(PORT_B, PIN_LCD_BACKLIGHT)
    
#else
#define	LCD_RST_CLR 		bcm2835_gpio_clr(PIN_LCD_RST) 
#define	LCD_RST_SET			bcm2835_gpio_set(PIN_LCD_RST) 

#define	LCD_CS_CLR 			bcm2835_gpio_clr(PIN_LCD_CS)
#define	LCD_CS_SET			bcm2835_gpio_set(PIN_LCD_CS)

#define	LCD_RS_CLR 			bcm2835_gpio_clr(PIN_LCD_RS)
#define	LCD_RS_SET			bcm2835_gpio_set(PIN_LCD_RS)  
  
#define	LCD_BACKLIGHT_ON bcm2835_gpio_set(PIN_LCD_BACKLIGHT)
#define	LCD_BACKLIGHT_OFF bcm2835_gpio_clr(PIN_LCD_BACKLIGHT)

#endif
#define	LCD_SPI_PUTC(__d)	SpiPutc(__d)
#define	LCD_SPI_WAIT_BUSY
  
 
void SpiPutc(unsigned char d);
void SetBacklight(uint8_t light);


#ifdef	__cplusplus
}
#endif

#endif	/* LCD_HW_H */

