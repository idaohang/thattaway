/***
Copyright (C) 2006 Peter Fleury, 2012 David DiPaola

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
***/

/****************************************************************************
 Title	:   HD44780U LCD library
 Authors:   Peter Fleury <pfleury@gmx.ch>  http://jump.to/fleury
            David DiPaola <dsd3275@cs.rit.edu>
 File:	    $Id: lcd.c,v 1.21 2012/01/06 03:59:47 david Exp david $
 Software:  AVR-GCC 4.3.5, AVR-LIBC 1.6.8
 Target:    any AVR device

 DESCRIPTION
       Basic routines for interfacing a HD44780U-based text lcd display

       Originally based on Volker Oth's lcd library,
       changed lcd_init(), added additional constants for lcd_command(),
       added 4-bit I/O mode, improved and optimized code.

 USAGE
       See the C include lcd.h file for a description of each function

*****************************************************************************/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "lcd.h"

/*
** constants/macros
*/

#define lcd_e_delay()   _delay_us(1);
#define lcd_e_high()    LCD_PORT  |=  _BV(LCD_E_PIN);
#define lcd_e_low()     LCD_PORT  &= ~_BV(LCD_E_PIN);
#define lcd_e_toggle()  toggle_e()
#define lcd_rw_high()   LCD_PORT |=  _BV(LCD_RW_PIN)
#define lcd_rw_low()    LCD_PORT &= ~_BV(LCD_RW_PIN)
#define lcd_rs_high()   LCD_PORT |=  _BV(LCD_RS_PIN)
#define lcd_rs_low()    LCD_PORT &= ~_BV(LCD_RS_PIN)
#define LCD_DATA_MASK   0x0F

#if LCD_LINES==1
  #define LCD_FUNCTION_DEFAULT    LCD_FUNCTION_4BIT_1LINE
#else
  #define LCD_FUNCTION_DEFAULT    LCD_FUNCTION_4BIT_2LINES
#endif

/*
** local functions
*/

/*************************************************************************
Toggle Enable Pin to initiate write
Input:    none
Returns:  none
*************************************************************************/
static void toggle_e(void)
{
    lcd_e_high();
    lcd_e_delay();
    lcd_e_low();
}

/*************************************************************************
Write a value to the 4-bit data port
Input:    data   data to write to LCD (only lower 4 bits used)
Returns:  none
*************************************************************************/
static void lcd_set_databus(uint8_t data){
  LCD_PORT &= ~( LCD_DATA_MASK << LCD_DATA0_PIN );
  LCD_PORT |= ( (data&LCD_DATA_MASK) << LCD_DATA0_PIN );
}

/*************************************************************************
Low-level function to write byte to LCD controller
Input:    data   byte to write to LCD
          rs     1: write data
                 0: write instruction
Returns:  none
*************************************************************************/
static void lcd_write(uint8_t data, uint8_t rs)
{
  if (rs) { //write data        (RS=1, RW=0)
    lcd_rs_high();
  } else {  //write instruction (RS=0, RW=0)
    lcd_rs_low();
  }
  lcd_rw_low();

  //configure data pins as output
  LCD_DDR |= (LCD_DATA_MASK << LCD_DATA0_PIN);

  //output high nibble first
  lcd_set_databus(data>>4);
  lcd_e_toggle();

  //output low nibble
  lcd_set_databus(data);
  lcd_e_toggle();

  //all data pins high (inactive)
  lcd_set_databus(0xFF);
}

/*************************************************************************
Write a value to the 4-bit data port
Input:    none
Returns:  halfbyte read from LCD controller (only lower 4 bits used)
*************************************************************************/
static uint8_t lcd_read_databus(void){
  uint8_t result = 0x00;

  result |= ( (LCD_PIN>>LCD_DATA0_PIN)&LCD_DATA_MASK );

  return result;
}

/*************************************************************************
Low-level function to read byte from LCD controller
Input:    rs     1: read data
                 0: read busy flag / address counter
Returns:  byte read from LCD controller
*************************************************************************/
static uint8_t lcd_read(uint8_t rs)
{
  uint8_t data = 0x00;

  if (rs)
      lcd_rs_high(); //RS=1: read data
  else
      lcd_rs_low();  //RS=0: read busy flag
  lcd_rw_high();     //RW=1  read mode

  //configure data pins as input
  LCD_DDR &= ~(LCD_DATA_MASK << LCD_DATA0_PIN);

  //read high nibble first
  lcd_e_high();
  lcd_e_delay();
  data |= ( lcd_read_databus() << 4 );
  lcd_e_low();

  lcd_e_delay(); //Enable 1us low

  //read low nibble
  lcd_e_high();
  lcd_e_delay();
  data |= ( lcd_read_databus() );
  lcd_e_low();

  return data;
}

