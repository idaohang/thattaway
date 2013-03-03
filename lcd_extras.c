/***
Copyright (C) 2012 David DiPaola

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
***/

#include <util/delay.h>
#include <avr/pgmspace.h> //for program space storage
#include <stdio.h> //for sprintf and NULL
#include <stdlib.h> //for atof and such
#include <string.h> //for cstring processing
#include "lcd_extras.h"
#include "keypad.h" //for keypad input
#include "keys.h"
#include "lcd.h" //for LCD output

//constants
static const uint8_t SMALL_BUF_LEN = 17;

//data stored in program memory
//user input prompt text
static const char PROGMEM UI_SIGN_PROMPT[] = "   1) Neg 3) Pos";
static const char PROGMEM UI_NUM_PROMPT[] = "#:";

//blanks a line on the display
//  uint8_t row - the row to blank
void lcd_clearline(uint8_t row){
  uint8_t i;

  lcd_gotoxy(0, row);
  for(i=0; i<LCD_DISP_LENGTH; i++){
    lcd_putc(' ');
  }
  lcd_gotoxy(0, row);
}

//writes a float to the lcd
//  float val - the floating point value to write
void print_float(float val){
  char small_buffer[SMALL_BUF_LEN];

  sprintf_P( small_buffer,
             PSTR("%f"), val );

  lcd_puts(small_buffer);
}

//get a string of null-terminated input from the keypad
//  char* str - the string buffer to write to
//  uint8_t maxlen - the maximum length of the string buffer
void prompt_getln( char* str, uint8_t maxlen ){
  uint8_t i = 0;
  char curr = NO_BUTTON;
  char prev = NO_BUTTON;

  //while "enter" hasn't been pressed and we're withing buffer limits
  while( (curr != ENTER_BUTTON) && (i < (maxlen-1)) ){
    curr = keypad_getchar();

    //make sure the keys have only been pressed this loop
    if( (curr != prev) && (curr != NO_BUTTON) ){
      lcd_putc(curr);
      str[i] = curr;
      i++;
    }

    prev = curr;
  }

  str[i] = '\0'; //terminate the string
}

//retrieves a floating point number from the user via the keypad and LCD
//  returns float - the value entered by the user
float prompt_float(){
  char small_buffer[SMALL_BUF_LEN];
  char ch;
  float result;

  lcd_clrscr();

  //prompt for sign
  lcd_puts_p(UI_SIGN_PROMPT);
  lcd_gotoxy( 0,0 );
  lcd_command(LCD_DISP_ON_CURSOR_BLINK); //turn on the blinking cursor
  //wait until the proper key is pressed
  do {
    ch = keypad_getchar();
  } while( (ch != '1') && (ch != '3') );
  lcd_putc(ch);

  //wait for keys to be released
  while( keypad_getchar() != NO_BUTTON ){ }

  //prompt for digits
  lcd_gotoxy( 0,1 );
  lcd_puts_p(UI_NUM_PROMPT);
  prompt_getln( small_buffer, SMALL_BUF_LEN );

  //store the value
  result = atof( small_buffer );

  //if the user said this was a negative number...
  if( ch == '1' ){
    result = -result;
  }

  lcd_command(LCD_DISP_ON); //turn the cursor back off
  lcd_clrscr();

  return result;
}

//prompts the user for an integer
//  uint8_t row - the row to draw the prompt on
//  returns uint16_t - the value entered
uint16_t prompt_uint16(uint8_t row){
  char small_buffer[SMALL_BUF_LEN];

  //wait for keys to be released before accepting input
  while(keypad_getchar() != NO_BUTTON) {}

  lcd_clearline(row);
  lcd_gotoxy(0, row);
  lcd_puts_p(UI_NUM_PROMPT);
  prompt_getln( small_buffer, SMALL_BUF_LEN );

  return (uint16_t)atoi(small_buffer);
}
