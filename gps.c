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
#include <stdio.h> //for sprintf and NULL
#include <stdlib.h> //for atof and such
#include <string.h> //for cstring processing
#include "gps.h"
#include "uart.h"
#include "coord_dist.h"

static const char* __GPS_DELIM = ",";

static const unsigned long __GPS_BAUD = 38400;
static const uint8_t __GPS_ECHO_OFF = 0;

static const uint8_t __GPS_LARGE_BUF_LEN = 80;

//converts an ASCII character to an integer
static inline int ascii_to_dec(char ch){
  return ch-0x30;
}

//parses an 8 digit GPS coordinate
static float parse_8digit(char* pch){
  float result;

  // it'll look like: 1234.5678
  // where 34.5678 is the number of minutes and 12 is the latitude

  // we only want the first two characters initially
  result = ascii_to_dec(pch[0])*10;
  result += ascii_to_dec(pch[1]);
  pch += 2;
  //now, get the remaining numbers and convert from
  // minutes to a fraction
  result += atof( pch )/60;

  return result;
}

//parses a 9 digit GPS coordinate
static float parse_9digit(char* pch){
  float result;

  // it'll look like: 01234.5678
  // where 34.5678 is the number of minutes and 012 is the longitude

  //get the first digit
  result = ascii_to_dec(pch[0])*100;
  //then, parse the rest as an 8 digit
  pch++;
  result += parse_8digit(pch);

  return result;
}

//parses a GPGGA line
//  char* gpgga_line - a NULL-terminated string
//  loc_state_t* loc - where to store location data
void parseGPGGA( char* gpgga_line, loc_state_t* loc ){
  char* pch;

  //initialize our tokenizer (and discard the first token)
  pch = strtok( gpgga_line, __GPS_DELIM );

  //get the time:
  pch = strtok( NULL, __GPS_DELIM );
  // convert the cstring to an unsigned long and use the time offset
  loc->time = atol( pch ); //+TIME_OFFSET;

  //get the next token (the latitude)
  pch = strtok( NULL, __GPS_DELIM );
  loc->curr_lat = parse_8digit(pch);
  //get the next token (North/South latitude)
  pch = strtok( NULL, __GPS_DELIM );
  // if it's South, then negate the latitude
  if( pch[0]=='S' ){
    (loc->curr_lat) = -(loc->curr_lat);
  }

  //get the next token (the longitude)
  pch = strtok( NULL, __GPS_DELIM );
  loc->curr_long = parse_9digit(pch);
  //get the next token (East/West longitude)
  pch = strtok( NULL, __GPS_DELIM );
  // if it's West, then negate the longitude
  if( pch[0]=='W' ){
    (loc->curr_long) = -(loc->curr_long);
  }

  //get the next token (GPS link type)
  pch = strtok( NULL, __GPS_DELIM );

  //get the next token (number of satellites)
  pch = strtok( NULL, __GPS_DELIM );
  (loc->sats) = atoi( pch );

  //get the next token (horizontal accuracy)
  pch = strtok( NULL, __GPS_DELIM );

  //get the next token (altitude)
  pch = strtok( NULL, __GPS_DELIM );
  (loc->altitude) = atof( pch );
}

//parses a GPRMC line
//  char* gprmc_line - a NULL-terminated string
//  loc_state_t* loc - where to store location data
void parseGPRMC( char* gprmc_line, loc_state_t* loc ){
  char* pch;

  //initialize our tokenizer (and discard the first token)
  pch = strtok( gprmc_line, __GPS_DELIM );

  //get the time
  pch = strtok( NULL, __GPS_DELIM );

  //get the status
  pch = strtok( NULL, __GPS_DELIM );
  if( pch[0] == 'A' ){ //if the status is "Active"...
    //get latitude
    pch = strtok( NULL, __GPS_DELIM );
    pch = strtok( NULL, __GPS_DELIM ); //north or south

    //get latitude
    pch = strtok( NULL, __GPS_DELIM );
    pch = strtok( NULL, __GPS_DELIM ); //east or west

    //get speed in knots
    pch = strtok( NULL, __GPS_DELIM );

    //get track angle in degrees True
    pch = strtok( NULL, __GPS_DELIM );
    (loc->heading) = atoi( pch );

    //get the date
    //pch = strtok( line, NULL );
  }
} //end GPRMC parse