/*************************************************************************
loops while lcd is busy
Input:    none
Returns:  address counter
*************************************************************************/
static uint8_t lcd_waitbusy(void)
{
    register uint8_t c;

    //wait until busy flag is cleared
    while ( (c=lcd_read(0)) & (1<<LCD_BUSY)) {}

    //the address counter is updated 4us after the busy flag is cleared
    _delay_us(2);

    //now read the address counter
    return (lcd_read(0));  // return address counter

}//lcd_waitbusy


/*************************************************************************
Move cursor to the start of next line or to the first line if the cursor
is already on the last line.
Input:    the current address counter
Returns:  none
*************************************************************************/
static inline void lcd_newline(uint8_t pos)
{
    register uint8_t addressCounter;

#if LCD_LINES==1
    addressCounter = 0;
#endif
#if LCD_LINES==2
    if ( pos < (LCD_START_LINE2) )
        addressCounter = LCD_START_LINE2;
    else
        addressCounter = LCD_START_LINE1;
#endif
#if LCD_LINES==4
    if ( pos < LCD_START_LINE3 )
        addressCounter = LCD_START_LINE2;
    else if ( (pos >= LCD_START_LINE2) && (pos < LCD_START_LINE4) )
        addressCounter = LCD_START_LINE3;
    else if ( (pos >= LCD_START_LINE3) && (pos < LCD_START_LINE2) )
        addressCounter = LCD_START_LINE4;
    else
        addressCounter = LCD_START_LINE1;
#endif
    lcd_command((1<<LCD_DDRAM)+addressCounter);

}//lcd_newline

/*************************************************************************
Extracts X and Y coordinates from address counter
Input:    current address counter,
          pointer to where x is to be stored,
          pointer to where y is to be stored
Returns:  none
*************************************************************************/
static void lcd_getxy(uint8_t pos, uint8_t* x, uint8_t* y){

#if LCD_LINES==1
  *x = (pos - LCD_START_LINE1);
  *y = 0;
#endif

#if LCD_LINES==2
  //if on first line
  if( pos < LCD_START_LINE2 ){
    *x = (pos - LCD_START_LINE1);
    *y = 0;
  }
  //otherwise, on second line
  else {
    *x = (pos - LCD_START_LINE2);
    *y = 1;
  }
#endif

#if LCD_LINES==4
  //if on first line
  if( pos < LCD_START_LINE3 ){
    *x = (pos - LCD_START_LINE1);
    *y = 0;
  }
  //if on third line
  else if( pos < LCD_START_LINE2 ){
    *x = (pos - LCD_START_LINE3);
    *y = 2;
  }
  //if on second line
  else if( pos < LCD_START_LINE4 ){
    *x = (pos - LCD_START_LINE2);
    *y = 1;
  }
  //otherwise, on fourth line
  else{
    *x = (pos - LCD_START_LINE4);
    *y = 3;
  }
#endif

}//lcd_getxy

/*
** PUBLIC FUNCTIONS
*/

/*************************************************************************
Send LCD controller instruction command
Input:   instruction to send to LCD controller, see HD44780 data sheet
Returns: none
*************************************************************************/
void lcd_command(uint8_t cmd)
{
    lcd_waitbusy();
    lcd_write(cmd,0);
}


/*************************************************************************
Send data byte to LCD controller
Input:   data to send to LCD controller, see HD44780 data sheet
Returns: none
*************************************************************************/
void lcd_data(uint8_t data)
{
    lcd_waitbusy();
    lcd_write(data,1);
}



/*************************************************************************
Set cursor to specified position
Input:    x  horizontal position  (0: left most position)
          y  vertical position    (0: first line)
Returns:  none
*************************************************************************/
void lcd_gotoxy(uint8_t x, uint8_t y)
{
#if LCD_LINES==1
    lcd_command((1<<LCD_DDRAM)+LCD_START_LINE1+x);
#endif
#if LCD_LINES==2
    if ( y==0 )
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE1+x);
    else
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE2+x);
#endif
#if LCD_LINES==4
    if ( y==0 )
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE1+x);
    else if ( y==1)
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE2+x);
    else if ( y==2)
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE3+x);
    else // y==3
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE4+x);
#endif

}//lcd_gotoxy

/*************************************************************************
Clear display and set cursor to home position
*************************************************************************/
void lcd_clrscr(void)
{
    lcd_command(1<<LCD_CLR);
}


