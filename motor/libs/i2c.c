/*
 * i2c_async.c
 *
 *  Created on: Jan 20, 2011
 *      Author: kevin
 *      Derived from/Inspired by: I2C master interface by Peter Fleury
 */

#include "i2c.h"





/********************Variables********************/
static struct{
	enum {
		I2C_IDLE = 0,
		I2C_SENDING_START_CONDITON,
		I2C_SENDING_ADDRESS,
		I2C_SENDING_STOP_CONDITION,
		I2C_WAITING_FOR_DEVICE,
		I2C_READING,
		I2C_WRITING,
		I2C_FAILED
	} state;
	enum {
		NONE = 0,
		READ_BYTE_ACK,	// read byte operation stores byte in "b"
		READ_BYTE_NAK,	// read byte operation stores byte in "b"
		READ_WORD,		// read word operation stores word in "w"
		READ_ARRAY,		// read array operation stores array length in "array length" and received words in "array_received"
		WRITE,
		WRITE_2
	} operation;
	uint8_t address;
	uint8_t error;
	union{
		uint8_t flags;
		struct {
			unsigned connecting :1;
			unsigned connected :1;
			unsigned reading_msb :1;
			unsigned failed :1;
			unsigned stop :1;
			unsigned stop_forced :1;
			unsigned start_wait :1;
			unsigned :1;
		};
	};
	union {
		uint16_t w;
		struct{
			uint8_t b1;
			uint8_t b2;
		};
		struct{
			uint8_t array_length;
			uint8_t array_received;
		};
	};
	uint8_t bit_mask;
	uint16_t* array;
	int8_t array_omit_value;
}i2c_a;







// Macros
#define set_state(st) { i2c_a.state = st; }			// sets the fsm state
#define set_operation(op) { i2c_a.operation = op; }	// sets the operation
#define end_operation() { i2c_a.operation = NONE; i2c_a.state = I2C_IDLE; }	//ends an operation => returns to I2C_IDLE
#define error() { i2c_a.state = I2C_FAILED; i2c_a.error = TW_STATUS; break; }// goes to error state and breaks



// i2c Macros

#define i2c_start_write(data) { TWDR = data; TWCR = (1<<TWINT) | (1<<TWEN); }
#define i2c_start_read_ack() { TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA); }
#define i2c_start_read_nak() { TWCR = (1<<TWINT) | (1<<TWEN); }
#define i2c_start_send_address(address) { TWDR = address; TWCR = (1<<TWINT) | (1<<TWEN); }
#define i2c_send_start_condition() { TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN); }
#define i2c_send_stop_condition() { TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO); }
#define i2c_read_status() TW_STATUS
#define i2c_trans_completed (TWCR & (1<<TWINT))
#define i2c_stop_trans_not_completed (TWCR & (1<<TWSTO))






/*
 * initializes i2c
 */
void i2c_async_init_(uint8_t twbr_){
	TWSR = 0x00;                         // no prescaler
	TWBR = twbr_;
	// Load Default Values
	i2c_a.state = I2C_IDLE;
	i2c_a.operation = NONE;
	i2c_a.address = 0;
	i2c_a.error = 0;
	i2c_a.flags = 0;
	i2c_a.w = 0;
	i2c_a.bit_mask = 0;
	i2c_a.array_omit_value = 0;
}

void i2c_async_disable(void){
	TWSR = 0x00;
	TWBR = 0x00;
}

/*
 * i2c state machine
 * returns 0 when state == IDLE
 */
