#ifndef __UART_H
#define __UART_H

#include <inttypes.h>
#include <avr/pgmspace.h>

//This code relies on F_CPU being defined as the CPU clockrate in Hertz.
//The example Makefile supplied with this library does this.
// e.g.: #define F_CPU 8000000

//initialize a uart
void uart_init(unsigned long baudrate);

//send a byte
// char data - the data to be sent
inline void uart_send(char data);

//waits until a byte is received and returns it
// returns char - the data received
inline char uart_get(void);

//prints a nibble (4 bits) in hexadecimal
//  uint8_t nibble - the nibble to print (only 4 lowest bits used)
void uart_print4(uint8_t nibble);

//prints a byte (8 bits) in hexadecimal
//  uint8_t val - the byte to print
void uart_print8(uint8_t val);

//prints a "word" (16 bits) in hexadecimal
//  uint16_t val - the "word" to print
void uart_print16(uint16_t val);

//sends characters from a null-terminated string
//does NOT send the null character
//  const char* data - the string to be sent
//  uint16_t maxlen - the maximum number of characters to be sent
void uart_print(const char* data, uint16_t maxlen);

//uart_prints a null-terminated string followed by CR and LF chars
//  const char* data - the string to be sent
//  uint16_t maxlen - the maximum number of characters to be sent
void uart_println(const char* data, uint16_t maxlen);

//sends characters from a null-terminated string in progmem
//does NOT send the null character
//  const char* PROGMEM data - the string to be sent
//  uint16_t maxlen - the maximum number of characters to be sent
void uart_print_p(const char* PROGMEM data, uint16_t maxlen);

//macro for the putting string literals in progmem automatically
#define uart_print_P(data) uart_print_p(PSTR(data), 0xFFFF)

//uart_print_ps a null-terminated string in progmem followed by CR and LF chars
//  const char* PROGMEM data - the string to be sent
//  uint16_t maxlen - the maximum number of characters to be sent
void uart_println_p(const char* PROGMEM data, uint16_t maxlen);

//macro for the putting string literals in progmem automatically
#define uart_println_P(data) uart_println_p(PSTR(data), 0xFFFF)

//receives up to maxlen characters, putting them into a string
//  char* data - the string to be written to
//  uint16_t maxlen - the number of characters to be sent
//  uint8_t echo - if 0, characters received will not be echoed
void uart_getln(char* data, uint16_t maxlen, uint8_t echo);

#endif
