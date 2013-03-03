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
#include "ui.h"
#include "lcd_extras.h"
#include "keypad.h"
#include "keys.h" //definitions for keypad keys
#include "lcd.h"
#include "storage.h" //for EEPROM storage
#include "gps.h" //for loc_state_t

//time zone
//uncomment to enable timezone time correction
//#define USE_TIME_ZONE
#ifdef USE_TIME_ZONE
static const int8_t TIME_ZONE = -7;
#endif

//EEPROM constraints (change EEPROM_SIZE depending on your MCU)
#define SLOT_SIZE (sizeof(float)*2)
#define EEPROM_SIZE 2048
#define NUM_SLOTS (EEPROM_SIZE/SLOT_SIZE)

//constants
//length of the small buffer used when printing things to the screen
static const uint8_t SMALL_BUF_LEN = LCD_DISP_LENGTH+1; //from lcdlibrary/lcd.h
//dilution of precision constants
static const uint8_t MAX_GREAT_DOP = 2;
static const uint8_t MAX_GOOD_DOP = 5;
static const uint8_t MAX_ACCEPTABLE_DOP = 10;
//how long to wait for message display (in milliseconds)
static const uint16_t MSG_WAIT = 2000;
//which row to draw the destination info on
static const uint8_t DEST_ROW = 0;
//which row to draw the page on
static const uint8_t PAGE_ROW = 1;
//page numbers for the various pages
static const uint8_t SAT_PAGE = 0;
static const uint8_t DRIVING_PAGE = 1;
static const uint8_t MEM_PAGE = 2;
static const uint8_t DESTLOC_PAGE = 3;
static const uint8_t CURRLOC_PAGE = 4;
static const uint8_t MIN_PAGE = 0; //(sat page)
static const uint8_t MAX_PAGE = 4; //(current location page)
//minimum number of satellites required
static const uint8_t MIN_SATS = 3;

//signal meter glyphs
static const uint8_t FULL_SIG_CHAR = 0;
static const uint8_t GOOD_SIG_CHAR = 1;
static const uint8_t LOW_SIG_CHAR = 2;
static const uint8_t BAD_SIG_CHAR = 3;
static const uint8_t SIG_METER_LEN = (8*4);
static const char PROGMEM SIG_METER[] = {
  //full signal
  0b00011111,
  0b00000000,
  0b00011110,
  0b00000000,
  0b00011100,
  0b00000000,
  0b00011000,
  0b00000000,
  //medium signal
  0b00000000,
  0b00000000,
  0b00011110,
  0b00000000,
  0b00011100,
  0b00000000,
  0b00011000,
  0b00000000,
  //low signal
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00011000,
  0b00000000,
  //no signal
  0b00000100,
  0b00001110,
  0b00001110,
  0b00000100,
  0b00000000,
  0b00000100,
  0b00001110,
  0b00000100,
};

//variables
//stores the state of the bottom line
static uint8_t bottom_screen = 0;
//a timer
static uint8_t timer = 0;

//initializes the LCD and loads custom glyphs
void ui_init(){
  uint8_t i;

  //LCD on
  lcd_init(LCD_DISP_ON);

  //load signal meter glyphs
  lcd_command(_BV(LCD_CGRAM));
  for(i=0; i<SIG_METER_LEN; i++){
    lcd_data(pgm_read_byte_near(&SIG_METER[i]));
  }
}

//prints one of 8 cardinal directions to the LCD
//  int16_t degrees - the input, in degrees
void ui_print_cardinal(const int16_t degrees){

  if( (23 <= degrees) && (degrees <= 67) ){
    lcd_puts_P("NE");
  }
  if( (68 <= degrees) && (degrees <= 112) ){
    lcd_puts_P("E");
  }
  if( (113 <= degrees) && (degrees <= 157) ){
    lcd_puts_P("SE");
  }
  if( (158 <= degrees) && (degrees <= 202) ){
    lcd_puts_P("S");
  }
  if( (203 <= degrees) && (degrees <= 247) ){
    lcd_puts_P("SW");
  }
  if( (248 <= degrees) && (degrees <= 292) ){
    lcd_puts_P("W");
  }
  if( (293 <= degrees) && (degrees <= 337) ){
    lcd_puts_P("NW");
  }
  if( (338 <= degrees) || (degrees <= 21) ){
    lcd_puts_P("N");
  }
}