uint8_t i2c_async_run(void){
	uint8_t i2c_stat;
	switch(i2c_a.state){
	case I2C_IDLE:
		if(i2c_a.connecting){					// if connecting => start connection
			i2c_send_start_condition();
			set_state(I2C_SENDING_START_CONDITON);
		}
		else if(i2c_a.stop &&(i2c_a.stop_forced || (!i2c_a.operation && i2c_a.connected))){
			i2c_send_stop_condition();
			set_state(I2C_SENDING_STOP_CONDITION);
		}
		else {
			switch(i2c_a.operation){
			case NONE:
				break;
			case READ_BYTE_ACK:
			case READ_WORD:							// the first read action of read_word and read_array is always a read_ack
			case READ_ARRAY:
				i2c_start_read_ack();				// if read_ack, issue command, set state to I2C_READING
				set_state(I2C_READING);
				break;
			case READ_BYTE_NAK:						// if read_nak, issue command, set state to I2C_READING
				i2c_start_read_nak();
				set_state(I2C_READING);
				break;
			case WRITE:								// if read, issue command, set state to I2C_WRITING
				i2c_start_write(i2c_a.b1);
				set_state(I2C_WRITING);
				break;
			case WRITE_2:
				i2c_start_write(i2c_a.b2);
				set_state(I2C_WRITING);
				break;
			}
		}
		break;

	case I2C_SENDING_START_CONDITON:
		if(i2c_trans_completed){			// if transmission has been completed
			i2c_stat = TW_STATUS;			// read i2c status
			if((i2c_stat != TW_START) && (i2c_stat != TW_REP_START)) error();	//if there is an error => return and set state
			i2c_start_send_address(i2c_a.address);	// start sending address
			set_state(I2C_SENDING_ADDRESS);			// set state to I2C_SENDING_ADDRESS
		}
		break;

	case I2C_SENDING_ADDRESS:
		if(i2c_trans_completed){			// if transmission has been completed
			i2c_stat = TW_STATUS;			// read i2c status
			if(!i2c_a.start_wait && (i2c_stat != TW_MT_SLA_ACK) && (i2c_stat != TW_MR_SLA_ACK))
				error();							//if there is an error => return and set state
			if(i2c_a.start_wait && ((i2c_stat == TW_MT_SLA_NACK )||(i2c_stat ==TW_MR_DATA_NACK))){
				i2c_send_stop_condition();		// if device bussy => send stop condition
				set_state(I2C_WAITING_FOR_DEVICE);
			}
			else {
				i2c_a.connecting = false;				// if there was no error => connected => go to idle
				i2c_a.connected = true;
				i2c_a.start_wait = false;
				set_state(I2C_IDLE);
			}
		}
		break;

	case I2C_WAITING_FOR_DEVICE:
		if(!i2c_stop_trans_not_completed){	// if transmission has been completed
			set_state(I2C_IDLE);					// restart connect
		}
		break;

	case I2C_SENDING_STOP_CONDITION:
		if(!i2c_stop_trans_not_completed){	// if transmission has been completed
			i2c_a.connected = false;
			i2c_a.stop = false;
			end_operation();
		}
		break;

	case I2C_READING:
		if(i2c_trans_completed){		// if transmission has been completed
			switch(i2c_a.operation){
			case READ_BYTE_ACK:						// if reading byte value
			case READ_BYTE_NAK:
				i2c_a.b1 = TWDR;						// save byte in byte value
				end_operation();					// done => set operation to NONE and go to IDLE
				break;
			case READ_WORD:
				if(i2c_a.reading_msb){
					i2c_a.w = (TWDR<<8);			// save msb
					i2c_a.reading_msb = false;		// not reading msb anymore
					i2c_start_read_nak();			// start reading again
					set_state(I2C_READING);			// set corresponding state
				}
				else {
					i2c_a.w |= TWDR;
					end_operation();				// done => set operation to NONE and go to IDLE
				}
				break;
			case READ_ARRAY:
				//deb_b(i2c_a.array_length);
				if(i2c_a.reading_msb){
					*i2c_a.array = ((TWDR & i2c_a.bit_mask)<<8);// save msb
					i2c_a.reading_msb = false;		// not reading msb anymore
					if(i2c_a.array_length < 2){		// if length = 1
						i2c_start_read_nak();		// start reading again
					}
					else {
						i2c_start_read_ack();		// start reading again
					}
					set_state(I2C_READING);			// set corresponding state
				}
				else{
					*i2c_a.array |= TWDR;			// save lsb
					if(i2c_a.array_length < 2){		// if length = 1
						end_operation();			// done => set operation to NONE and go to IDLE
					}
					else {
						if(i2c_a.array_length == i2c_a.array_omit_value){
							i2c_a.array_omit_value = -1;
						}
						else {
							i2c_a.array++;				// move array pointer
							i2c_a.array_length--;		// decrease length to be read
						}
						i2c_a.reading_msb = true;	// read msb next
						i2c_start_read_ack();		// start reading again
						set_state(I2C_READING);		// set corresponding state
					}
				}
				break;
			case WRITE:
			case WRITE_2:
			case NONE:
				break;
			}
		}
		break;

	case I2C_WRITING:
		if(i2c_trans_completed){		// if transmission has been completed
			i2c_stat = TW_STATUS;			// read i2c status
			if(i2c_stat != TW_MT_DATA_ACK)  error();// if there is an error => return and set state
			if(i2c_a.operation == WRITE_2){
				i2c_a.operation = WRITE;
				set_state(I2C_IDLE);
			}
			else{
				end_operation();					// if there is no error => done => set operation to NONE and go to IDLE
			}
		}
		break;

	case I2C_FAILED:
		i2c_a.failed = true;
		end_operation();
		break;

	default:
		set_state(I2C_IDLE);
		break;
	}
	return (i2c_a.state || i2c_a.start_wait || i2c_a.operation || i2c_a.stop) && !i2c_a.failed;
}

