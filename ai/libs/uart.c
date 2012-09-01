/*
 * uart.c
 *
 *  Created on: Jul 12, 2011
 *      Author: kevin
 *
 *
 *      For 250kbps use MAX3232 instead of MAX232 out of spec
 */
#include "uart.h"


#define uart_udr_empty (UCSRnA & (1<<UDREn))



void uart_send(uint8_t data){
	while(!uart_udr_empty); //wait for data register to be empty
	UDRn = data; //putting data into register sends it
}

uint8_t uart_receive(){
	while(!(UCSRnA & (1<<RXCn)));//wait for data
	return UDRn;
}

void uart_init_(uint8_t baud){
	UBRRn = baud;						//set baud rate
	UCSRnC = (0<<USBSn) | (3<<UCSZn0);	//frame format: 8data, 1 stop bit
	UCSRnB = (1<<RXENn) | (1<<TXENn);	//enable Rx and Tx
}


void uart_endl(void){
	uart_send('\r');
	uart_send('\n');
}

void uart_puts (const char *s){
	do{
		uart_send (*s);
		s++;
    }
	while (*s);

}

void uart_send_byte(uint8_t b, const char *s){
	char print[3];
	uart_puts(s); uart_send(' ');
	itoa(b, print, 10);
	uart_puts(print); uart_send(' ');
}

void uart_send_word(uint16_t w, const char *s){
	char print[5];
	uart_puts(s); uart_send(' ');
	itoa(w, print, 10);
	uart_puts(print); uart_send(' ');
}

void uart_send_word_i(uint8_t i, uint16_t w, const char *s){
	char print[5];
	itoa(i, print, 10);
	uart_puts(print); uart_send('.');
	uart_puts(s); uart_send(' ');
	itoa(w, print, 10);
	uart_puts(print); uart_send(' ');
}


void uart_writeline(const char *s){
	uart_puts(s);
	uart_endl();
}

void uart_drawline(uint8_t length, const uint8_t space){
	if(space){uart_send('\r');uart_send('\n');}
	while(length){
		length--;
		uart_send('-');
	}
	if(space){uart_send('\r');uart_send('\n');}
}

void uart_drawline_def(){
	uart_drawline(15, 1);
}
