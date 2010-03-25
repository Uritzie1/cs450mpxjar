/**
 * File Name: comhan.c
 * \title JAROS
 * \author Jonroy Canady
 * \author Adam Trainer
 * \author Rob Wayland   
 * \date 3/15/2010
 * \version: 5.0
 *
 * Components: 
 *
 *******************************************************************************
 * Change Log:
 *
 *        3/15/2010  JC           Original version: outline, nonfunctional    
 */

// Included ANSI C Files
#include <stdlib.h>
#include <dos.h>

// Status and Error Codes
#define OK 0
#define COM_OPEN_NULL_EFLAG_P -101
#define COM_OPEN_INV_BAUD_RATE_DIV -102
#define COM_OPEN_PORT_OPEN -103
#define COM_CLOSE_PORT_NOT_OPEN -201
#define COM_READ_PORT_NOT_OPEN -301
#define COM_READ_INV_BUFF_ADDR -302
#define COM_READ_INV_COUNT_VAR -303
#define COM_READ_DEVICE_BUSY -304
#define COM_WRITE_PORT_NOT_OPEN -401
#define COM_WRITE_INV_BUFF_ADDR -402
#define COM_WRITE_INV_COUNT_VAR -403
#define COM_WRITE_DEVICE_BUSY -404

// Constants
#define IDLE 0
#define READING 1
#define WRITING 2
#define CLOSED 0
#define OPEN 1
#define RING_BUFFER_SIZE 128
#define COM1_INT_ID 0x0c
#define COM1_BASE 0x3f8
#define COM1_INT_EN COM1_BASE+1
#define COM1_BRD_LSB COM1_BASE
#define COM1_BRD_MSB COM1_BASE+1
#define COM1_INT_ID_REG COM1_BASE+2
#define COM1_LC COM1_BASE+3
#define COM1_MC COM1_BASE+4
#define COM1_LS COM1_BASE+5
#define COM1_MS COM1_BASE+6
#define PIC_MASK 0x21
#define PIC_CMD 0x20
#define EOI 0x20

//Structures

/** \struct DCB
 */
typedef struct DCB {
	int open_flag;         //
	int* event_flag_p;	   //
	int status;		       //
	char* in_buff;	       //
	int *in_count;	       //
	int in_done;	       //
	char* out_buff;        //
	int*out_count;	       //
	int out_done;	       //
	char ring_buffer[RING_BUFFER_SIZE];    //
	int ring_buffer_in;    //
	int ring_buffer_out;   //
	int ring_buffer_count; //
} DCB;

// Global Variables
static DCB com_port;
static void interrupt (*oldfunc) (void);
static int type;
static char temp;

// Function Prototypes
int com_open (int *eflag_p, int baud_rate);
int com_close();
int com_read(char* buf_p, int *count_p);
int com_write(char* buf_p, int *count_p);
void interrupt serialport();
void read_int();
void write_int();
void linestat_int();
void modemstat_int();
void stop_com_request();




/**
 */
int com_open (int *eflag_p, int baud_rate) {
	//Variables
	int baud_rate_div, mask;
	//Check Parameters
	if(eflag_p == NULL) return COM_OPEN_NULL_EFLAG_P;
	//expanding this check to ensure compatible baud rate to those listed in R5 manual; hope this shortened logic works
	if(baud_rate != 100 && baud_rate != 150 && baud_rate != 300 && baud_rate != 600 && baud_rate != 1200 && baud_rate != 2400 && baud_rate != 4800 && baud_rate != 9600 && baud_rate != 19200) {
		return COM_OPEN_INV_BAUD_RATE_DIV;
	}
	if (com_port.open_flag == OPEN) return COM_OPEN_PORT_OPEN;
	//Set the device to open
	com_port.open_flag = OPEN;
	//Save the event flag so that the device driver can edit it
	com_port.event_flag_p = eflag_p;
	//Set the device's status to IDLE
	com_port.status = IDLE;
	//Initialize the DCB ring buffer variables
	com_port.ring_buffer_in = 0;
	com_port.ring_buffer_out = 0;
	com_port.ring_buffer_count = 0;
	//Save the old interrupt handler
	oldfunc = getvect(COM1_INT_ID);
	//Install the new handler
	setvect(COM1_INT_ID, &serialport); 
	//Compute the baud rate divisor
	baud_rate_div = 115200 / (long) baud_rate;
	//Store 0x80 in the line control register, this will allow you to change the BRD registers
	outportb(COM1_LC,0x80);
	//Set the baud rate divisor in the registers
	outportb(COM1_BRD_LSB, baud_rate_div & 0xFF);
	outportb(COM1_BRD_MSB, (baud_rate_div >> 8) & 0xFF);
	//Store 0x03 in the line control register
	outportb(COM1_LC,0x03);
	//Enable the appropriate level (for COM1) in the PIC Mask register
	disable();
	mask = inportb(PIC_MASK);
	mask = mask & ~0x10; //Number might need changed possibly to 0x10, I need to ask the Braintrust -- I agree! (mark2)
	outportb(PIC_MASK, mask);
	enable();
	//Store 0x08 in the modem control register to enable serial interrupts
	outportb(COM1_MC,0x08);
	//Store 0x01 in the interrupt enable register to enable input ready interrupts
	outportb(COM1_INT_EN,0x01);
	return 0;
}