/*************************************************************************
Set cursor to home position
*************************************************************************/
void lcd_home(void)
{
    lcd_command(1<<LCD_HOME);
}

/*************************************************************************
Display character at current cursor position
Input:    character to be displayed
Returns:  none
*************************************************************************/
void lcd_putc(char c)
{
  uint8_t pos, x, y;

  pos = lcd_waitbusy(); //read busy-flag and address counter
  if (c=='\n') {
    lcd_newline(pos);
  } else {
    lcd_getxy(pos, &x, &y);
    lcd_write(c, 1);

    //if, before writing that character, the cursor was at the end of the line
    if( x == (LCD_DISP_LENGTH-1) ){
      //if( LCD_WRAP_LINES ){
      #if LCD_WRAP_LINES
        lcd_newline(pos);
      //} else {
      #else
        //put the cursor back to the end of the display
        lcd_gotoxy(LCD_DISP_LENGTH-1, y);
      //}
      #endif
    }
  }
}//lcd_putc


/*************************************************************************
Display string without auto linefeed
Input:    string to be displayed
Returns:  none
*************************************************************************/
void lcd_puts(const char *s)
//print string on lcd (no auto linefeed)
{
    register char c;

    while ( (c = *s++) ) {
        lcd_putc(c);
    }

}//lcd_puts


/*************************************************************************
Display string from program memory without auto linefeed
Input:     string from program memory be be displayed
Returns:   none
*************************************************************************/
void lcd_puts_p(const char *progmem_s)
//print string from program memory on lcd (no auto linefeed)
{
    register char c;

    while ( (c = pgm_read_byte(progmem_s++)) ) {
        lcd_putc(c);
    }

}//lcd_puts_p

/*************************************************************************
Load a custom glyph into CGRAM
Input:     pointer to 8 bytes in program memory to be loaded
           slot number (0-7) specifying the destination in CGRAM
Returns:   none
*************************************************************************/
void lcd_load_glyph(const unsigned char* PROGMEM glyph, uint8_t slot){
  uint8_t i;

  //convert slot number into address
  slot *= 8;

  //send command to write to CGRAM at the address specified
  lcd_command( _BV(LCD_CGRAM) | (slot&(0b00111111)) );

  //send in each byte of the glyph sequentially
  for(i=0; i<8; i++){
    lcd_data( pgm_read_byte_near(glyph+i) );
  }

  //terminate the transaction with a DDRAM write command
  lcd_command( _BV(LCD_DDRAM) );
}

/*************************************************************************
Initialize display and select type of cursor
Input:    dispAttr LCD_DISP_OFF            display off
                   LCD_DISP_ON             display on, cursor off
                   LCD_DISP_ON_CURSOR      display on, cursor on
                   LCD_DISP_CURSOR_BLINK   display on, cursor on flashing
Returns:  none
*************************************************************************/
void lcd_init(uint8_t dispAttr)
{
  //Initialize LCD to 4 bit I/O mode
  LCD_DDR |= ( _BV(LCD_RS_PIN) |
               _BV(LCD_RW_PIN) |
               _BV(LCD_E_PIN) |
               _BV(LCD_DATA0_PIN) |
               _BV(LCD_DATA1_PIN) |
               _BV(LCD_DATA2_PIN) |
               _BV(LCD_DATA3_PIN) );

  _delay_us(16000); //wait 16ms or more after power-on

  //initial write to lcd is 8bit
  LCD_DATA1_PORT |= _BV(LCD_DATA1_PIN);
  LCD_DATA0_PORT |= _BV(LCD_DATA0_PIN);
  lcd_e_toggle();
  _delay_us(4992); //delay, busy flag can't be checked here

  //repeat last command
  lcd_e_toggle();
  _delay_us(64); //delay, busy flag can't be checked here

  //repeat last command a third time
  lcd_e_toggle();
  _delay_us(64); //delay, busy flag can't be checked here

  //now configure for 4bit mode
  LCD_DATA0_PORT &= ~_BV(LCD_DATA0_PIN);
  lcd_e_toggle();
  _delay_us(64); //some displays need this additional delay
  //from now the LCD only accepts 4 bit I/O, we can use lcd_command()

  lcd_command(LCD_FUNCTION_DEFAULT);  //function set: display lines
  lcd_command(LCD_DISP_OFF);          //display off
  lcd_clrscr();                       //display clear
  lcd_command(LCD_MODE_DEFAULT);      //set entry mode
  lcd_command(dispAttr);              //display/cursor control
}
