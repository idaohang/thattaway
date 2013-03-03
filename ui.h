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

#ifndef __UI_H
#define __UI_H

#include <inttypes.h>
#include "gps.h" //for loc_state_t

//initializes the LCD and loads custom glyphs
void ui_init();

//prints one of 8 cardinal directions to the LCD
//  const int16_t degrees - the input, in degrees
void ui_print_cardinal(const int16_t degrees);

//draws a single line with destination info
//  const uint8_t row - the row to draw the line on
//  const loc_state_t* loc - GPS location information
void ui_draw_dest_info( const uint8_t row, const loc_state_t* loc );

//draws a single line with GPS satellite info
//  const uint8_t row - the row to draw the line on
//  const loc_state_t* loc - GPS location information
void ui_draw_sat_info( const uint8_t row, const loc_state_t* loc );

//draws a single line with driving info
//  const uint8_t row - the row to draw the line on
//  const loc_state_t* loc - GPS location information
void ui_draw_driving_info( const uint8_t row, const loc_state_t* loc );

//asks the user what latitude and longitude to set the destination to
//  loc_state_t* loc - where to write the destination information to
void ui_enter_dest_screen(loc_state_t* loc);

//asks the user what slot thay wish to load data from and loads the data into
//the destination portions of a location state
//  loc_state_t* loc - the location state to store data to
void ui_load_screen(loc_state_t* loc);

//asks the user what slot they wish to save the destination to
//  const loc_state_t* loc - the location data to be read
void ui_save_screen(const loc_state_t* loc);

//draws UI elements to the screen and accepts user input
//  loc_state_t* loc - the location data to use/modify
void ui_update(loc_state_t* loc);

#endif