/**
 */
int com_close() {
	//Variables
	int mask;
	//Check that the port is already open 
	if (com_port.open_flag != OPEN) return COM_CLOSE_PORT_NOT_OPEN;
	//Close the DCB
	com_port.open_flag = CLOSED;
	//Disable the appropriate level in the PIC mask register
	disable();
	mask = inportb(PIC_MASK);
	mask = mask | 0x10; //Number might need changed possibly to 0x10, I need to ask the Braintrust -- I agree! (mark2)
	outportb(PIC_MASK, mask);
	enable();
	//Loading 0x00 into the Modem Status and Interrupt Enable registers to disable serial interrupts
	outportb(COM1_MC,0x00);
	outportb(COM1_INT_EN,0x00);
	//Restore the original interrupt vector
	setvect(COM1_INT_ID, oldfunc);
	return OK;
}

/**
 */
int com_read(char* buf_p, int *count_p) {
   	// Check parameters (not null)
	if(buf_p == NULL) return COM_READ_INV_BUFF_ADDR;
	if(count_p == NULL || *count_p <= 0) return COM_READ_INV_COUNT_VAR;
	// Check device open & idle.
	if(com_port.open_flag != OPEN) return COM_READ_PORT_NOT_OPEN;
	if(com_port.status != IDLE) return COM_READ_DEVICE_BUSY;
    // Install input buffer in DCB
	// Save address of the buffer to the DCB's input buffer pointer & count.
	com_port.in_count = count_p;
	com_port.in_buff = buf_p; // buf_p is a pointer so no need for &
	// Set input done count to 0.
	com_port.in_done = 0;
    // Clear the event flag.
	*(com_port.event_flag_p) = 0; // Note-to-self event_flag_p is a pointer.
	// Disable interrupts
	disable();
	// Set status to reading
	com_port.status = READING;
	// Read character from ring buffer if characters are in it and requestor buffer isn't full
	//since com_read only executes once per read, need to use a loop to pull characters from the buffer!
	while(com_port.ring_buffer_count > 0) { //loop while the ring buffer isn't empty
		//check first to see if requested count has been reached, or next character is a '/r'
		if(*(com_port.in_count) == com_port.in_done) {//means enough characters have been copied!
			com_port.status = IDLE;
			break;
		}
		if(com_port.ring_buffer[com_port.ring_buffer_out] == '\r') { //carriage return signifies end of input!
			com_port.status = IDLE; //to prevent interrupts from copying more characters into the in_buff
			com_port.ring_buffer_out++; //advance out pointer so the '\r' isn't copied for the next com_read call
			if(com_port.ring_buffer_out > (RING_BUFFER_SIZE - 1)) com_port.ring_buffer_out = 0; //ring'n the buffer
			com_port.ring_buffer_count--;	//reduce count, obviously
			break;
		}
		//hey, looks like we actually need to copy some characters!
		com_port.in_buff[com_port.in_done] = com_port.ring_buffer[com_port.ring_buffer_out]; //copy a character
		com_port.in_done++;
		com_port.ring_buffer_count--;
		com_port.ring_buffer_out++;
		if(com_port.ring_buffer_out > (RING_BUFFER_SIZE -1)) com_port.ring_buffer_out = 0; //ring'n that buffer
	}
	// Enable interrupts
    enable();
	// Check here to see if input is over, or if there is still more needed
	if(com_port.status == IDLE) { //have grabbed needed input from ring buffer!
		com_port.in_buff[com_port.in_done] = '\0'; //null-terminate buffer
		*(com_port.in_count) = com_port.in_done; //returning number of characters written to calling process
		*(com_port.event_flag_p) = 1; //set event flag to signal end of read!
	}
	// In either case, return 0 to signify no errors have occurred. If more reading is needed, interrupt will finish
	return OK;
}

/**
  */