/*
 * starts i2c connection
 */
void i2c_async_start(uint8_t address){
	i2c_a.connecting = true;
	i2c_a.address = address;
}

/*
 * starts i2c connection w/ wait
 */
void i2c_async_start_wait(uint8_t address){
	i2c_a.connecting = true;
	i2c_a.start_wait = true;
	i2c_a.address = address;
}

/*
 * stops i2c connection
 */
void i2c_async_stop(void){
	i2c_a.stop = true;
}

/*
 * starts read with acknowledge
 */
void i2c_async_read_ack(void){
	if(i2c_a.operation) return;
	i2c_a.operation = READ_BYTE_ACK;
}

/*
 * starts read without acknowledge
 */
void i2c_async_read_nak(void){
	if(i2c_a.operation) return;
	i2c_a.operation = READ_BYTE_NAK;
}

/*
 * starts reading two byte (1. MSB, 2.LSB) with acknowledge
 */
void i2c_async_read_word(void){
	if(i2c_a.operation) return;
	i2c_a.w = 0;
	i2c_a.reading_msb = true;
	i2c_a.operation = READ_WORD;
}

/*
 *  starts reading 16bit values into array
 */
void i2c_async_read_array(uint16_t* a, uint8_t length, uint8_t bit_mask){
	if(i2c_a.operation) return;
	i2c_a.bit_mask = bit_mask;
	i2c_a.array = a;
	i2c_a.array_length = length;
	i2c_a.reading_msb = true;
	i2c_a.array_omit_value = -1;
	i2c_a.operation = READ_ARRAY;
}

/*
 *  starts reading 16bit values into array, omits the value specified
 */
void i2c_async_read_array_omit(uint16_t* a, uint8_t length, uint8_t bit_mask, uint8_t omit){
	if(i2c_a.operation) return;
	i2c_a.bit_mask = bit_mask;
	i2c_a.array = a;
	i2c_a.array_length = length;
	i2c_a.reading_msb = true;
	i2c_a.array_omit_value = length - omit;
	i2c_a.operation = READ_ARRAY;
}

/*
 * starts write
 */
void i2c_async_write(uint8_t b){
	if(i2c_a.operation) return;
	i2c_a.b1 = b;
	i2c_a.operation = WRITE;
}

/*
 * starts writing two commands
 */
void i2c_async_write_2(uint8_t b1, uint8_t b2){
	if(i2c_a.operation) return;
	i2c_a.b2 = b1;
	i2c_a.b1 = b2;
	i2c_a.operation = WRITE_2;
}

/*
 * get the i2c byte value
 */
uint8_t i2c_async_get_byte(void){
	return i2c_a.b1;
}

/*
 * get the i2c word value
 */
uint16_t i2c_async_get_word(void){
	return i2c_a.w;
}

uint8_t i2c_async_error(void){
	if(!i2c_a.failed)
		return 0;
	return i2c_a.error;
}
