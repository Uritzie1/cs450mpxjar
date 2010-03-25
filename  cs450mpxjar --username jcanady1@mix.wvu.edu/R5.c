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
	int flagOpen;         //is COM open
	int* eventFlagp;	   //
	int status;		       //COM current status
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
char iochar;
char mask;
int intType;

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
int com_open (int *eflag_p, int baud_rate) 
{
	int mask;
	
	if (eflag_p = NULL) //COM_OPEN_NULL_EFLAG_P
	{
		return -101;
	}
	
	if (baud_rate <= 0) //COM_OPEN_INV_BAUD_RATE_DIV
	{
		return -102;
	}
	
	if (com_port.flagOpen == OPEN) //COM_OPEN_PORT_OPEN
	{
		return -103;
	}
	
	else 
	{
		// Initialize DCB
		int new_baud_rate;
		com_port.status = IDLE;
		com_port.flagOpen = OPEN;
		com_port.eventFlagp = eflag_p;
		com_port.ring_buffer_in = 0;
		com_port.ring_buffer_out = 0;
		com_port.ring_buffer_count = 0;
		oldfunc = getvect(COM1_INT_ID);
		
		new_baud_rate = 115200 / (long) baud_rate;
		outportb(COM1_LC, 0x80);
		outportb(COM1_BRD_LSB, new_baud_rate & 0xFF);
		outportb(COM1_BRD_MSB, (new_baud_rate >> 8) & 0xFF);
		outportb(COM1_LC, 0x03);
		disable();
		mask = inportb(PIC_MASK);
		mask = mask & 0x10;
		outportb(PIC_MASK, mask);
		enable();
		outportb(COM1_MC, 0x08);
		outportb(COM1_INT_EN, 0x01);
	}
				
	return OK;
}

/**
 */
int com_read(char* buf_p, int *count_p) 
{
	if (com_port.flagOpen != OPEN)
	{
		return -301;
	}
	
	if (buf_p == NULL)
	{
		return -302;
	}
	
	if (count_p == NULL)
	{
		return -303;
	}
	
	if (com_port.status != IDLE)
	{
		return -304;
	}
	
	else
	{
		com_port.ring_buffer_in = buf_p;
		com_port.in_count = count_p;
		com_port.in_done = 0;
		com_port.eflag_p = 0;
		disable();
		com_port.status = READ;
	}
	
   	
    return OK;
}

/**
 */
int com_write(char *buf_p, int *count_p)
{
	int mask;
	
	if (com_port.flagOpen != OPEN)
	{
		return -401;
	}
	
	if (buf_p == NULL)
	{
		return -402;
	}
	
	if (count_p == NULL)
	{
		return -403;
	}
	
	if (com_port.status != IDLE)
	{
		return -404;
	}
	
	else
	{
		com_port.out_buff = buf_p;
		com_port.out_count = count_p;
		com_port.out_done = 0;
		com_port.status = WRITING;
		com_port.eventFlagp = 0;
		
		outportb(COM1_BASE, *com_port.out_buff);
		com_port.out_buffer++;
		com_port.out_done++;
		disable();
		
		mask = inportb(COM1_INT_EN);
		mask = mask | 0x02;
		outportb(COM1_INT_EN, mask);
		enable();
	}


/**
 */
int com_close() 
{
	int mask;
	
	if (com_port.flagOpen != OPEN)
	{
		return -201;
	}
	
	else
	{
		com_port.flagOpen = CLOSED;
		disable();
		mask = inportb(PIC_MASK);
		mask = mask | 0x10;
		outportb(PIC_, mask);
		enable();
		
		outportb(COM1_MS, 0x00);
		outportb(COM1_INT_EN, 0x00);
		setvect(COM1_INT_ID, oldfunc);
	}
	
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

	iochar = inportb(COM1_BASE);
	if(com_port->status == READING){
		if(iochar != '\r') {	//if we are not done reading
			com_port->in_buff[com_port.in_done] = iochar;
			com_port->in_done++;}
		if(iochar == '\r'){ //if were are done reading
			com_port->in_buff[com_port->in_done] = '\0';
			com_port->status == IDLE;
			*(com_port-eventFlagp) == 1;
			*(com_port->in_count) = com_port->in_done;
	}
	else{//COM not Reading
		if(com_port->ring_buffer_count < RING_SIZE){ // Ring Buffer not full
			com_port->ring_buffer[ring_buffer_in] = iochar;
			com_port->ring_buffer_in++;
			com_port->ring_buffer_count++;

			if(com_port->ring_buffer_in > (RING_BUFFER_SIZE -1))//wrapping around
				com_port->ring_buffer_in = 0;
		}


	}
}

/**
  */
void writeCom() {
	if(com_port->status == WRITING)
	{
		if(com_port->out_done != *(com_put->out_count){//if writting is sitll possible
			iochar = com_port->out_buff[com_port->out_done];
			com_port->out_done++;
			outportb(COM1_BASE,iochar);
		}
		else{//done writting
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

}