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

#include <avr/eeprom.h> //for EEPROM read/write
#include <inttypes.h> //for uint16_t
#include "storage.h"
#include "gps.h" //for loc_state_t

//For EEPROM documentation, see:
//  http://www.nongnu.org/avr-libc/user-manual/group__avr__eeprom.html

//stores a floating point number into the EEPROM
//  uint16_t idx - the one-float-sized bank to store the float into
//  float data - the data to store
void store_float(uint16_t idx, float data){
  //wait for the EEPROM to not be busy
  eeprom_busy_wait();
  //the first parameter of eeprom_write_float is the address to write to in
  //  bytes, I only use the (float*) cast to make the compiler shut up
  eeprom_write_float((float*)(idx*sizeof(float)), data);
}

//stores a floating point number into the EEPROM
//  uint16_t idx - the one-float-sized bank to read the float from
//  return float - the data read
float get_float(uint16_t idx){
  //wait for the EEPROM to not be busy
  eeprom_busy_wait();
  //the first parameter of eeprom_read_float is the address to write to in
  //  bytes, I only use the (float*) cast to make the compiler shut up
  return eeprom_read_float((float*)(idx*sizeof(float)));
}

//stores the current location to the EEPROM
//  uint16_t slot - the slot to store data to
//  const loc_state_t* loc - the location to read data from
//  returns char - 0 if the save failed, 1 otherwise
char store_loc(uint16_t slot, const loc_state_t* loc){
  char result = 0;

  slot *= 2; //since this is a *pair* of floats

  //store the first float
  store_float(slot, (loc->curr_lat));
  //store the second float
  store_float(slot+1, (loc->curr_long));

  //verify the write
  if( ((loc->curr_lat) == get_float(slot)) &&
      ((loc->curr_long) == get_float(slot+1)) ){
    result = 1;
  }

  return result;
}

//stores the trip destination to the EEPROM
//  uint16_t slot - the slot to store data to
//  const loc_state_t* loc - the location to read data from
//  returns char - 0 if the save failed, 1 otherwise
char store_dest(uint16_t slot, const loc_state_t* loc){
  char result = 0;

  slot *= 2; //since this is a *pair* of floats

  //store the first float
  store_float(slot, (loc->dest_lat));
  //store the second float
  store_float(slot+1, (loc->dest_long));

  //verify the write
  if( ((loc->dest_lat) == get_float(slot)) &&
      ((loc->dest_long) == get_float(slot+1)) ){
    result = 1;
  }

  return result;
}

//reads the trip destination from the EEPROM
//  uint16_t slot - the slot to read data from
//  loc_state_t* loc - the location to write data to
void read_dest(uint16_t slot, loc_state_t* loc){
  slot *= 2; //since this is a *pair* of floats

  //get the first float
  (loc->dest_lat) = get_float(slot);
  //store the second float
  (loc->dest_long) = get_float(slot+1);
}
