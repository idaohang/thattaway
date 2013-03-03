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

#include <inttypes.h>
#include "uart.h"
#include "gps.h"
#include "keypad.h"
#include "lcd.h"
#include "storage.h"
#include "ui.h"

//the slot of EEPROM that stores the location of "home"
static const uint16_t HOME_SLOT = 0;

void init(){
  //initialize hardware
  keypad_init();
  gps_init();
  ui_init();
}

int main(){
  //struct for storing state
  loc_state_t loc;
  read_dest(HOME_SLOT, &loc);

  init();

  lcd_clrscr();
  lcd_puts_P("Waiting for GPS\ndata...");

  for(;;){
    gps_update(&loc);
    ui_update(&loc);
  }

  return 0;
}
