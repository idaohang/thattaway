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

#ifndef __KEYPAD_H
#define __KEYPAD_H

#include <inttypes.h> //for uint16_t, uint8_t

//initialize the keypad
void keypad_init();

//gets a character from the keypad, uses debouncing
//  returns char - \0 if no key entered, otherwise the button pressed
char keypad_getchar();

//get a string of null-terminated input from the keypad
// char* str - the string buffer to write to
// uint8_t maxlen - the maximum length of the string buffer
void keypad_getln( char* str, uint8_t maxlen );

#endif
