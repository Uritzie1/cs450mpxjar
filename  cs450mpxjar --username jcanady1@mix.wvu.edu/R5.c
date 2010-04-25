/**
 * File Name: comhan.c
 * \title JAROS
 * \author Jonroy Canady
 * \author Adam Trainer
 * \author Rob Wayland   
 * \date 3/25/2010
 * \version: 5.0
 *
 * Components: 
 *
 *******************************************************************************
 * Change Log:
 *
 *        3/15/2010  JC           Original version: outline, nonfunctional 
 *        3/22/2010  RW           Primary Int Handler & Started Secondary Int Handlers
 *        3/25/2010  RW,AT,JC     Finished Int Handlers and COM functions   
 */

// Included ANSI C Files
#include <stdlib.h>
#include <dos.h>
#include "R12.h"

// Global Variables
static struct DCB *com_port;
static void interrupt (*oldfunc) (void);
static char iochar;
static char mask;
static int intType;

/**
 */
int com_open (int *eflag_p, int baud_rate) {
    int new_baud_rate, tmask;
	if (eflag_p == NULL) return ERR_OPEN_NULL_EFLAGP;
	if (baud_rate <= 0) return ERR_OPEN_INV_BAUD_RATE;
	if (com_port->flagOpen == OPEN) return ERR_OPEN_PORT_OPEN;
	
	// Initialize DCB
    com_port->status = IDLE;	
	com_port->flagOpen = OPEN;
	com_port->eventFlagp = eflag_p;
	com_port->ring_buffer_in = 0;
	com_port->ring_buffer_out = 0;
	com_port->ring_buffer_count = 0;
	oldfunc = getvect(COM1_INT_ID);
	setvect(COM1_INT_ID, &com_check);
    	
	new_baud_rate = 115200 / (long) baud_rate;
	outportb(COM1_LC, 0x80);
	outportb(COM1_BRD_LSB, new_baud_rate & 0xFF);
	outportb(COM1_BRD_MSB, (new_baud_rate >> 8) & 0xFF);
	outportb(COM1_LC, 0x03);
	disable();
	
	tmask = inportb(PIC_MASK);
	tmask = tmask & ~0x10;
	outportb(PIC_MASK, tmask);
	enable();
	outportb(COM1_MC, 0x08);
	outportb(COM1_INT_EN, 0x01);			
	return OK;
}

/**
 */
int com_read(char* buf_p, int *count_p) {
	if (com_port->flagOpen != OPEN) return ERR_READ_PORT_NOT_OPEN;
	if (buf_p == NULL) return ERR_READ_INV_BUFF_ADDR;
	if (count_p == NULL) return ERR_READ_INV_COUNT_VAR;
	if (com_port->status != IDLE) return ERR_READ_DEVICE_BUSY;
	
	com_port->in_buff = buf_p;
	com_port->in_count = count_p;
	com_port->in_done = 0;
	*(com_port->eventFlagp) = 0;
	disable();
	com_port->status = READING;
	
	while(com_port->ring_buffer_count > 0) { //loop while the ring buffer isn't empty
		//check first to see if requested count has been reached, or next character is a '/r'
		if(*(com_port->in_count) == com_port->in_done) { //finished copying chars
			com_port->status = IDLE;
			break;
		}
		if(com_port->ring_buffer[com_port->ring_buffer_out] == '\r') { //carriage return means end of input
			com_port->status = IDLE; //prevent interrupts from copying more characters into the in_buff
			com_port->ring_buffer_out++; //advance out pointer so that '\r' isn't copied for the next call
			if(com_port->ring_buffer_out > (RING_SIZE - 1)) com_port->ring_buffer_out = 0;
			com_port->ring_buffer_count--;
			break;
		}
		com_port->in_buff[com_port->in_done] = com_port->ring_buffer[com_port->ring_buffer_out]; //copy a character
		com_port->in_done++;
		com_port->ring_buffer_count--;
		com_port->ring_buffer_out++;
		if(com_port->ring_buffer_out > (RING_SIZE -1)) com_port->ring_buffer_out = 0;
	}
    enable();
	if(com_port->status == IDLE) { //Check to see if input is done
		com_port->in_buff[com_port->in_done] = '\0'; //null-terminate buffer
		*(com_port->in_count) = com_port->in_done; //returning number of characters written to calling process
		*(com_port->eventFlagp) = 1; //set event flag to signal end of read
	}	
    return OK;
}