//reads the keypad for a yes or no answer
//  returns char - 0 if 4 was pressed, 1 if 6 was pressed
char ui_choice(){
  char result = 0;
  char button;

  //wait for all buttons to be released
  while( keypad_getchar() != NO_BUTTON ) {}
  //wait for user to enter a choice
  do{
    button = keypad_getchar();
  }while((button != '4') && (button != '6'));
  if( button == '6' ){
    result = 1;
  }

  return result;
}

//draws a single line with destination info
//  const uint8_t row - the row to draw the line on
//  const loc_state_t* loc - GPS location information
void ui_draw_dest_info( const uint8_t row, const loc_state_t* loc ){
  char small_buffer[SMALL_BUF_LEN];

  lcd_clearline(row);
  lcd_gotoxy(0, row);

  //only print info if we have enough satellites
  if( (loc->sats) >= MIN_SATS ){
    //print the distance
    if( (loc->distance) >= 1000000ul ){ //megameters
      sprintf_P( small_buffer,
                 PSTR("%lu.%dMm"),
                 (unsigned long)(loc->distance)/1000000ul,
                 (int16_t)((loc->distance)/10000ul)%100 );
      lcd_puts( small_buffer );
    } else if( (loc->distance) >= 1000 ){ //kilometers
      sprintf_P( small_buffer,
                 PSTR("%lu.%dkm"),
                 (unsigned long)(loc->distance)/1000,
                 (int16_t)((loc->distance)/10)%100 );
      lcd_puts( small_buffer );
    } else { //meters
      sprintf_P( small_buffer,
                 PSTR("%lum"),
                 (unsigned long)(loc->distance) );
      lcd_puts( small_buffer );
    }

    lcd_gotoxy(8, row);
    //direction of travel
    ui_print_cardinal( loc->heading );

    //print the heading (the \xDF is for something that looks like a degree
    // symbol)
    lcd_gotoxy(11, row);
    sprintf_P( small_buffer, PSTR("%d\xDF"), loc->deltaHeading );
    lcd_puts( small_buffer );
  } else {
    lcd_puts_P("Too few sats");
  }
}

//draws a single line with GPS satellite info
//  const uint8_t row - the row to draw the line on
//  const loc_state_t* loc - GPS location information
void ui_draw_sat_info( const uint8_t row, const loc_state_t* loc ){
  char small_buffer[SMALL_BUF_LEN];

  lcd_clearline(row);
  lcd_gotoxy(0, row);

  //print out the dilution of precision meter
  lcd_gotoxy(0, row);
  if( (loc->dop) <= MAX_GREAT_DOP ){ //if dop is ideal or excellent
    lcd_putc(FULL_SIG_CHAR);
  } else if( (loc->dop) <= MAX_GOOD_DOP ){ //if dop is good
    lcd_putc(GOOD_SIG_CHAR);
  } else if( (loc->dop) <= MAX_ACCEPTABLE_DOP ){ //if dop is moderate
    lcd_putc(LOW_SIG_CHAR);
  } else { //if dop is fair or poor
    lcd_putc(BAD_SIG_CHAR);
  }

  //number of satellites
  lcd_gotoxy(1, row);
  sprintf_P( small_buffer, PSTR("%dst"), loc->sats );
  lcd_puts( small_buffer );

  //time in HH:MM:SS
  lcd_gotoxy(8, row);
  sprintf_P( small_buffer,
             PSTR("%lu:%lu:%lu"),
             (((loc->time)/10000)
               #ifdef USE_TIME_ZONE
               +24+TIME_ZONE)%24
               #else
               )
               #endif
             ,
             ((loc->time)%10000)/100,
             (loc->time)%100);
  lcd_puts( small_buffer );
}

//draws a single line with driving info
//  const uint8_t row - the row to draw the line on
//  const loc_state_t* loc - GPS location information
void ui_draw_driving_info( const uint8_t row, const loc_state_t* loc ){
  char small_buffer[SMALL_BUF_LEN];

  lcd_clearline(row);
  lcd_gotoxy(0, row);

  //print out the speed
  lcd_gotoxy(0, row);
  sprintf_P( small_buffer, PSTR("%dkm/h"), (int)(loc->speed) );
  lcd_puts( small_buffer );

  //print out the altitude
  lcd_gotoxy(11, row);
  sprintf_P( small_buffer, PSTR("%dm"), (int)loc->altitude );
  lcd_puts( small_buffer );
}

