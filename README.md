thattaway
=========

A minimalistic GPS navigation system for the AVR microcontroller.

Features:
  -Calculates the change in heading required and distance to the goal
  -Can enter destination GPS coordinates manually
  -Can save/load entered or current GPS coordinates to/from EEPROM
  -Selectable information on bottom line of LCD:
    -dilution of precision, number of sats, time
    -speed, elevation
  -Program to dump the EEPROM and write a CSV file with stored coordinates

Hardware:
  This software was tested on a one-off ATMega644 board running at 7.3728MHz
  with an HD44780 LCD on port A, a 3x4 keypad on port C, and a Holux M1000
  hooked up to UART0. More details can be found in the source code.

Things you may wish to change/look at before building:
  Makefile - the programmer and MCU type are set up here
  lcdlibrary/lcd.h - what port and pins the LCD is connected to
  keypad.c - what port and pins the keypad is connected to and how to read it
  uart.c, uart.h - may need tweaking for MCUs I haven't tested it with
  ui.c - the EEPROM_SIZE define
  gps.c - the NMEA parsing may not be correct for your GPS receiver
