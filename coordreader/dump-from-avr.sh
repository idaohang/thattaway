#!/bin/bash

#Be sure to update this value with the programmer that you have
PROGRAMMER=usbtiny

#Dump the EEPROM image
avrdude -c $PROGRAMMER -B 1 -p atmega644 -U eeprom:r:eeprom.dump:r
#Extract coordinates from the EEPROM image
./coordreader eeprom.dump