//asks the user what latitude and longitude to set the destination to
//  loc_state_t* loc - where to write the destination information to
void ui_enter_dest_screen(loc_state_t* loc){
  lcd_clrscr();

  //get destination information
  lcd_clrscr();
  lcd_puts_P("Enter the goal\nLatitude...");
  _delay_ms(MSG_WAIT);
  (loc->dest_lat) = prompt_float();

  lcd_clrscr();
  lcd_puts_P("Enter the goal\nLongitude...");
  _delay_ms(MSG_WAIT);
  (loc->dest_long) = prompt_float();
}

//asks the user what slot thay wish to load data from and loads the data into
//the destination portions of a location state
//  loc_state_t* loc - the location state to store data to
void ui_load_screen(loc_state_t* loc){
  char load_from_mem;
  uint16_t slot;

  lcd_clrscr();
  lcd_puts_P("Load how?\n4)TYPE     6)MEM");

  load_from_mem = ui_choice();

  if( load_from_mem ){
    lcd_clrscr();
    lcd_puts_P("Load which slot?");
    slot = prompt_uint16(1); //ROW 1

    lcd_clrscr();
    if( slot < (NUM_SLOTS-1) ){
      read_dest(slot, loc);
      lcd_puts_P("LOADED");
    } else {
      lcd_puts_P("INVALID SLOT!");
    }
    _delay_ms(MSG_WAIT);
  } else {
    ui_enter_dest_screen(loc);
  }
}

//asks the user what slot they wish to save the destination to
//  const loc_state_t* loc - the location data to be read
void ui_save_screen(const loc_state_t* loc){
  uint16_t slot;
  uint8_t save_currloc = 0;
  //char button;
  char success;

  lcd_clrscr();
  lcd_puts_P("Save which?\n4)DEST 6)CURRLOC");
  save_currloc = ui_choice();

  lcd_clrscr();
  lcd_puts_P("Save to where?");
  slot = prompt_uint16(1); //ROW 1

  lcd_clrscr();
  if( slot < (NUM_SLOTS-1) ){
    if( save_currloc ){
      success = store_loc(slot, loc);
    } else {
      success = store_dest(slot, loc);
    }

    if( !success ){
      lcd_puts_P("SAVE FAILED!");
    } else {
      lcd_puts_P("SAVED");
    }
  } else {
    lcd_puts_P("INVALID SLOT!");
  }

  _delay_ms(MSG_WAIT);
}

//draws UI elements to the screen and accepts user input
//  loc_state_t* loc - the location data to use/modify
void ui_update(loc_state_t* loc){
  char curr_button = NO_BUTTON;

  timer++;

  curr_button = keypad_getchar();
  if( curr_button == LEFT_BUTTON ){
    bottom_screen--;
  } else if( curr_button == RIGHT_BUTTON ){
    bottom_screen++;
  }
  //if(bottom_screen > MAX_PAGE){
    //bottom_screen = MIN_PAGE;
  //}
  bottom_screen = bottom_screen % (MAX_PAGE+1);

  lcd_clrscr();
  //draw the destination info on row 0
  ui_draw_dest_info(DEST_ROW, loc);

  if( bottom_screen == SAT_PAGE ){
    ui_draw_sat_info(PAGE_ROW, loc);
  } else if( bottom_screen == DRIVING_PAGE ){
    ui_draw_driving_info(PAGE_ROW, loc);
  } else if( bottom_screen == MEM_PAGE ){
    lcd_gotoxy(0, PAGE_ROW);
    lcd_puts_P("4)LOAD    6)SAVE");
    if( curr_button == '4' ){
      ui_load_screen(loc);
    } else if( curr_button == '6' ){
      ui_save_screen(loc);
    }
  } else if( bottom_screen == DESTLOC_PAGE ){
    lcd_gotoxy(0, PAGE_ROW);
    if( (timer & _BV(2)) == 0 ){
      lcd_puts_P("DLa ");
      print_float(loc->dest_lat);
    } else {
      lcd_puts_P("DLo ");
      print_float(loc->dest_long);
    }
  } else if( bottom_screen == CURRLOC_PAGE ){
    lcd_gotoxy(0, PAGE_ROW);
    if( (timer & _BV(2)) == 0 ){
      lcd_puts_P("CLa ");
      print_float(loc->curr_lat);
    } else {
      lcd_puts_P("CLo ");
      print_float(loc->curr_long);
    }
  }
}
