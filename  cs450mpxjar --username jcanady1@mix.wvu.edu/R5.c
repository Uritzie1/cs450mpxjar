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
void interrupt serialport() {
	
}

/**
  */
void read_int() {

}

/**
  */
void write_int() {

}

/**
  */
void linestat_int() {
	
}

/**
  */
void modemstat_int() {
	
}

/**
  */
void stop_com_request() {

}
