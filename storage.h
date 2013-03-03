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

#ifndef __STORAGE_H
#define __STORAGE_H

#include <inttypes.h> //for uin16_t
#include "gps.h" //for loc_state_t

//stores a floating point number into the EEPROM
//  uint16_t idx - the one-float-sized bank to store the float into
//  float data - the data to store
void store_float(uint16_t idx, float data);

//stores a floating point number into the EEPROM
//  uint16_t idx - the one-float-sized bank to read the float from
//  return float - the data read
float get_float(uint16_t idx);

//stores the current location to the EEPROM
//  uint16_t slot - the slot to store data to
//  const loc_state_t* loc - the location to read data from
//  returns char - 0 if the save failed, 1 otherwise
char store_loc(uint16_t slot, const loc_state_t* loc);

//stores the trip destination to the EEPROM
//  uint16_t slot - the slot to store data to
//  const loc_state_t* loc - the location to read data from
char store_dest(uint16_t slot, const loc_state_t* loc);

//reads the trip destination from the EEPROM
//  uint16_t slot - the slot to read data from
//  loc_state_t* loc - the location to write data to
void read_dest(uint16_t slot, loc_state_t* loc);

#endif