//parses a GPGSA line
//  char* gpgsa_line - a NULL-terminated string
//  loc_state_t* loc - where to store location data
void parseGPGSA( char* gpgsa_line, loc_state_t* loc ){
  uint8_t i=0;
  char* pch;

  //initialize our tokenizer (and discard the first token)
  pch = strtok( gpgsa_line, __GPS_DELIM );

  //skip a bunch of tokens and lastly get the PDOP token
  for(i=0; i<13; i++){
    pch = strtok( NULL, __GPS_DELIM );
  }

  // get dilution of precision
  (loc->dop) = atof( pch );
} //end GPGSA parse

//parses a GPVTG line
//  char* gpvtg_line - a NULL-terminated string
//  loc_state_t* loc - where to store location data
void parseGPVTG( char* gpvtg_line, loc_state_t* loc ){
  uint8_t i=0;
  char* pch;

  //initialize our tokenizer (and discard the first token)
  pch = strtok( gpvtg_line, __GPS_DELIM );

  //skip a bunch of tokens and lastly get the speed in km/h token
  for(i=0; i<6; i++){
    pch = strtok( NULL, __GPS_DELIM );
  }

  // get dilution of precision
  (loc->speed) = atof( pch );
}

//initializes the GPS
void gps_init(){
  //fire up the serial port
  uart_init(__GPS_BAUD);
}

//get updated GPS data
//  (NOTE: this function waits until the final line of data has been sent by
//   the GPS)
//  loc_state_t* loc - where to store GPS data
void gps_update( loc_state_t* loc ){
  char last_line = 0;
  char line[__GPS_LARGE_BUF_LEN];
  char gpgga_line[__GPS_LARGE_BUF_LEN];
  char gpgsa_line[__GPS_LARGE_BUF_LEN];
  char gprmc_line[__GPS_LARGE_BUF_LEN];
  char gpvtg_line[__GPS_LARGE_BUF_LEN];

  while( !last_line ){
    //get a line
    uart_getln(line, __GPS_LARGE_BUF_LEN, __GPS_ECHO_OFF);

    //check if it's the uber line that has lots of neato things
    if( strstr(line, "$GPGGA") != NULL ){
      strcpy( gpgga_line, line );
    }
    //check if it's the uber line that has lots of neato things
    if( strstr(line, "$GPGSA") != NULL ){
      strcpy( gpgsa_line, line );
    }
    //if this line is the one with position, velocity, and time
    if( strstr(line, "$GPRMC") != NULL ){
      strcpy( gprmc_line, line );
    }
    //if this line is the one with speed
    if( strstr(line, "$GPVTG") != NULL ){
      strcpy( gpvtg_line, line );

      //now that we've seen the last line we care about, begin calc
      parseGPGGA( gpgga_line, loc );
      parseGPGSA( gpgsa_line, loc );
      parseGPRMC( gprmc_line, loc );
      parseGPVTG( gpvtg_line, loc );

      //finally, calculate the distance
      loc->distance = get_distance(loc->curr_lat, loc->curr_long,
                                   loc->dest_lat, loc->dest_long);
      //...and the heading
      loc->deltaHeading = get_fwd_azimuth(loc->curr_lat, loc->curr_long,
                                          loc->dest_lat, loc->dest_long)-loc->heading;

      //if the "left turn" is too big, make it a "right turn"
      if( loc->deltaHeading < -180 ){
        loc->deltaHeading = loc->deltaHeading+360;
      }

      last_line = 1; //break out of the loop
    }
  }
}
