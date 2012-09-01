/*
 * uart.h
 *
 *  Created on: Jul 12, 2011
 *      Author: kevin
 */

#ifndef UART_H_
#define UART_H_

// Includes
#include <avr/io.h>


// UART Hardware Defines
#define UBRRn UBRR0
#define UCSRnA UCSR0A
#define UCSRnC UCSR0C
#define USBSn USBS0
#define UCSZn0 UCSZ00
#define RXENn RXEN0
#define TXENn TXEN0
#define UCSRnB UCSR0B
#define RXCIEn RXCIE0
#define TXCIEn TXCIE0
#define UDRIEn UDRIE0
#define RXCn RXC0
#define UDREn UDRE0
#define UDRn UDR0


// UART Functions
void uart_init_(uint8_t baud);
#define uart_init(baud) uart_init_((F_CPU/(16*baud))-1)
void uart_send(uint8_t data);
uint8_t uart_receive();

void uart_endl(void);
void uart_puts (const char *s);
void uart_send_byte(uint8_t b, const char *s);
void uart_send_byte(uint8_t b, const char *s);
void uart_send_word(uint16_t w, const char *s);
void uart_send_word_i(uint8_t i, uint16_t w, const char *s);
void uart_writeline(const char *s);
void uart_drawline(uint8_t length, const uint8_t space);
void uart_drawline_def();


//Possible BaudRates:
//#define BAUDRATE 4800UL 	//4800
//#define BAUDRATE 9600UL 	//9600
//#define BAUDRATE 19200UL 	//19.2K
#define BAUDRATE 38400UL 	//38.4K
//#define BAUDRATE 76800UL 	//76.8K
//#define BAUDRATE 250000UL 	//250K
//#define BAUDRATE 500000UL 	//500K


#endif /* UART_H_ */