/**
 */
int com_write(char *buf_p, int *count_p) {
    int tmask;
    if (com_port->flagOpen != OPEN) return ERR_WRITE_PORT_NOT_OPEN;
	if (buf_p == NULL) return ERR_WRITE_INV_BUFF_ADDR;
	if (count_p == NULL || *count_p <= 0) return ERR_WRITE_INV_COUNT_VAR;
	if (com_port->status != IDLE) return ERR_WRITE_DEVICE_BUSY;
	
	com_port->out_buff = buf_p;
	com_port->out_count = count_p;
	com_port->out_done = 0;
	com_port->status = WRITING;
	*(com_port->eventFlagp) = 0;
		
	outportb(COM1_BASE, *(com_port->out_buff));
	com_port->out_done++;
		
	tmask = inportb(COM1_INT_EN);
	tmask = tmask | 0x02;
	outportb(COM1_INT_EN, tmask);
	return OK;
}

/**
 */
int com_close() {
    int tmask;
	if (com_port->flagOpen != OPEN) return ERR_CLOSE_PORT_NOT_OPEN;
	com_port->flagOpen = CLOSED;
	disable();
	tmask = inportb(PIC_MASK);
	tmask = tmask | 0x10;
	outportb(PIC_MASK, tmask);
	enable();
		
	outportb(COM1_MC, 0x00);
	outportb(COM1_INT_EN, 0x00);
	setvect(COM1_INT_ID, oldfunc);
	return OK;
}
				
				
/**
 */
void interrupt com_check() {
	if(com_port->flagOpen == OPEN) {
		//gets interupt from COM1
		intType = inportb(COM1_INT_ID_REG);
		intType = intType & 0x07; 
		//which op to call
		if(intType == COM1_READ) readCom();
		else if (intType == COM1_WRITE) writeCom();
	}
	outportb(PIC_CMD, EOI);
}


/**
  */
void readCom() {
	iochar = inportb(COM1_BASE);
	if(iochar < 0 || iochar > 127)  return; //ignoring non-ascii characters
	if(com_port->status == READING) {
		if(iochar != '\r') { //if we are not done reading
			com_port->in_buff[com_port->in_done] = iochar;
			com_port->in_done++;
        }
		if(iochar == '\r' || com_port->in_done == *(com_port->in_count)) { //if were are done reading
			com_port->in_buff[com_port->in_done] = '\0';
			com_port->status = IDLE;
			*(com_port->eventFlagp) = 1;
			*(com_port->in_count) = com_port->in_done;
	    }
    }
    else { //COM not Reading
		if(com_port->ring_buffer_count < RING_SIZE ) { // Ring Buffer not full
			com_port->ring_buffer[com_port->ring_buffer_in] = iochar;
			com_port->ring_buffer_in++;
			com_port->ring_buffer_count++;
			if(com_port->ring_buffer_in > (RING_SIZE -1)) com_port->ring_buffer_in = 0; //wrap around
		}
	}
}

/**
  */
void writeCom() {
	if(com_port->status == WRITING) {
		if(com_port->out_done != *(com_port->out_count)) { //if writing is still possible
			iochar = com_port->out_buff[com_port->out_done];
			com_port->out_done++;
			outportb(COM1_BASE,iochar);
		}
		else { //done writting
			com_port->status = IDLE;
			*(com_port->eventFlagp) = 1;
			mask = inportb(COM1_INT_EN);
			mask = mask & ~0x02;
			outportb(COM1_INT_EN,mask);
		}
	}
}

/**
  */
void stop_com_request() {
	char temp;	
	com_port->status = IDLE;
	temp = inportb(COM1_INT_EN);//turns read ints back on
	temp = temp & ~0x02; //disable write
	temp = temp | 0x01; //enable read
	outportb(COM1_INT_EN, temp);
	*(com_port->eventFlagp) = 1;
}
