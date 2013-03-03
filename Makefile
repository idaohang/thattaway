# Name: Makefile
# Author: David DiPaola

# This is a prototype Makefile. Modify it according to your needs.
# You should at least check the settings for
# DEVICE ....... The AVR device you compile for
# CLOCK ........ Target AVR clock rate in Hertz
# OBJECTS ...... The object files created from your source files. This list is
#                usually the same as the list of source files with suffix ".o".
# PROGRAMMER ... Options to avrdude which define the hardware you use for
#                uploading to the AVR and the interface where this hardware
#                is connected.
# FUSES ........ Parameters for avrdude to flash the fuses appropriately.

DEVICE     = atmega644
CLOCK      = 7372800
PROGRAMMER = -c usbtiny

OBJECTS    = main.o lcd.o lcd_extras.o uart.o keypad.o gps.o coord_dist.o storage.o ui.o
#BE SURE TO SET THE FUSEBIT FOR EEPROM PRESERVATION IF YOU WANT TO KEEP YOUR
#COORDINATES WHEN REPROGRAMMING THE AVR
FUSES      = -U lfuse:w:0xFD:m -U hfuse:w:0xD1:m -U efuse:w:0xFF:m

# Tune the lines below only if you know what you are doing:

AVRDUDE = avrdude $(PROGRAMMER) -B 1 -p $(DEVICE)
COMPILE = avr-gcc -Wall -Wl,-u,vfprintf -lprintf_flt -lm -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)
#thanks to paxdiablo on Stack Overflow for the printf args

# symbolic targets:
all:	main.hex

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:main.hex:i

fuse:
	$(AVRDUDE) $(FUSES)

# Xcode uses the Makefile targets "", "clean" and "install"
install: flash fuse

# if you use a bootloader, change the command below appropriately:
load: all
	bootloadHID main.hex

realclean: clean

clean:
	rm -f main.hex main.elf $(OBJECTS)

# file targets:
lcd.o:
	$(COMPILE) -c lcd.c

main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS) -lm
	avr-size -C main.elf

main.hex: main.elf
	rm -f main.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
# If you have an EEPROM section, you must also create a hex file for the
# EEPROM and add it to the "flash" target.

# Targets for code debugging and analysis:
disasm:	main.elf
	avr-objdump -d main.elf

cpp:
	$(COMPILE) -E main.c lcd.c lcd_extras.c uart.c keypad.c gps.c coord_dist.c storage.c ui.c

dump-eeprom:
	$(AVRDUDE) -U eeprom:r:eeprom.dump:r
