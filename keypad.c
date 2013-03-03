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

#include <avr/io.h>
#include <util/delay.h>
#include "keypad.h"
#include "keys.h"

#define __KEYPAD_PORT PORTC
#define __KEYPAD_PIN PINC
#define __KEYPAD_DDR DDRC

const uint8_t __KEYPAD_ROW_MASK = 0b00001111;
const uint8_t __KEYPAD_COL_MASK = 0b01110000;

//keys
const uint16_t __KEYPAD_KEY_1 = 1<<0;
const uint16_t __KEYPAD_KEY_2 = 1<<1;
const uint16_t __KEYPAD_KEY_3 = 1<<2;
const uint16_t __KEYPAD_KEY_4 = 1<<3;
const uint16_t __KEYPAD_KEY_5 = 1<<4;
const uint16_t __KEYPAD_KEY_6 = 1<<5;
const uint16_t __KEYPAD_KEY_7 = 1<<6;
const uint16_t __KEYPAD_KEY_8 = 1<<7;
const uint16_t __KEYPAD_KEY_9 = 1<<8;
const uint16_t __KEYPAD_KEY_STAR = 1<<9;
const uint16_t __KEYPAD_KEY_0 = 1<<10;
const uint16_t __KEYPAD_KEY_POUND = 1<<11;

//initialize the keypad
void keypad_init(){
  //set rows to output
  __KEYPAD_DDR |= __KEYPAD_ROW_MASK;
  //set columns to input
  __KEYPAD_DDR &= ~__KEYPAD_COL_MASK;
}

//gets the state of the keypad
//  returns uint16_t - the state of the keypad
uint16_t keypad_getst(){
  uint16_t result = 0x0000;
  uint8_t i;

  //read in each row at a time
  for( i=0; i<4; i++){
    //clear row bits
    __KEYPAD_PORT &= ~__KEYPAD_ROW_MASK;
    //set the row
    __KEYPAD_PORT |= ((1 << i) & __KEYPAD_ROW_MASK);
    //let the inputs settle
    _delay_us(1);

    //read the columns
    result |= (((__KEYPAD_PIN & __KEYPAD_COL_MASK)>>4) << (i * 3));
  }

  return result;
}

//gets the debounced state of the keypad
//  returns uint16_t - the debounced state of the keypad
uint16_t keypad_getst_deb(){
  uint16_t keyst;

  //get the key states and debounce them
  keyst = keypad_getst();
  _delay_ms(5);
  keyst &= keypad_getst();
  _delay_ms(5);
  keyst &= keypad_getst();
  _delay_ms(5);
  keyst &= keypad_getst();
  _delay_ms(5);
  keyst &= keypad_getst();

  return keyst;
}

//gets a character from the keypad, uses debouncing
//  returns char - \0 if no key entered, otherwise the button pressed
char keypad_getchar(){
  char result = NO_BUTTON;
  uint16_t keyst = keypad_getst_deb();

  if(keyst & __KEYPAD_KEY_1){
      result = '1';
  } else if(keyst & __KEYPAD_KEY_2){
      result = '2';
  } else if(keyst & __KEYPAD_KEY_3){
      result = '3';
  } else if(keyst & __KEYPAD_KEY_4){
      result = '4';
  } else if(keyst & __KEYPAD_KEY_5){
      result = '5';
  } else if(keyst & __KEYPAD_KEY_6){
      result = '6';
  } else if(keyst & __KEYPAD_KEY_7){
      result = '7';
  } else if(keyst & __KEYPAD_KEY_8){
      result = '8';
  } else if(keyst & __KEYPAD_KEY_9){
      result = '9';
  } else if(keyst & __KEYPAD_KEY_STAR){
      result = '.';
  } else if(keyst & __KEYPAD_KEY_0){
      result = '0';
  } else if(keyst & __KEYPAD_KEY_POUND){
      result = '#';
  }

  return result;
}
