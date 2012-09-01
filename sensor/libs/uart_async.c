/*
 * uart_async.c
 *
 *  Created on: Jul 1, 2012
 *      Author: kevin
 *
 *
 */
#include "uart_async.h"

#define uart_udr_empty (UCSRnA & (1<<UDREn))

struct{
	struct{
		char buf[UART_BUFFER_LENGTH];
		char* write;
		char* read;
	}tx;
} uart_a;

void uart_async_init_(uint8_t baud){
	// Initialize Global Variables
	uart_a.tx.read = uart_a.tx.buf;
	uart_a.tx.write = uart_a.tx.buf;
	// Initialize Hardware
	UBRRn = baud;						//set baud rate
	UCSRnC = (0<<USBSn) | (3<<UCSZn0);	//frame format: 8data, 1 stop bit
	UCSRnB = (1<<TXENn);				//enable Tx (to enable RX: (1<<RXENn) )

}

bool uart_async_run(){
	if(uart_udr_empty){							// if send register is empty
		if(uart_a.tx.write != uart_a.tx.read){	// and there are bytes that need to be sent
			UDRn = *uart_a.tx.read;				// send byte
			/* move read buffer */
			if((uart_a.tx.read - uart_a.tx.buf) == UART_BUFFER_LENGTH - 1){
				uart_a.tx.read = uart_a.tx.buf;
			}else{
				uart_a.tx.read++;
			}
		}
	}
	return (uart_a.tx.write != uart_a.tx.read);	// return true if there is nothing to send
}


// ----------------- Send Functions ---------------------------------------------------
/*
 * adds one byte to the send buffer that will be sent asap
 */
void uart_send(uint8_t b){
	*uart_a.tx.write = b;
	if((uart_a.tx.write - uart_a.tx.buf) == UART_BUFFER_LENGTH - 1){
		uart_a.tx.write = uart_a.tx.buf;
	}else{
		uart_a.tx.write++;
	}
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
