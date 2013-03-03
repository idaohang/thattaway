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

#ifndef __GPS_H
#define __GPS_H

//holds a location state
struct loc_state {
  //stuff we get from the GPS
  unsigned long time;
  float curr_lat, curr_long, dest_lat, dest_long;
  int dop; //diution of positon
  int16_t heading;
  uint8_t sats;
  float altitude;
  float speed;
  //stuff we compute
  int deltaHeading;
  float distance;
};
typedef struct loc_state loc_state_t;

//initializes the GPS
void gps_init();

//get updated GPS data
//  (NOTE: this function waits until the final line of data has been sent by
//   the GPS)
//  loc_state_t* loc - where to store GPS data
void gps_update( loc_state_t* loc );

#endif
