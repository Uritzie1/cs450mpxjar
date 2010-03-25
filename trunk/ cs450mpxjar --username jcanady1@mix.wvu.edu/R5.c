/**
 * File Name: comhan.c
 * \title JAROS
 * \author Jonroy Canady
 * \author Adam Trainer
 * \author Rob Wayland   
 * \date 3/15/2010
 * \version: 5.0
 *
 * Components: err_hand, init_r1, cleanup_r1, disp_dir, comhan, terminate_mpx,
 *             help, get_Version, date, valid_date, toLowerCase, trim
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
#define COM1_READ 0x02
#define COM1_WRITE 0x04
#define IDLE 0
#define READING 1
#define WRITING 2
#define CLOSED 0
#define OPEN 1
#define RING_SIZE 128
#define COM1_ID 0x0c
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
	int flagOpen;         //
	int* eventFlagp;	   //
	int status;		       //
	char* in_buff;	       //
	int *in_count;	       //
	int in_done;	       //
	char* out_buff;        //
	int*out_count;	       //
	int out_done;	       //
	char ring_buffer[RING_SIZE];    //
	int ring_buffer_in;    //
	int ring_buffer_out;   //
	int ring_buffer_count; //
} DCB;


// Global Variables
static DCB com_port;
static void interrupt (*oldfunc) (void);
int intType;
static char temp;

// Function Prototypes
int com_open (int *eflag_p, int baud_rate);
int com_close();
int com_read(char* buf_p, int *count_p);
int com_write(char* buf_p, int *count_p);
void interrupt serialport();
void readCom();
void writeCom();
void stop_com_request();




/**
  */
int com_open (int *eflag_p, int baud_rate) {
	
	return OK;
}

/**
  */
int com_close() {

	return OK;
}

/**
  */
int com_read(char* buf_p, int *count_p) {
   	
    return OK;
}

/**
  */
int com_write(char* buf_p, int *count_p) {

	return OK;
}

/**
  */
void interrupt com_check() {

	if(com_port->flagOpen == OPEN)
	{
		//gets interupt from COM1
		tType = inportb(COM1_ID);
		tType = tType & 0x07; 
		//which op to call
		if(tType == COM1_READ) readCom();
		else if (tType == COM1_WRITE) writeCom();
	}
	
}


/**
  */
void readCom() {
	/*
Read a character from the input register
If current status is NOT reading, no process is currently requesting data, so we add to the ring buffer
If the ring buffer is full, discard the character and return;
If the ring buffer is not full, store the character in the ring buffer and increment the ring buffer indexes
Put the character in the ring buffer at the index indicated by the “ring buffer in” attribute in the DCB
If current status is reading- a process is currently requesting data so we:
Store the character in the input buffer and increment the count
Check to see if we have finished reading- this happens  if the character you just read was a ‘\r’  OR if the input buffer has been filled.  If this has occurred:
Null terminate the Input buffer
Set  device status to idle
Set the event flag- this indicates that an operation has been completed
Set input count = input done, so the calling function knows how many characters you placed into the buffer*/
}

/**
  */
void writeCom() {
	/*
	If the current status is NOT writing, Ignore the interrupt and return.
If the requestor’s output buffer has not been completely copied
Copy the next character to the output register and increment the appropriate counts
Return
Otherwise, all characters have been transferred- so we clear the interrupt
Reset the status to idle. 
Set the event  flag to indicate completion of an operation
Disable write interrupts by clearing bit 1 in the interrupt enable register.
mask = inportb(COM1_INT_EN);
mask = mask & ~0x02;
outportb(COM1_INT_EN ,mask);
*/
}
/**
  */
void stop_com_request() {

}
