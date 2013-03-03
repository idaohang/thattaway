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

#ifndef __LCD_EXTRAS_H
#define __LCD_EXTRAS_H

#include <inttypes.h> //for uint16_t, uint8_t

//blanks a line on the display
//  uint8_t row - the row to blank
void lcd_clearline(uint8_t row);

//writes a float to the lcd
//  float val - the floating point value to write
void print_float(float val);

//get a string of null-terminated input from the keypad
//  char* str - the string buffer to write to
//  uint8_t maxlen - the maximum length of the string buffer
void prompt_getln( char* str, uint8_t maxlen );

//retrieves a floating point number from the user via the keypad and LCD
//  returns float - the value entered by the user
float prompt_float();

//prompts the user for an integer
//  uint8_t row - the row to draw the prompt on
//  returns uint16_t - the value entered
uint16_t prompt_uint16(uint8_t row);

#endif