int com_write(char* buf_p, int *count_p) {
	int mask;
	// Check parameters (not null)
	if(buf_p == NULL) return COM_WRITE_INV_BUFF_ADDR;
	if(count_p == NULL || *count_p <= 0) return COM_WRITE_INV_COUNT_VAR;
	// Check device open & idle.
	if(com_port.open_flag != OPEN) return COM_WRITE_PORT_NOT_OPEN;
	if(com_port.status != IDLE) return COM_WRITE_DEVICE_BUSY;
	// Install output buffer in DCB
	// Save address of the buffer to the DCB's output buffer pointer & count.
	com_port.out_count = count_p;
	com_port.out_buff = buf_p; // buf_p is a pointer so no need for &
	// Set output done count to 0.
	com_port.out_done = 0;
	// Set status to writing
	com_port.status = WRITING;
	// Clear the event flag.
	*(com_port.event_flag_p) = 0; // Note-to-self event_flag_p is a pointer.
	// Write first character to com port.
	outportb(COM1_BASE, *com_port.out_buff);
	com_port.out_done++;
	// Enable write interrupts
	mask = inportb(COM1_INT_EN);
	mask = mask | 0x02;
	mask = mask & ~0x01;//attempting to disable read interrupts
	outportb(COM1_INT_EN,mask);
	return OK;
}

/**
  */
void interrupt serialport() {
	//check to see if port is open
	if(com_port.open_flag == OPEN){
		//check type of interrupt generated
		type = inportb(COM1_INT_ID_REG);
		type = type & 0x07;
		if(type == 0x02) write_int(); //write operation
		else if(type == 0x04) read_int(); //read operation
		else if(type == 0x00) modemstat_int(); //modem status interrupt
		else if(type == 0x06) linestat_int(); //line status interrupt
	}
	//clear interrupt
	outportb(PIC_CMD,EOI);
}

/**
  */
void read_int() {
	//read input character
	temp = inportb(COM1_BASE);
	if(temp < 0 || temp > 127)  return; //ignoring non-ascii characters
	if(com_port.status == READING) {
		//in a read state; write to requestor's buffer!
		//store character, increment count
		if(temp != '\r') {	//don't want to copy a CR
			com_port.in_buff[com_port.in_done] = temp;
			com_port.in_done++;
		}
		if(temp == '\r' || com_port.in_done == *(com_port.in_count)) {
			//end of input!
			com_port.in_buff[com_port.in_done] = '\0'; //null-terminate input buffer
			*(com_port.in_count) = com_port.in_done; //returning the count to requestor
			com_port.status = IDLE;
			*(com_port.event_flag_p) = 1;		
		}
		return;
	}
	else {
		//not in a read state; write to ring buffer!
		if(com_port.ring_buffer_count < RING_BUFFER_SIZE) { //space left in buffer
			//store character, increment count & next write position
			com_port.ring_buffer[com_port.ring_buffer_in] = temp;
			com_port.ring_buffer_count++;
			com_port.ring_buffer_in++;
			//if the new position is past the buffer array bounds, wrap back around
			if(com_port.ring_buffer_in > (RING_BUFFER_SIZE -1))
				com_port.ring_buffer_in = 0;
		}
		//if no space left in buffer, character is discarded
		//in either case, return to first-level handler
		return;
	}
}

/**
  */
void write_int() {
	if(com_port.status == WRITING){ //in a write state
		if(com_port.out_done != *(com_port.out_count)){ //still able to write characters
			//copying character from output buffer to output register 
			temp = com_port.out_buff[com_port.out_done];
			com_port.out_done++;
			outportb(COM1_BASE,temp);
		} 
        else { //all characters have been copied.
			//disabling write interrupts.
			//using temp here as well(shouldn't cause any problems)
			temp = inportb(COM1_INT_EN);
			temp = temp & ~0x02;
			temp = temp | 0x01; //to re-enable read interrupt
			outportb(COM1_INT_EN,temp);
			com_port.status = IDLE; //seting status back to idle
			*(com_port.event_flag_p) = 1; //setting event flag
		}
	}
}

/**
  */
void linestat_int() {
	temp = inportb(COM1_LS);
}

/**
  */
void modemstat_int() {
	temp = inportb(COM1_MS);
}

/**
  */
void stop_com_request() {
	char oldmask;
	//set to idle to stop interrupts
	com_port.status = IDLE;
	//in case a write was stopped, need to re-enable read interrupts
	oldmask = inportb(COM1_INT_EN);
	oldmask = oldmask & ~0x02; //disable write
	oldmask = oldmask | 0x01; //re-enable read
	outportb(COM1_INT_EN, oldmask);
	//set event flag
	*(com_port.event_flag_p) = 1;
}
